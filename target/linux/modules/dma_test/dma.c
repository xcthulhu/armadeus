/*
 * Driver to test DMA throughput
 *
 * Copyright (C) 2008 Armadeus Systems <julien.boibessot@armadeus.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#define DEBUG

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <asm/types.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>        /* get_user,copy_to_user */
#include <linux/jiffies.h>

#ifdef CONFIG_ARCH_MX2
#include <mach/dma-mx1-mx2.h>
#define DMA_DIMR    0x08		/* Interrupt mask Register */
#define DMA_CCR(x)  (0x8c + ((x) << 6))	/* Control Registers */
#define CCR_FRC            (1<<1)
#define CCR_CEN            (1<<0)
#else
#include <mach/imx-dma.h>
#endif


#define DRIVER_NAME "dma-test"


struct dma_buffer {
	u32* cpu_mem;
	dma_addr_t dma_mem;
};

struct dma_buffer dma_source_buffer, dma_dest_buffer;

static unsigned int nb_todo = 0;
static unsigned int nb_done = 0;

static int channel;

unsigned int buffer_size = 512 * 1024;

static unsigned int start_time = 0;
static unsigned int stop_time = 0;


static void inline dma_test_trig_transfer(void)
{
	/* Activate interrupts for channel */
	__raw_writel(__raw_readl(DMA_BASE + DMA_DIMR) & ~(1<<channel),
			DMA_BASE + DMA_DIMR);
	/* Manually trigger DMA transfer */
	__raw_writel(__raw_readl(DMA_BASE + DMA_CCR(channel)) | CCR_FRC | CCR_CEN,
			DMA_BASE + DMA_CCR(channel));
}

/* Called by the DMA framework when a period has elapsed (scatter-gather */
/*static void dma_test_progression(int channel, void *data,
					struct scatterlist *sg)
{
	if (sg) {
		;
	}
}*/

/* called when the DMA unit has finished the buffer (single mode) */
static void dma_test_callback(int channel, void *data)
{
	if (nb_done > nb_todo) {
		stop_time = (unsigned int)jiffies;
		printk("%s: done (%d) at time 0x%08x\n", __func__, nb_done, stop_time);
		printk("0x%08x 0x%08x\n", dma_dest_buffer.cpu_mem[0], dma_dest_buffer.cpu_mem[4096]);
		printk("Tranferred %d Bytes in %d msecs\n", (buffer_size * nb_done), (stop_time - start_time)*10);
		printk(" --> %d Bytes/secs\n", ((buffer_size * nb_done) / ((stop_time - start_time)*10)) * 1000);
		nb_done = 0;
	} else {
		dma_source_buffer.cpu_mem[0] &= 0xffff0000;
		dma_source_buffer.cpu_mem[0] |= nb_done;
		nb_done++;
		dma_test_trig_transfer();
	}
}

/* Called by the DMA framework when an error has occured */
static void dma_test_err_handler(int channel, void *data, int err)
{
	printk("%s %d %d\n", __func__, channel, err);

	printk("DMA timeout on channel %d -%s%s%s%s%s\n",
		 channel,
		 err & IMX_DMA_ERR_BURST ?    " burst":"",
		 err & IMX_DMA_ERR_REQUEST ?  " request":"",
		 err & IMX_DMA_ERR_TRANSFER ? " transfer":"",
		 err & IMX_DMA_ERR_BUFFER ?   " buffer":"",
		 err & IMX_DMA_ERR_TIMEOUT ?  " hw_chain_watchdog":"");

	imx_dma_disable(channel);
}

/* configure DMA channel of a given substream */
static int dma_request(void)
{
	int err=0, chan=0;

	chan = imx_dma_request_by_prio(DRIVER_NAME, DMA_PRIO_HIGH);
	if (chan < 0) {
		printk(KERN_ERR "Unable to grab a DMA channel\n");
		err = chan;
		goto on_error_1;
	}

	err = imx_dma_setup_handlers(chan,
					dma_test_callback,
					dma_test_err_handler, NULL /* private data comes here */);
	if (err < 0) {
		printk(KERN_ERR "Unable to setup DMA handler for channel %d\n", chan);
		err = -EIO;
		goto on_error_2;
	}

/*	err = imx_dma_setup_progression_handler(chan,
						dma_test_progression);
	if (err != 0) {
		pr_err("Failed to setup the DMA progression handler\n");
		err = -EIO;
		goto on_error_2;
	} */

	printk("DMA channel %d setup\n", chan);
	imx_dma_disable(chan);

	return chan;

on_error_2:
	imx_dma_free(chan);
on_error_1:
	return err;
}


static int dma_test_proc_write( __attribute__ ((unused)) struct file *file, const char *buf, unsigned long count, void *data)
{
	int len;
	char given_param[16];
	int loops;

	if (count <= 0) {
		printk("Empty string transmitted !\n");
		return 0;
	}
	if (count > 4) {
		len = 4;
		printk("Only 4x[0-9] decimal values supported !\n");
	} else {
		len = count;
	}

	if (copy_from_user(given_param, buf, len)) {
		return -EFAULT;
	}

	loops = simple_strtol(given_param, 0, 10);
	/* start transfer */
	printk("Launching %d DMA transfers (mem to mem) at time 0x%08x\n", loops, (unsigned int)jiffies);
	nb_todo = loops;
	start_time = (unsigned int)jiffies;
	dma_test_trig_transfer();

	return len;
}



int __init dma_test_init(void)
{
	int err, dma_req;
	unsigned int burstlen, i;
	static struct proc_dir_entry *proc_active;

	/* Allocate memory buffers */
	dma_source_buffer.cpu_mem = dma_alloc_coherent(0,
					buffer_size,
					&(dma_source_buffer.dma_mem),
					GFP_DMA | GFP_KERNEL);
	dma_dest_buffer.cpu_mem = dma_alloc_coherent(0,
					buffer_size,
					&(dma_dest_buffer.dma_mem),
					GFP_DMA | GFP_KERNEL);

	if ((!dma_source_buffer.cpu_mem) || (!dma_dest_buffer.cpu_mem)) {
		printk("Failed to allocate buffers !\n");
		return -ENOMEM;
	}

	for (i = 0; i <  (buffer_size/sizeof(u32)); i++) {
		dma_source_buffer.cpu_mem[i] = i;
	}
	printk("DMA source virt: 0x%p  phys: 0x%08x\n", dma_source_buffer.cpu_mem, dma_source_buffer.dma_mem);
	memset(dma_dest_buffer.cpu_mem, 0, buffer_size);
	printk("DMA dest virt: 0x%p  phys: 0x%08x\n", dma_dest_buffer.cpu_mem, dma_dest_buffer.dma_mem);

	burstlen = 64;
	dma_req = 0; /* no hw req for mem to mem DMA */

	/* request DMA channel */
	channel = dma_request();
	if (channel < 0)
		return channel;

	/* configure i.MX DMA control register for given channel */
	err = imx_dma_config_channel(channel,
					IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR,
					IMX_DMA_MEMSIZE_32 | IMX_DMA_TYPE_LINEAR,
					dma_req, 1);
	if (err < 0) {
		printk("Cannot configure DMA for channel %d\n", channel);
	}

	/* configure DMA burst length for channel */
	imx_dma_config_burstlen(channel, burstlen);

	err = imx_dma_setup_single(channel, dma_source_buffer.dma_mem,
		     buffer_size, dma_dest_buffer.dma_mem,
		     DMA_MODE_WRITE);
	if (err < 0) {
		printk("Cannot setup DMA for channel %d\n", channel);
	}

	/* create proc files */
	proc_active = create_proc_entry( "driver/dma_test", S_IWUSR | S_IRGRP | S_IROTH, NULL);
	if ((proc_active == NULL)) {
		printk(KERN_ERR DRIVER_NAME ": Could not register one /proc file. Terminating\n");
		return -ENOMEM;
	} else {
		proc_active->write_proc = dma_test_proc_write;
	}

	return 0;
}

void __exit dma_test_exit(void)
{
	imx_dma_disable(channel);
	imx_dma_free(channel);

	remove_proc_entry("driver/dma_test", NULL);

	dma_free_coherent(0, buffer_size, dma_source_buffer.cpu_mem,
			  dma_source_buffer.dma_mem);
	dma_free_coherent(0, buffer_size, dma_dest_buffer.cpu_mem,
			  dma_dest_buffer.dma_mem);
}


module_init(dma_test_init);
module_exit(dma_test_exit);

MODULE_AUTHOR("Julien Boibessot");
MODULE_DESCRIPTION("APF27 DMA test");
MODULE_LICENSE("GPL");
