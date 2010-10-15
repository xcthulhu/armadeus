/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Copyright (c) 2006, Chips & Media.  All rights reserved.
 *
 * Copyright (c) 2010, Armadeus Systems.  All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file vpu_codec.h
 *
 * @brief This file implement codec application.
 *
 * @ingroup VPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <linux/videodev.h>

#include "rtp.h"
/*#include "vpu_voip_test.h"
#include "vpu_display.h"
#include "vpu_capture.h"*/
#include "vpu_io.h"
#include "vpu_lib.h"

#include "vpu_codec.h"


typedef struct {
	int Index;
	int AddrY;
	int AddrCb;
	int AddrCr;
	int StrideY;
	int StrideC;		/* Stride Cb/Cr */

	int DispY;		/* DispY is the page aligned AddrY */
	int DispCb;		/* DispCb is the page aligned AddrCb */
	int DispCr;
	vpu_mem_desc CurrImage;	/* Current memory descriptor for user space */
} FRAME_BUF;

// extern struct codec_file multi_yuv[MAX_NUM_INSTANCE];
/******  Timing Stuff Begin******/
typedef enum evType_t {
	DEC_READ = 1,
	DEC_RD_OVER,
	DEC_START,
	DEC_STOP,
	DEC_OUT,
	DEC_OUT_OVER,
	ENC_READ,
	ENC_RD_OVER,
	ENC_START,
	ENC_STOP,
	ENC_OUT,
	ENC_OUT_OVER,
	TEST_BEGIN,
	TEST_END,

} evType;

#define MEASURE_COUNT 3000

typedef struct time_rec {
	evType tag;
	struct timeval tval;
} time_rec_t;

time_rec_t time_rec_vector[MEASURE_COUNT + 1];

static unsigned long iter;

unsigned int enc_core_time;
unsigned int enc_core_time_max;
unsigned int enc_core_time_min = 100000;
unsigned int enc_idle_time;
unsigned int enc_idle_time_max;
unsigned int enc_idle_time_min = 100000;

unsigned int dec_core_time;
unsigned int dec_core_time_max;
unsigned int dec_core_time_min = 100000;
unsigned int dec_idle_time;
unsigned int dec_idle_time_max;
unsigned int dec_idle_time_min = 100000;

unsigned int ttl_time = 0;

#define	DIFF_TIME_TV_SEC(i, j)	\
	(time_rec_vector[i].tval.tv_sec - time_rec_vector[j].tval.tv_sec)
#define	DIFF_TIME_TV_USEC(i, j)	\
	(time_rec_vector[i].tval.tv_usec - time_rec_vector[j].tval.tv_usec)
#define DIFF_TIME_US(i, j) (DIFF_TIME_TV_SEC(i, j) * 1000000 + DIFF_TIME_TV_USEC(i, j))

void timer(evType tag)
{
	if (iter >= MEASURE_COUNT)
		return;

	struct timeval tval;
	gettimeofday(&tval, NULL);
	time_rec_vector[iter].tag = tag;
	time_rec_vector[iter].tval = tval;
	iter++;
}

/******  Timing Stuff End******/

// extern struct codec_file multi_bitstream[MAX_NUM_INSTANCE];
#define EOSCHECK_APISCHEME

int quitflag = 0;

typedef struct codec_info {
	CodecInst handle;
	FRAME_BUF *FbPool;
	int FbNumber;
	vpu_mem_desc bitstream_buf;
} codec_info;

static struct codec_info enc_info;

// static pthread_mutex_t codec_mutex;
#ifdef INT_CALLBACK
static pthread_mutex_t vpu_busy_mutex;
static pthread_cond_t current_job_cond;
#endif

extern sigset_t mask;

/* FrameBuffer is a round-robin buffer for Current buffer and reference */
int FrameBufferInit(int strideY, int height,
		    FRAME_BUF * FrameBuf, int FrameNumber)
{
	int i;

	for (i = 0; i < FrameNumber; i++) {
		memset(&(FrameBuf[i].CurrImage), 0, sizeof(vpu_mem_desc));
		FrameBuf[i].CurrImage.size = (strideY * height * 3 / 2);
		IOGetPhyMem(&(FrameBuf[i].CurrImage));
		if (FrameBuf[i].CurrImage.phy_addr == 0) {
			int j;
			for (j = 0; j < i; j++) {
				IOFreeVirtMem(&(FrameBuf[j].CurrImage));
				IOFreePhyMem(&(FrameBuf[j].CurrImage));
			}
			printf("No enough mem for framebuffer!\n");
			return -1;
		}
		FrameBuf[i].Index = i;
		FrameBuf[i].AddrY = FrameBuf[i].CurrImage.phy_addr;
		FrameBuf[i].AddrCb = FrameBuf[i].AddrY + strideY * height;
		FrameBuf[i].AddrCr =
		    FrameBuf[i].AddrCb + strideY / 2 * height / 2;
		FrameBuf[i].StrideY = strideY;
		FrameBuf[i].StrideC = strideY / 2;
		FrameBuf[i].DispY = FrameBuf[i].AddrY;
		FrameBuf[i].DispCb = FrameBuf[i].AddrCb;
		FrameBuf[i].DispCr = FrameBuf[i].AddrCr;
		FrameBuf[i].CurrImage.virt_uaddr =
		    IOGetVirtMem(&(FrameBuf[i].CurrImage));
	}
	return 0;
}

void FrameBufferFree(FRAME_BUF * FrameBuf, int FrameNumber)
{
	int i;
	for (i = 0; i < FrameNumber; i++) {
		IOFreeVirtMem(&(FrameBuf[i].CurrImage));
		IOFreePhyMem(&(FrameBuf[i].CurrImage));
	}
	return;
}

FRAME_BUF *GetFrameBuffer(int index, FRAME_BUF * FrameBuf)
{
	return &FrameBuf[index];
}

int GetFrameBufIndex(FRAME_BUF ** bufPool, int poolSize, FrameBuffer * frame)
{
	int i;

	for (i = 0; i < poolSize; ++i) {
		if ((*bufPool)->AddrY == frame->bufY) {
			return i;
		}
		bufPool++;
	}
	return i;
}

#ifdef INT_CALLBACK
void vpu_test_callback(int status)
{
#if 1
	if (status & IRQ_PIC_RUN) {
#else
	if ((status & IRQ_PIC_RUN) || (status & IRQ_DEC_BUF_EMPTY)) {
#endif
		pthread_mutex_lock(&vpu_busy_mutex);
		pthread_cond_signal(&current_job_cond);
		pthread_mutex_unlock(&vpu_busy_mutex);
	}

	return;
}
#endif

static int vpu_SystemInit(void)
{
	int ret = -1;
// 	pthread_mutex_init(&codec_mutex, NULL);
#ifdef INT_CALLBACK
	pthread_mutex_init(&vpu_busy_mutex, NULL);
	pthread_cond_init(&current_job_cond, NULL);

	ret = IOSystemInit((void *)(vpu_test_callback));
#else
	ret = IOSystemInit(NULL);
#endif
	if (ret < 0) {
		printf("IO system init failed!\n");
		return -1;
	}
	return ret;
}

int vpu_SystemShutdown(void)
{
	if (enc_info.FbNumber) {
		vpu_EncClose(&(enc_info.handle));
		FrameBufferFree(enc_info.FbPool, enc_info.FbNumber);
		IOFreeVirtMem(&(enc_info.bitstream_buf));
		IOFreePhyMem(&(enc_info.bitstream_buf));
		memset(&enc_info, 0, sizeof(struct codec_info));
	}
	IOSystemShutdown();
// 	PrintTimingData();
	DPRINTF("file close end\n");

	return 0;
}

static void set_encoder_opening_parameters(EncOpenParam * enc_op_params, vpu_mem_desc *bit_stream_buf,
					int mode, int width, int height, int bitrate)
{
	enc_op_params->bitstreamBuffer = bit_stream_buf->phy_addr;
	enc_op_params->bitstreamBufferSize = bit_stream_buf->size;
	enc_op_params->bitstreamFormat = mode;
	enc_op_params->picWidth = width;
	enc_op_params->picHeight = height;
	enc_op_params->frameRateInfo = 15;
	enc_op_params->bitRate = bitrate;
	enc_op_params->initialDelay = 0;
	enc_op_params->vbvBufferSize = 0;	/* 0 = ignore 8 */
	enc_op_params->enableAutoSkip = 0;
	enc_op_params->gopSize = 0;	/* only first picture is I */
	enc_op_params->slicemode.sliceMode = 0;	/* 1 slice per picture */
	enc_op_params->slicemode.sliceSizeMode = 0;
	enc_op_params->slicemode.sliceSize = 4000;	/* not used if sliceMode is 0 */
	enc_op_params->intraRefresh = 0;
	enc_op_params->sliceReport = 0;
	enc_op_params->mbReport = 0;
	enc_op_params->mbQpReport = 0;
	enc_op_params->rcIntraQp = -1;
	enc_op_params->ringBufferEnable = 0;
	enc_op_params->dynamicAllocEnable = 0;

	if (mode == STD_MPEG4) {
		enc_op_params->EncStdParam.mp4Param.mp4_dataPartitionEnable = 0;
		enc_op_params->EncStdParam.mp4Param.mp4_reversibleVlcEnable = 0;
		enc_op_params->EncStdParam.mp4Param.mp4_intraDcVlcThr = 0;
		enc_op_params->EncStdParam.mp4Param.mp4_hecEnable = 0;
		enc_op_params->EncStdParam.mp4Param.mp4_verid = 2;
	} else if (mode == STD_H263) {
		enc_op_params->EncStdParam.h263Param.h263_annexJEnable = 0;
		enc_op_params->EncStdParam.h263Param.h263_annexKEnable = 0;
		enc_op_params->EncStdParam.h263Param.h263_annexTEnable = 0;
	} else if (mode == STD_AVC){
		enc_op_params->EncStdParam.avcParam.avc_constrainedIntraPredFlag = 0;
		enc_op_params->EncStdParam.avcParam.avc_disableDeblk = 0;
		enc_op_params->EncStdParam.avcParam.avc_deblkFilterOffsetAlpha = 0;
		enc_op_params->EncStdParam.avcParam.avc_deblkFilterOffsetBeta = 0;
		enc_op_params->EncStdParam.avcParam.avc_chromaQpOffset = 0;
		enc_op_params->EncStdParam.avcParam.avc_audEnable = 0;
		/* FMO(Flexible macroblock ordering) support */
		enc_op_params->EncStdParam.avcParam.avc_fmoEnable = 0;
		enc_op_params->EncStdParam.avcParam.avc_fmoType = 0; /* 0 - interleaved, or 1 - dispersed */
		enc_op_params->EncStdParam.avcParam.avc_fmoSliceNum = 2;
	} else {
		printf("Encoder: Invalid codec standard mode \n");
// 		return NULL;
	}
}

int gdest = PATH_NET;

static void export_bitstream(char *packet, int size)
{
	if (gdest & PATH_NET) {
		rtp_send(packet, size);
	}
}

void vpu_codec_stop(void)
{
	quitflag = 1;
}

void vpu_codec_pause(void)
{
	quitflag = 0; /* TBDL */
}


EncHandle handle = { 0 };
EncOpenParam encOP = { 0 };
EncParam encParam = { 0 };
EncHeaderParam encHeaderParam = { 0 };
EncOutputInfo outputInfo = { 0 };

int virt_bit_stream_buf;
vpu_mem_desc bit_stream_buf;

	FrameBuffer frameBuf[NUM_FRAME_BUF];
	FRAME_BUF FrameBufPool[MAX_FRAME];

int frame_index;
	int srcFrameIdx;
int image_size;
int g_initialized = 0;


/* for this test, only 1 reference */ /* no rotation */
int vpu_codec_init(int mode, int picWidth, int picHeight, int bitRate, Uint32 dest)
{
	EncInitialInfo initialInfo = { 0 };
	SearchRamParam searchPa = { 0 };
	RetCode ret = RETCODE_SUCCESS;
	int EXTRA_FB = 1;
	int stride = 0;
	Uint32 framebufWidth = 0, framebufHeight = 0;
	FRAME_BUF *pFrame[NUM_FRAME_BUF];
	int i;
	struct v4l2_buffer prp_buffer;

	image_size = picWidth * picHeight; /* in pixels */

	memset(&encParam, 0, sizeof(EncParam));
	memset(&encHeaderParam, 0, sizeof(EncHeaderParam));
	memset(&handle, 0, sizeof(EncHandle));
	memset(&encOP, 0, sizeof(EncOpenParam));
	memset(&outputInfo, 0, sizeof(EncOutputInfo));

	ret = vpu_SystemInit();
	if (ret < 0)
		return ret;

	switch (mode) {
	case STD_MPEG4:
		DPRINTF("Enc mode: MPEG4\n");
		break;
	case STD_H263:
		DPRINTF("Enc mode: H.263\n");
		break;
	case STD_AVC:
		DPRINTF("Enc mode: H.264\n");
		break;
	default:
		printf("Unknown Enc mode\n");
		return -1;
	}
	DPRINTF("width %d, height %d, bitrate %d Kbps\n", picWidth, picHeight, bitRate);

	memset(&enc_info, 0, sizeof(struct codec_info));

	/* allocate the bitstream/output buffer */
	memset(&bit_stream_buf, 0, sizeof(vpu_mem_desc));
	bit_stream_buf.size = STREAM_BUF_SIZE;
	IOGetPhyMem(&bit_stream_buf);
	virt_bit_stream_buf = IOGetVirtMem(&bit_stream_buf);

	/* framebufWidth and framebufHeight must be a multiple of 16 */
	framebufWidth = ((picWidth + 15) & ~15);  	
	framebufHeight = ((picHeight + 15) & ~15);

	/* set Encoder start/opening Parameters */
	set_encoder_opening_parameters(&encOP, &bit_stream_buf, mode, picWidth, picHeight, bitRate);

	ret = vpu_EncOpen(&handle, &encOP);
	if (ret != RETCODE_SUCCESS) {
		printf("vpu_EncOpen failed. Error code is %d \n", ret);
		goto ERR_ENC_INIT;
	}

	searchPa.searchRamAddr = DEFAULT_SEARCHRAM_ADDR;
	ret = vpu_EncGiveCommand(handle, ENC_SET_SEARCHRAM_PARAM, &searchPa);
	if (ret != RETCODE_SUCCESS) {
		printf("vpu_EncGiveCommand ( ENC_SET_SEARCHRAM_PARAM ) failed. Error code is %d \n", ret);
		goto ERR_ENC_OPEN;
	}

	ret = vpu_EncGetInitialInfo(handle, &initialInfo);
	if (ret != RETCODE_SUCCESS) {
		printf("vpu_EncGetInitialInfo failed. Error code is %d \n",
		       ret);
		goto ERR_ENC_OPEN;
	}
	DPRINTF("Enc: min buffer count= %d\n", initialInfo.minFrameBufferCount);
	printf("Initial Infos: minFrameBufferCount=%d\n", initialInfo.minFrameBufferCount);

	/* allocate the image buffer, rec buffer/ref buffer plus src buffer */
	FrameBufferInit(picWidth, picHeight,
			FrameBufPool,
			initialInfo.minFrameBufferCount + EXTRA_FB);

	enc_info.FbPool = FrameBufPool;
	enc_info.FbNumber = initialInfo.minFrameBufferCount + EXTRA_FB;
	memcpy(&(enc_info.bitstream_buf), &bit_stream_buf,
	       sizeof(struct vpu_mem_desc));
	memcpy(&(enc_info.handle), &handle, sizeof(DecHandle));

	srcFrameIdx = initialInfo.minFrameBufferCount;

	for (i = 0; i < initialInfo.minFrameBufferCount + EXTRA_FB; ++i) {
		pFrame[i] = GetFrameBuffer(i, FrameBufPool);
		frameBuf[i].bufY = pFrame[i]->AddrY;
		frameBuf[i].bufCb = pFrame[i]->AddrCb;
		frameBuf[i].bufCr = pFrame[i]->AddrCr;
	}
	
	stride = framebufWidth;
	ret = vpu_EncRegisterFrameBuffer(handle, frameBuf,
					 initialInfo.minFrameBufferCount,
					 stride);
	if (ret != RETCODE_SUCCESS) {
		printf("vpu_EncRegisterFrameBuffer failed.Error code is %d \n", ret);
		goto ERR_ENC_OPEN;
	}

	DPRINTF("Disp %x, %x, %x,\n\tStore buf %x, %x, %x\n",
		pFrame[srcFrameIdx]->DispY,
		pFrame[srcFrameIdx]->DispCb,
		pFrame[srcFrameIdx]->DispCr,
		(unsigned int)frameBuf[srcFrameIdx].bufY,
		(unsigned int)frameBuf[srcFrameIdx].bufCb,
		(unsigned int)frameBuf[srcFrameIdx].bufCr);

	frame_index = 0;
	encParam.sourceFrame = &frameBuf[srcFrameIdx];
	encParam.quantParam = 30;
	encParam.forceIPicture = 0;
	encParam.skipPicture = 0;

	memset(&prp_buffer, 0, sizeof(struct v4l2_buffer));

	/* Must put encode header before first picture encoding. */
	if (mode == STD_MPEG4) {
		encHeaderParam.headerType = VOS_HEADER;
		vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &encHeaderParam); 
		if (encOP.ringBufferEnable == 0) {
			export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
		}

		encHeaderParam.headerType = VIS_HEADER;
		vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &encHeaderParam); 
		if (encOP.ringBufferEnable == 0) {
			export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
		}

		encHeaderParam.headerType = VOL_HEADER;
		vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &encHeaderParam); 
		if (encOP.ringBufferEnable == 0) {
			export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
		}
	} else if (mode == STD_AVC) {
		encHeaderParam.headerType = SPS_RBSP;
		vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &encHeaderParam); 
		if (encOP.ringBufferEnable == 0) {
			export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
		}

		encHeaderParam.headerType = PPS_RBSP;
		vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &encHeaderParam); 
		if (encOP.ringBufferEnable == 0) {
			export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
		}
	}

	g_initialized = 1;
	goto NO_ERR;

ERR_ENC_OPEN:
	ret = vpu_EncClose(handle);
	if (ret == RETCODE_FRAME_NOT_COMPLETE) {
		vpu_EncGetOutputInfo(handle, &outputInfo);
		vpu_EncClose(handle);
	}

ERR_ENC_INIT:
	FrameBufferFree(FrameBufPool,
			initialInfo.minFrameBufferCount + EXTRA_FB);
	IOFreeVirtMem(&bit_stream_buf);
	IOFreePhyMem(&bit_stream_buf);
	DPRINTF("Enc closed\n");
	memset(&enc_info, 0, sizeof(struct codec_info));

NO_ERR:
	return ret; //&codec_thread[usr_config->index];
}

int vpu_codec_encode_next_frame(size_t frame_offset, unsigned char* image)
{
	int ret;

	if (!g_initialized) {
		printf("%s: please initialize codec first !\n", __func__);
		return -1;
	}

	/* main loop */
// 	while (1) {
	/*while (frame_index < 150)*/ {
		if (frame_index % 100 == 0) {
			DPRINTF(" Inst %d, No. %d\n",
				((EncInst *) handle)->instIndex, frame_index);
		}

		if (gdest == PATH_NET) {
			if (frame_index % 5 == 0) {
				encParam.forceIPicture = 1;
			} else {
				encParam.forceIPicture = 0;
			}
		}

		// change Rc para test
		// GOP number change
		if (frame_index == 10) {
			int newGopNum = 0;
			vpu_EncGiveCommand(handle, ENC_SET_GOP_NUMBER, &newGopNum);
		}
		// Intra Qp change
		if (frame_index == 20) {
			int newIntraQp = 0;
			vpu_EncGiveCommand(handle, ENC_SET_INTRA_QP, &newIntraQp);
		}
		// Bit Rate change
		if (frame_index == 30) {
			int newBitrate = 384000;
			vpu_EncGiveCommand(handle, ENC_SET_BITRATE, &newBitrate);
		}
		// Frame Rate change
		if (frame_index == 40) {
			int newFramerate = 30;
			vpu_EncGiveCommand(handle, ENC_SET_FRAME_RATE, &newFramerate);
			if (encOP.bitstreamFormat== STD_MPEG4) {
				encHeaderParam.headerType = VOL_HEADER;
				vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &encHeaderParam); 
				if (encOP.ringBufferEnable == 0) {
					DPRINTF("MPEG4 encHeaderParam(VOL_HEADER) addr:%p len:%d\n", bsBuf0, size0);
					export_bitstream((char *)(virt_bit_stream_buf + encHeaderParam.buf - bit_stream_buf.phy_addr), encHeaderParam.size);
				}
			}
		}
		// Intra Refresh Number change
		if (frame_index == 50) {
			int newIntraRefreshNum = 0;
			vpu_EncGiveCommand(handle, ENC_SET_INTRA_MB_REFRESH_NUMBER, &newIntraRefreshNum);
		}
		// Slice Mode change
		if (frame_index == 60) {
			EncSliceMode newSlice;
			newSlice.sliceMode = 0;
			if(!newSlice.sliceMode) {
				newSlice.sliceSizeMode	= 0;
				newSlice.sliceSize	= 0;
			} else {
				// New Slice Size Mode[0:bit number, 1:MB number]
				newSlice.sliceSizeMode = 0;
				
				newSlice.sliceSize = 3000; 
			}
			vpu_EncGiveCommand(handle, ENC_SET_SLICE_INFO, &newSlice);
		}
		// HEC Mode change
		if (frame_index == 70) {
			int newHecMode=0;
			if (newHecMode > 0) {
				vpu_EncGiveCommand(handle, ENC_ENABLE_HEC, &newHecMode);
			} else {
				vpu_EncGiveCommand(handle, ENC_DISABLE_HEC, &newHecMode);
			}
		}
#if 0
		/* get next frame to encode */
		/*ret =
		    FillYuvImageMulti(usr_config->src, usr_config->src_name,
				      frameBuf[srcFrameIdx].bufY +
				      FrameBufPool[srcFrameIdx].CurrImage.
				      virt_uaddr -
				      FrameBufPool[srcFrameIdx].CurrImage.
				      phy_addr, (void *)&prp_buffer, picWidth,
				      picHeight,
				      ((EncInst *) handle)->instIndex,
				      CODEC_READING, rot_en, output_ratio, 0);*/
// 		if (usr_config->src == PATH_EMMA) {
			frameBuf[srcFrameIdx].bufY = frame_offset;
// 			    cap_buffers[prp_buffer.index].offset;
			frameBuf[srcFrameIdx].bufCb =
			    frameBuf[srcFrameIdx].bufY + image_size;
			frameBuf[srcFrameIdx].bufCr =
			    frameBuf[srcFrameIdx].bufCb + (image_size >> 2);
#endif
		memcpy((unsigned char*)(frameBuf[srcFrameIdx].bufY + FrameBufPool[srcFrameIdx].CurrImage.virt_uaddr - FrameBufPool[srcFrameIdx].CurrImage.phy_addr),
			image,
			FrameBufPool[srcFrameIdx].CurrImage.size);
// 		memset((unsigned char*)(frameBuf[srcFrameIdx].bufY + FrameBufPool[srcFrameIdx].CurrImage.virt_uaddr - FrameBufPool[srcFrameIdx].CurrImage.phy_addr),
// 			frame_index,
// 			FrameBufPool[srcFrameIdx].CurrImage.size);

		/* To fix the MPEG4 issue on MX27 TO2 */
		CodStd codStd = ((EncInst *) handle)->CodecInfo.encInfo.openParam.bitstreamFormat;
		DPRINTF("CodStd = %d\n", codStd);
		if (codStd == STD_MPEG4 && (getChipVersion() == MX27_REV2)) {
			vpu_ESDMISC_LHD(1);
		}

		/* encode frame */
		ret = vpu_EncStartOneFrame(handle, &encParam);
		if (ret != RETCODE_SUCCESS) {
			printf("vpu_EncStartOneFrame failed. Error code is %d \n", ret);
			return ret;
		}
		timer(ENC_START);
#ifdef INT_CALLBACK
		pthread_mutex_lock(&vpu_busy_mutex);
		pthread_cond_wait(&current_job_cond, &vpu_busy_mutex);
		pthread_mutex_unlock(&vpu_busy_mutex);
#else
		vpu_WaitForInt(200);
#endif
		timer(ENC_STOP);

		/* get resulting bitstream */
		ret = vpu_EncGetOutputInfo(handle, &outputInfo);
		if (ret != RETCODE_SUCCESS) {
			printf("vpu_EncGetOutputInfo failed. Error code is %d \n", ret);
			return ret;
		}

		/* To fix the MPEG4 issue on MX27 TO2 */
		if (codStd == STD_MPEG4 && (getChipVersion() == MX27_REV2)) {
			vpu_ESDMISC_LHD(0);
		}

		/* send buffer to dest */
		export_bitstream((char *)(virt_bit_stream_buf + outputInfo.bitstreamBuffer - bit_stream_buf.phy_addr), outputInfo.bitstreamSize);

/*		if (encOP.mbQpReport == 1) {
			SaveQpReport(outputInfo.mbQpInfo, encOP.picWidth, 
				encOP.picHeight, frame_index, "encqpreport.dat" );
		}*/

		frame_index++;
	}

	return 0;
}
