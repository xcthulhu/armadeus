/*
 *  Armadeus V4L2/SDL video capture example
 *
 * Inspired a lot from:
 * - http://www.linuxtv.org/downloads/video4linux/API/V4L2_API/v4l2spec/capture.c
 * - http://freshmeat.net/projects/libv4l
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#include <SDL/SDL.h>		/* TODO: make it compatible with Framebuffer only */


#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

#define CAM_WIDTH 320
#define CAM_HEIGHT 240

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = NULL;
static io_method	io		= IO_METHOD_MMAP;
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

SDL_Surface *screen = NULL, *image = NULL;

struct camera {
	unsigned int width;
	unsigned int height;
	unsigned int pixelformat;
	unsigned int bytesperline;
	unsigned int sizeimage;
};

struct camera mycamera;


static void errno_exit(const char *s)
{
        fprintf (stderr, "%s error %d, %s\n",
                 s, errno, strerror (errno));

        exit (EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

static Uint16 inline yuv2rgb(int Y, int U, int V)
{
	int R, G, B;

	R = Y + 1.4026 * (V-128);
	G = Y - 0.3444 * (U-128) - 0.7144 * (V-128);
	B = Y + 1.7730 * (U-128);

	if (R > 255) R = 255;
	if (G > 255) G = 255;
	if (B > 255) B = 255;
	if (R < 0) R = 0;
	if (G < 0) G = 0;
	if (B < 0) B = 0;

	return ((R>>3) << 11) | ((G>>2) << 5) | (B>>3);
}

#define STRIP(X) (X = X > 255 ? 255 : (X < 0) ? 0 : X)

static Uint8 inline YUV2R(int Y, int U, int V)
{
	int R;

	R = Y + 1.4026 * (V-128);
	STRIP(R);

	return R;
}

static Uint8 inline YUV2G(int Y, int U, int V)
{
	int G;

	G = Y - 0.3444 * (U-128) - 0.7144 * (V-128);
	STRIP(G);

	return G;
}

static Uint16 inline YUV2B(int Y, int U, int V)
{
	int B;

	B = Y + 1.7730 * (U-128);
	STRIP(B);

	return B;
}

static void yuv422_to_rgb565(void *yuv, int size, void *rgb)
{
	unsigned int Y, Y2, U, V, yuv32, rgb32;
	int R, G, B;
	int i;
	Uint32 *dest = (Uint32*) rgb;
	Uint32 *src = (Uint32*) yuv;

	for (i=size/4; i; i--)
	{
		yuv32 = *(src++);

		/* packed format */
		Y = (yuv32 & 0x000000ff);
		U = (yuv32 & 0x0000ff00) >> 8;
		Y2 = (yuv32 & 0x00ff0000) >> 16;
		V = (yuv32 & 0xff000000) >> 24;

		R = YUV2R(Y, U, V);
		G = YUV2G(Y, U, V);
		B = YUV2B(Y, U, V);
		rgb32 = ((R>>3) << 11) | ((G>>2) << 5) | (B>>3);
// 		rgb32 = yuv2rgb(Y, U, V);

		R = YUV2R(Y2, U, V);
		G = YUV2G(Y2, U, V);
		B = YUV2B(Y2, U, V);
		rgb32 |= (((R>>3) << 11) | ((G>>2) << 5) | (B>>3)) << 16;
// 		rgb32 |= yuv2rgb(Y2, U, V) << 16;

		*(dest++) = rgb32;
	}
}

/* Only grayscale conversion for the moment
   As yuv420 is slightly complicated to decode it would be worth using libv4l
*/
static void yuv420_to_rgb565(void *yuv, int size, void *rgb)
{
	Uint8 yuv8;
	int R, G, B;
	int i;
	int nbpix = mycamera.width * mycamera.height;
	Uint16 *dest = (Uint16*) rgb;
	Uint8 *src = (Uint8*) yuv;

	nbpix = mycamera.width * mycamera.height;
	for (i=nbpix; i; i--) {
		yuv8 = *(src++);
		R = G = B = yuv8;
		*(dest++) = ((R>>3) << 11) | ((G>>2) << 5) | (B>>3);;
	}
}

static void simple_copy(void *yuv, int size, void* rgb)
{
	memcpy(rgb, yuv, size);
}

static void process_image(/*const */void *pCaptured)
{
	SDL_Rect update_rec;

	if (mycamera.pixelformat == V4L2_PIX_FMT_YUYV) {
		yuv422_to_rgb565(pCaptured, mycamera.sizeimage, image->pixels);
	} else if (mycamera.pixelformat == V4L2_PIX_FMT_YUV420) {
		yuv420_to_rgb565(pCaptured, mycamera.sizeimage, image->pixels);
	} else {
		printf("Unknown image format\n");
		simple_copy(pCaptured, mycamera.sizeimage, image->pixels);
	}

	/* Blit converted image on screen */
	update_rec.x = 0;
	update_rec.y = 0;
	update_rec.w = mycamera.width;
	update_rec.h = mycamera.height;
	SDL_BlitSurface(image, NULL, screen, &update_rec);
	SDL_Flip(screen);
}

static int read_frame(void)
{
        struct v4l2_buffer buf;
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
    		if (-1 == read (fd, buffers[0].start, buffers[0].length)) {
            		switch (errno) {
            		case EAGAIN:
                    		return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("read");
			}
		}

    		process_image (buffers[0].start);

		break;

	case IO_METHOD_MMAP:
		CLEAR (buf);

            	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            	buf.memory = V4L2_MEMORY_MMAP;

    		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
            		switch (errno) {
            		case EAGAIN:
                    		return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

                assert (buf.index < n_buffers);

	        process_image (buffers[buf.index].start);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF");

		break;

	case IO_METHOD_USERPTR:
		CLEAR (buf);

    		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

		for (i = 0; i < n_buffers; ++i)
			if (buf.m.userptr == (unsigned long) buffers[i].start
			    && buf.length == buffers[i].length)
				break;

		assert (i < n_buffers);

    		process_image ((void *) buf.m.userptr);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF");

		break;
	}

	return 1;
}

static void mainloop(void)
{
	int go_on = 1;
	SDL_Event event;

        while (go_on) {
                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO (&fds);
                        FD_SET (fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;

                                errno_exit ("select");
                        }

                        if (0 == r) {
                                fprintf (stderr, "select timeout\n");
                                exit (EXIT_FAILURE);
                        }

			if (read_frame ())
                    		break;
	
			/* EAGAIN - continue select loop. */
                }
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
				case SDL_KEYDOWN:
				case SDL_QUIT:
					go_on = 0;
			}
		}
// 		SDL_Delay(200);
        }
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
			errno_exit ("VIDIOC_STREAMOFF");

		break;
	}
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
            		struct v4l2_buffer buf;

        		CLEAR (buf);

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_MMAP;
        		buf.index       = i;

        		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
                    		errno_exit ("VIDIOC_QBUF");
		}
		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
            		struct v4l2_buffer buf;

        		CLEAR (buf);

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_USERPTR;
			buf.index       = i;
			buf.m.userptr	= (unsigned long) buffers[i].start;
			buf.length      = buffers[i].length;

			if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
                    		errno_exit ("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;
	}
}

static void uninit_device(void)
{
        unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		free (buffers[0].start);
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap (buffers[i].start, buffers[i].length))
				errno_exit ("munmap");
		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free (buffers[i].start);
		break;
	}

	free (buffers);
}

static void init_read(unsigned int buffer_size)
{
        buffers = calloc (1, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

	buffers[0].length = buffer_size;
	buffers[0].start = malloc (buffer_size);

	if (!buffers[0].start) {
    		fprintf (stderr, "Out of memory\n");
            	exit (EXIT_FAILURE);
	}
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf (stderr, "%s does not support "
                                 "memory mapping\n", dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf (stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit (EXIT_FAILURE);
        }

        buffers = calloc (req.count, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit ("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap (NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit ("mmap");
        }
}

static void init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
        unsigned int page_size;

        page_size = getpagesize ();
        buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf (stderr, "%s does not support "
                                 "user pointer i/o\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        buffers = calloc(4, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = memalign (/* boundary */ page_size,
                                                     buffer_size);

                if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
        }
}

static Uint32 get_a_supported_pix_fmt(void)
{
	struct v4l2_fmtdesc fmtdesc;
	int i;

	for (i = 0;; i++) {
		CLEAR(fmtdesc);
		fmtdesc.index = i;
		fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc))
			break;

		if ((fmtdesc.pixelformat == V4L2_PIX_FMT_YUYV) || (fmtdesc.pixelformat == V4L2_PIX_FMT_YUV420)) {
			fprintf(stdout, "Using img format (%d 0x%08x): ", i, fmtdesc.pixelformat);
			if (fmtdesc.pixelformat == V4L2_PIX_FMT_YUYV)
				fprintf(stdout, "YUYV (YUV:4:2:2)\n");
			if (fmtdesc.pixelformat == V4L2_PIX_FMT_YUV420)
				fprintf(stdout, "YUV:4:2:0\n");
			return fmtdesc.pixelformat;
		}
	}

	return 0;
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
	unsigned int min;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
			fprintf(stderr, "%s does not support read i/o\n",
				 dev_name);
			exit(EXIT_FAILURE);
		}

		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
			fprintf(stderr, "%s does not support streaming i/o\n",
				 dev_name);
			exit(EXIT_FAILURE);
		}

		break;
	}


        /* Select video input, video standard and tune here. */


	CLEAR(cropcap);
        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {	
                /* Errors ignored. */
        }


        CLEAR(fmt);
	fmt.fmt.pix.pixelformat = get_a_supported_pix_fmt();
	if (!fmt.fmt.pix.pixelformat) {
		fprintf(stderr, "%s does not support requested img formats\n",
				dev_name);
	}

        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = CAM_WIDTH; 
        fmt.fmt.pix.height      = CAM_HEIGHT;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
                errno_exit ("VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	mycamera.width = fmt.fmt.pix.width;
	mycamera.height = fmt.fmt.pix.height;
	mycamera.pixelformat = fmt.fmt.pix.pixelformat;
	mycamera.bytesperline = fmt.fmt.pix.bytesperline;
	mycamera.sizeimage = fmt.fmt.pix.sizeimage;

	switch (io) {
	case IO_METHOD_READ:
		init_read (fmt.fmt.pix.sizeimage);
		break;

	case IO_METHOD_MMAP:
		init_mmap ();
		break;

	case IO_METHOD_USERPTR:
		init_userp (fmt.fmt.pix.sizeimage);
		break;
	}
}

static void close_device(void)
{
        if (-1 == close (fd))
	        errno_exit("close");

        fd = -1;
}

static void open_device(void)
{
        struct stat st; 

        if (-1 == stat (dev_name, &st)) {
                fprintf (stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR (st.st_mode)) {
                fprintf(stderr, "%s is no device\n", dev_name);
                exit(EXIT_FAILURE);
        }

        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit(EXIT_FAILURE);
        }
}

static void usage(FILE *fp, int argc, char **argv)
{
        fprintf (fp,
                 "Usage: %s [options]\n\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video0]\n"
                 "-h | --help          Print this message\n"
                 "-m | --mmap          Use memory mapped buffers\n"
                 "-r | --read          Use read() calls\n"
                 "-u | --userp         Use application allocated buffers\n"
                 "",
		 argv[0]);
}

static const char short_options [] = "d:hmru";

static const struct option long_options [] = {
        { "device",     required_argument,      NULL,           'd' },
        { "help",       no_argument,            NULL,           'h' },
        { "mmap",       no_argument,            NULL,           'm' },
        { "read",       no_argument,            NULL,           'r' },
        { "userp",      no_argument,            NULL,           'u' },
        { 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
        dev_name = "/dev/video0";

	atexit(SDL_Quit);
        for (;;) {
                int index;
                int c;
                
                c = getopt_long(argc, argv,
                                 short_options, long_options,
                                 &index);

                if (-1 == c)
                        break;

                switch (c) {
                case 0: /* getopt_long() flag */
                        break;

                case 'd':
                        dev_name = optarg;
                        break;

                case 'h':
                        usage(stdout, argc, argv);
                        exit (EXIT_SUCCESS);

                case 'm':
                        io = IO_METHOD_MMAP;
			break;

                case 'r':
                        io = IO_METHOD_READ;
			break;

                case 'u':
                        io = IO_METHOD_USERPTR;
			break;

                default:
                        usage(stderr, argc, argv);
                        exit(EXIT_FAILURE);
                }
        }

        open_device();
        init_device();

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, (SDL_HWSURFACE));
	image = SDL_CreateRGBSurface(/*SDL_HWSURFACE*/0, mycamera.width, mycamera.height, 16, 0, 0, 0, 0);
// 	printf("image fmt: RGB%d%d%d\n", image->format->Rshift, image->format->Gshift, image->format->Bshift);

        start_capturing();
        mainloop();
        stop_capturing();

        uninit_device();
        close_device();

        exit(EXIT_SUCCESS);

        return 0;
}
