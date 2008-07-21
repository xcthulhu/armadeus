/*
 * linux/include/asm-arm/arch-imx/imx-alsa.h
 *
 * Alsa Driver for AIC23 and TSC2101 codecs on iMX platform boards.
 *
 * Copyright (C) 2008 Armadeus <nicolas.colombain@armadeus.com>
 * Based on the omap alsa driver
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

#ifndef __IMX_ALSA_H
#define __IMX_ALSA_H

#include <sound/driver.h>
#include <asm/arch/dma.h>
#include <asm/arch/imx-dma.h>
#include <sound/core.h>
#include <sound/pcm.h>
/*#include <asm/arch/ssi.h>*/
#include <linux/platform_device.h>
/*
 * Debug functions
 */
#undef DEBUG
//#define DEBUG

#define ERR(ARGS...) printk(KERN_ERR "{%s}-ERROR: ", __FUNCTION__);printk(ARGS);

#ifdef DEBUG
#define DPRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define ADEBUG() printk("XXX Alsa debug f:%s, l:%d\n", __FUNCTION__, __LINE__)
#define FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define FN_OUT(n) printk(KERN_INFO "[%s]: end(%u)\n",__FUNCTION__, n)
#else
#define DPRINTK(ARGS...)	/* nop */
#define ADEBUG()		/* nop */
#define FN_IN			/* nop */
#define FN_OUT(n)		/* nop */
#endif

#define DMA_BUF_SIZE	(1024 * 8)

/*
 * Buffer management for alsa and dma
 */
struct audio_stream {
	char *id;		/* identification string */
	int stream_id;		/* numeric identification */
//	int dma_dev;		/* dma number of that device */
	imx_dmach_t dma_dev;	/* device identifier for DMA */
	int dma_in_progress;
	int offset;		/* store start position of the last period in the alsa buffer */
	int active:1;		/* we are using this stream for transfer now */
	int period;		/* current transfer period */
	int periods;		/* current count of periods registered in the DMA engine */
	spinlock_t dma_lock;	/* for locking in DMA operations */
	struct snd_pcm_substream *stream;	/* the pcm stream */
	int (*hw_start)(void);  /* interface to start HW interface, e.g. McBSP */
	int (*hw_stop)(void);   /* interface to stop HW interface, e.g. McBSP */
};


/*
 * Alsa card structure
 */
struct snd_card_imx_codec {
	struct snd_card *card;
	struct snd_pcm *pcm;
	long   samplerate;
	struct audio_stream s[2];	/* playback & capture */
};

/* Codec specific information and function pointers.
 * Codec imx-alsa-tsc2101.c
 * is responsible for defining the function pointers.
 */
struct imx_alsa_codec_config {
	char 	*name;
	struct	imx_ssi_reg_cfg *mcbsp_regs_alsa;
	struct	snd_pcm_hw_constraint_list *hw_constraints_rates;
	struct	snd_pcm_hardware *snd_imx_alsa_playback;
	struct	snd_pcm_hardware *snd_imx_alsa_capture;
	void	(*codec_configure_dev)(void);
	void	(*codec_set_samplerate)(long);
	void	(*codec_clock_setup)(void);
	int	    (*codec_clock_on)(void);
	int 	(*codec_clock_off)(void);
	int	    (*get_default_samplerate)(void);
};

/*********** Mixer function prototypes *************************/
int snd_imx_mixer(struct snd_card_imx_codec *);
void snd_imx_init_mixer(void);

#ifdef CONFIG_PM
void snd_imx_suspend_mixer(void);
void snd_imx_resume_mixer(void);
#endif

int snd_imx_alsa_post_probe(struct platform_device *pdev, struct imx_alsa_codec_config *config);
int snd_imx_alsa_remove(struct platform_device *pdev);
#ifdef CONFIG_PM
int snd_imx_alsa_suspend(struct platform_device *pdev, pm_message_t state);
int snd_imx_alsa_resume(struct platform_device *pdev);
#else
#define snd_imx_alsa_suspend NULL
#define snd_imx_alsa_resume	NULL
#endif
void snd_imx_alsa_dma_interrupt(int, void *);

#endif
