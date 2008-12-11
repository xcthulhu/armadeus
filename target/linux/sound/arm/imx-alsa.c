/*
 * sound/arm/imx-alsa.c
 *
 * Alsa Driver for i.MXL / SSI
 *
 * Copyright (C) 2008 Armadeus Systems <nicolas.colombain@armadeus.com>
 *                                     <julien.boibessot@armadeus.com>
 * Based on omap-alsa.c
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

#include <linux/platform_device.h>
#include <linux/spi/tsc2102.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include <sound/core.h>
#include <sound/pcm.h>

#include <mach/imx-alsa.h>
#include <linux/dma-mapping.h>
#include <mach/imx-dma.h>

#define TSC_MASTER

#define TRANSMIT_WM_LEVEL 8 // (in free places)
#define SSI_DMA_BLR_TRANSMIT 16 // (3*2)   // DMA Burst Length (in bytes)
#define SSI_DMA_BLR 8

MODULE_AUTHOR("Nicolas Colombain / Julien Boibessot - Armadeus Systems");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("i.MX1/L driver for ALSA");
MODULE_ALIAS("iMX_alsa_ssi");

static struct snd_card_imx_codec 	*alsa_codec		= NULL;
static struct imx_alsa_codec_config	*alsa_codec_config	= NULL;


/*
 * HW interface start and stop helper functions
 */
static int audio_ifc_start(void)
{	
	printk("Starting HW\n");
	SSI_SCSR |= SSI_TE; /* TE enable */
	return 0;
}

static int audio_ifc_stop(void)
{
	printk("Stopping HW\n");
	SSI_SCSR &= ~SSI_TE; /* TE disable */
	return 0;
}

#ifdef DEBUG
static void dump_ssi_registers(void) 
{
	printk("SSI_STX:   0x%x\n", SSI_STX);
	printk("SSI_SRX:   0x%x\n", SSI_SRX);
	printk("SSI_SCSR:  0x%x\n", SSI_SCSR);
	printk("SSI_STCR:  0x%x\n", SSI_STCR);
	printk("SSI_SRCR:  0x%x\n", SSI_SRCR);
	printk("SSI_STCCR: 0x%x\n", SSI_STCCR);
	printk("SSI_SRCCR: 0x%x\n", SSI_SRCCR);
	printk("SSI_STSR:  0x%x\n", SSI_STSR);
	printk("SSI_SFCSR: 0x%x\n", SSI_SFCSR);
	printk("SSI_STR:   0x%x\n", SSI_STR);
	printk("SSI_SOR:   0x%x\n", SSI_SOR);
}
#endif

static void imx_alsa_audio_init(struct snd_card_imx_codec *imx_alsa)
{
	/* Setup DMA stuff */
	imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].id = "Alsa imx out";
	imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].stream_id = 
		SNDRV_PCM_STREAM_PLAYBACK;
	imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev =
		IMX_DMA_CHANNELS-1;
	imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].hw_start =
	    audio_ifc_start;
	imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].hw_stop =
	    audio_ifc_stop;

	imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].id = "Alsa imx in";
	imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].stream_id =
	    SNDRV_PCM_STREAM_CAPTURE;
	imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].dma_dev =
		IMX_DMA_CHANNELS-2;
	imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].hw_start =
	    audio_ifc_start;
	imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].hw_stop =
	    audio_ifc_stop;
}

static void dma_err_handler(int channel, void *data, int errcode)
{
	printk("dma_err_handler %d %d\n",channel, errcode);

	imx_dma_disable(channel);
}

/*
 * DMA functions
 * Depends on imx-alsa-dma.c functions and (imx) dma.c
 *
 */
static int audio_dma_request(struct audio_stream *s,
			     void (*callback) (int, void *))
{
	int err=0, chan=0;
	ADEBUG();

	chan = imx_dma_request_by_prio(s->id, DMA_PRIO_HIGH);
	if (chan < 0) {
		printk(KERN_ERR "Unable to grab dma channel: %d\n", s->dma_dev);
	} else {
		s->dma_dev = chan;
		err = imx_dma_setup_handlers(s->dma_dev,
		       snd_imx_alsa_dma_interrupt,
		       dma_err_handler,
		       s);
		if (err < 0)
			printk(KERN_ERR "Unable to setup dma handler for channel %d\n", s->dma_dev);
	}
	printk("audio_dma_request done (%d)\n", s->dma_dev);
	imx_dma_disable(s->dma_dev);
	return err;
}

static int audio_dma_free(struct audio_stream *s)
{
	int err = 0;
	ADEBUG();

	imx_dma_free(s->dma_dev); // doesn't return an error if failed

	return err;
}


/*
 *  This function should calculate the current position of the dma in the
 *  buffer. It will help alsa middle layer to continue update the buffer.
 *  Its correctness is crucial for good functioning.
 */
static u_int audio_get_dma_pos(struct audio_stream *s)
{
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned int offset;
	unsigned long flags;
	ADEBUG();

	/* this must be called w/ interrupts locked as requested in dma.c */
	spin_lock_irqsave(&s->dma_lock, flags);

	/* For the current period let's see where we are */
	spin_unlock_irqrestore(&s->dma_lock, flags);

	/* Now, the position related to the end of that period */
// 	offset = bytes_to_frames(runtime, s->offset) + (runtime->period_size >> 1); //bytes_to_frames(runtime, count);
	if( s->dma_in_progress ) {
		offset = (runtime->period_size * (s->periods)) + (runtime->period_size >> 1);
		if (offset >= runtime->buffer_size)
			offset = runtime->period_size >> 1;
	} else {
		offset = (runtime->period_size * (s->periods));
		if (offset >= runtime->buffer_size)
			offset = 0;
	}
// 	printk("audio_get_dma_pos: %d (%d)\n", offset, bytes_to_frames(runtime, s->offset));
	return offset;
}

/*
 * this stops the dma and clears the dma ptrs
 */
static void audio_stop_dma(struct audio_stream *s)
{
	unsigned long flags;
	ADEBUG();

	spin_lock_irqsave(&s->dma_lock, flags);
	s->active = 0;
	s->period = 0;
	s->periods = 0;

	/* this stops the dma channel and clears the buffer ptrs */
	imx_dma_disable(s->dma_dev);

	spin_unlock_irqrestore(&s->dma_lock, flags);
}

/*
 *  Main dma routine, requests dma according where you are in main alsa buffer
 */
static void audio_process_dma(struct audio_stream *s)
{
	//#define IMX_SSI_BASE (0x00218000)  /* a retrouver depuis le plateform data !!! */
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime;
	unsigned int dma_size;
	unsigned int offset;
	int ret;
	dma_addr_t source;

	runtime = substream->runtime;
	imx_dma_disable(s->dma_dev);

	if (s->active) {
		dma_size = frames_to_bytes(runtime, runtime->period_size);
		offset = dma_size * s->period;
// 		printk("dma_size %d, off %d psize %d from 0x%x\n", dma_size, offset, runtime->period_size, (dma_addr_t)runtime->dma_area);
		snd_assert(dma_size <= DMA_BUF_SIZE,);

		source = dma_map_single(NULL,
							runtime->dma_area + offset,
							dma_size,
							DMA_TO_DEVICE);
		if (dma_mapping_error((struct device*)NULL, source)) {
			printk("Unable to map DMA buffer\n");
			return;
		}

		ret = imx_dma_setup_single(s->dma_dev,
				source, dma_size,
				0x00218000, DMA_MODE_WRITE);
		if (ret) {
			printk(KERN_ERR
			       "audio_process_dma: cannot queue DMA buffer (%i)\n", ret);
			return;
		}
		imx_dma_enable(s->dma_dev);

		s->dma_in_progress = 1;
		s->period++;
		s->period %= runtime->periods;
// 		s->periods++;
		s->offset = offset;

	} else {
		printk("stream not active\n");
	}
}

/*
 *  This is called when dma IRQ occurs at the end of each transmited block
 */
void snd_imx_alsa_dma_interrupt(int channel, void *data)
{
	struct audio_stream *s = data;
	struct snd_pcm_substream *substream = s->stream;
	struct snd_pcm_runtime *runtime;
// 	unsigned int dma_size;
// 	unsigned int offset;
// 	unsigned long temp;
// 	int ret, i;
// 	dma_addr_t source;

	runtime = substream->runtime;
	
 	//printk("[%x] <dma %d>\n",IMX_TCN(IMX_TIM2_BASE), channel);
/*	// Reset DMA channel
	CCR(channel) &= ~CCR_CEN; TO REMOVE
	CCR(channel) |= CCR_CEN;*/
	/* 
	 * If we are getting a callback for an active stream then we inform
	 * the PCM middle layer we've finished a period
	 */
	if (s->active)
		snd_pcm_period_elapsed(s->stream);

	spin_lock(&s->dma_lock);
	s->dma_in_progress = 0;
	s->periods++;
	s->periods %= runtime->periods;

// 	if (s->periods > 0) 
// 		s->periods--;

	/* Trig next DMA transfer */
	audio_process_dma(s);
	spin_unlock(&s->dma_lock);
}

/*
 * Alsa section
 * PCM settings and callbacks
 */
static int snd_imx_alsa_trigger(struct snd_pcm_substream * substream, int cmd)
{
	struct snd_card_imx_codec *chip =
	    snd_pcm_substream_chip(substream);
	int stream_id = substream->pstr->stream;
	struct audio_stream *s = &chip->s[stream_id];
	int err = 0;
	u32 temp;
	
	/* note local interrupts are already disabled in the midlevel code */

	spin_lock(&s->dma_lock);
	switch (cmd) {
		case SNDRV_PCM_TRIGGER_START:
			/* requested stream startup */
			// clear SSI underrun
			temp = SSI_SCSR;
			SSI_STX = 0x0000;
	
			s->active = 1;
			s->dma_in_progress = 0;
			audio_process_dma(s);
			SSI_SCSR |= SSI_TE;
			break;

		case SNDRV_PCM_TRIGGER_STOP:
			/* requested stream shutdown */
			audio_stop_dma(s);
			SSI_SCSR &= ~SSI_TE;
			break;

		default:
			printk("Not supported (yet?) ALSA trigger: 0x%x\n", cmd);
			err = -EINVAL;
			break;
	}
	spin_unlock(&s->dma_lock);
	
	return err;
}

static int snd_imx_alsa_prepare(struct snd_pcm_substream * substream)
{
	struct snd_card_imx_codec *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct audio_stream *s = &chip->s[substream->pstr->stream];
	
	/* set requested samplerate */
	alsa_codec_config->codec_set_samplerate(runtime->rate);
	chip->samplerate = runtime->rate;

	s->period = 0;
	s->periods = 0;

	return 0;
}

static snd_pcm_uframes_t snd_imx_alsa_pointer(struct snd_pcm_substream *substream)
{
	struct snd_card_imx_codec *chip = snd_pcm_substream_chip(substream);

	ADEBUG();	
	return audio_get_dma_pos(&chip->s[substream->pstr->stream]);
}

static int snd_card_imx_alsa_open(struct snd_pcm_substream * substream)
{
	struct snd_card_imx_codec *chip =
	    snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int stream_id = substream->pstr->stream;
	int err;
	
	printk ("%s substream @: 0x%x\n", __func__, (unsigned int)substream);
	chip->s[stream_id].stream = substream;
	alsa_codec_config->codec_clock_on();
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		runtime->hw = *(alsa_codec_config->snd_imx_alsa_playback);
	}
	else{ 
		runtime->hw = *(alsa_codec_config->snd_imx_alsa_capture);
		printk ("capture not supported\n");
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime,
					   SNDRV_PCM_HW_PARAM_PERIODS)) < 0) 
		return err;
	
	printk ("playback supported %d\n",runtime->hw.rate_min);
	if ((err = snd_pcm_hw_constraint_list(runtime,
					0,
					SNDRV_PCM_HW_PARAM_RATE,
					alsa_codec_config->hw_constraints_rates)) < 0) 
		return err;
	printk("success\n");
	return 0;
}

static int snd_card_imx_alsa_close(struct snd_pcm_substream * substream)
{
	struct snd_card_imx_codec *chip = snd_pcm_substream_chip(substream);
	
	ADEBUG();
	alsa_codec_config->codec_clock_off();
	chip->s[substream->pstr->stream].stream = NULL;
	
	return 0;
}

/* HW params & free */
static int snd_imx_alsa_hw_params(struct snd_pcm_substream * substream,
				   struct snd_pcm_hw_params * hw_params)
{
	return snd_pcm_lib_malloc_pages(substream,
					params_buffer_bytes(hw_params));
}

static int snd_imx_alsa_hw_free(struct snd_pcm_substream * substream)
{
	return snd_pcm_lib_free_pages(substream);
}

/* pcm operations */
static struct snd_pcm_ops snd_card_imx_alsa_playback_ops = {
	.open =		snd_card_imx_alsa_open,
	.close =	snd_card_imx_alsa_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_imx_alsa_hw_params,
	.hw_free =	snd_imx_alsa_hw_free,
	.prepare =	snd_imx_alsa_prepare,
	.trigger =	snd_imx_alsa_trigger,
	.pointer =	snd_imx_alsa_pointer,
};

static struct snd_pcm_ops snd_card_imx_alsa_capture_ops = {
	.open =		snd_card_imx_alsa_open,
	.close =	snd_card_imx_alsa_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_imx_alsa_hw_params,
	.hw_free =	snd_imx_alsa_hw_free,
	.prepare =	snd_imx_alsa_prepare,
	.trigger =	snd_imx_alsa_trigger,
	.pointer =	snd_imx_alsa_pointer,
};

/*
 *  Alsa init and exit section
 *  
 *  Inits pcm alsa structures, allocate the alsa buffer, suspend, resume
 */
static int __init snd_card_imx_alsa_pcm(struct snd_card_imx_codec *imx_alsa, 
					int device)
{
	struct snd_pcm *pcm;
	int err;
	
	ADEBUG();
	if ((err = snd_pcm_new(imx_alsa->card, "IMX PCM", device, 1, 1, &pcm)) < 0)
		return err;

	/* sets up initial buffer with continuous allocation */
	snd_pcm_lib_preallocate_pages_for_all(pcm,
					      SNDRV_DMA_TYPE_CONTINUOUS,
					      snd_dma_continuous_data
					      (GFP_KERNEL),
					      8 * 1024, 8 * 1024);

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK,	
					&snd_card_imx_alsa_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE,
					&snd_card_imx_alsa_capture_ops);
	pcm->private_data = imx_alsa;
	pcm->info_flags = 0;
	strcpy(pcm->name, "imx alsa pcm");

	imx_alsa_audio_init(imx_alsa);

	/* request DMA channels */
	audio_dma_request(&imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK],
			  snd_imx_alsa_dma_interrupt);
	audio_dma_request(&imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE],
			  snd_imx_alsa_dma_interrupt);

	/* configure i.MXL DMA control register for given channels */
	CCR(imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev) =
				CCR_DMOD_FIFO |
				CCR_SMOD_LINEAR |
				CCR_SSIZ_32 | CCR_DSIZ_16 |
				CCR_REN;
	CCR(imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].dma_dev) =
				CCR_DMOD_LINEAR |
				CCR_SMOD_FIFO |
				CCR_SSIZ_16 | CCR_DSIZ_32 |
				CCR_REN;
	/* configure i.MXL DMA request channel */
	RSSR(imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev) = DMA_REQ_SSI_T;
	RSSR(imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].dma_dev) = DMA_REQ_SSI_R;
	/* configure DMA burst length per channel */
	BLR(imx_alsa->s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev) = SSI_DMA_BLR_TRANSMIT;
	BLR(imx_alsa->s[SNDRV_PCM_STREAM_CAPTURE].dma_dev) = SSI_DMA_BLR;

	/* Reset SSI before configuring it */
	SSI_SCSR &= ~SSI_EN;
	SSI_SCSR |= SSI_EN;
	/* SSI FIFO watermark */
	SSI_SFCSR = SSI_RFWM(8) | SSI_TFWM(TRANSMIT_WM_LEVEL);
#ifdef TSC_MASTER
	/* Clocks */
	SSI_STCCR = SSI_WL_16 | SSI_DC(0);
	SSI_SRCCR = SSI_WL_16 | SSI_DC(0);
	/* Control */
	SSI_SCSR = SSI_I2S_SLAVE_MODE | SSI_SYN | SSI_EN;
	/* Transmit config */
	SSI_STCR = SSI_MAE | SSI_FEN | SSI_SCKP | SSI_FSI | SSI_EFS;
#else
	/* Clocks */
	SSI_STCCR = 0x00006103;
	SSI_SRCCR = 0x00006103;
	/* Transmit config */
	SSI_STCR = SSI_MAE | SSI_FEN | SSI_FDIR | SSI_DIR | SSI_SCKP | SSI_FSI | SSI_EFS;
	/* Control */
	SSI_SCSR = /*SSI_SYS_CLK_EN |*/ SSI_I2S_MASTER_MODE | SSI_SYN | /*SSI_TE |*/ SSI_EN;
#endif // TSC_MASTER

	imx_alsa->pcm = pcm;

	return 0;
}


#ifdef CONFIG_PM
/*
 * Driver suspend/resume - calls alsa functions. Some hints from aaci.c
 */
int snd_imx_alsa_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_card_imx_codec *chip;
	struct snd_card *card = platform_get_drvdata(pdev);
	
	if (card->power_state != SNDRV_CTL_POWER_D3hot) {
		chip = card->private_data;
		if (chip->card->power_state != SNDRV_CTL_POWER_D3hot) {
			snd_power_change_state(chip->card, SNDRV_CTL_POWER_D3hot);
			snd_pcm_suspend_all(chip->pcm);
			/* Mutes and turn clock off */
			alsa_codec_config->codec_clock_off();
			snd_imx_suspend_mixer();
		}
	}
	return 0;
}

int snd_imx_alsa_resume(struct platform_device *pdev)
{
	struct snd_card_imx_codec *chip;
	struct snd_card *card = platform_get_drvdata(pdev);

	if (card->power_state != SNDRV_CTL_POWER_D0) {				
		chip = card->private_data;
		if (chip->card->power_state != SNDRV_CTL_POWER_D0) {
			snd_power_change_state(chip->card, SNDRV_CTL_POWER_D0);
			alsa_codec_config->codec_clock_on();
			snd_imx_resume_mixer();
		}
	}
	return 0;
}

#endif	/* CONFIG_PM */

void snd_imx_alsa_free(struct snd_card * card)
{
	struct snd_card_imx_codec *chip = card->private_data;
	
	/*
	 * Turn off codec after it is done.
	 * Can't do it immediately, since it may still have
	 * buffered data.
	 */
	schedule_timeout_interruptible(2);

/*	omap_mcbsp_stop(AUDIO_MCBSP);
	omap_mcbsp_free(AUDIO_MCBSP);
*/
	audio_dma_free(&chip->s[SNDRV_PCM_STREAM_PLAYBACK]);
	audio_dma_free(&chip->s[SNDRV_PCM_STREAM_CAPTURE]);
}

/* module init & exit */

/* 
 * Inits alsa soundcard structure.
 * Called by the probe method in codec after function pointers has been set.
 */
int snd_imx_alsa_post_probe(struct platform_device *pdev, struct imx_alsa_codec_config *config)
{
	int err = 0;
	int def_rate;
	struct snd_card *card;
	
	alsa_codec_config	= config;

	alsa_codec_config->codec_clock_setup();
	alsa_codec_config->codec_clock_on(); 

	if (alsa_codec_config && alsa_codec_config->codec_configure_dev)
		alsa_codec_config->codec_configure_dev();

	alsa_codec_config->codec_clock_off();

	/* create the soundcard */
	card = snd_card_new(-1, "IMX-ALSA", THIS_MODULE, sizeof(alsa_codec));
	if (card == NULL)
		goto nodev1;

	alsa_codec = kcalloc(1, sizeof(*alsa_codec), GFP_KERNEL);
	if (alsa_codec == NULL)
		goto nodev2;

	card->private_data = (void *)alsa_codec;
	card->private_free = snd_imx_alsa_free;
	alsa_codec->card	= card;
	def_rate		= alsa_codec_config->get_default_samplerate(); 
	alsa_codec->samplerate	= def_rate;
	spin_lock_init(&alsa_codec->s[0].dma_lock);
	spin_lock_init(&alsa_codec->s[1].dma_lock);

	/* mixer */
	if ((err = snd_imx_mixer(alsa_codec)) < 0)
		goto nodev3;

	/* PCM */
	if ((err = snd_card_imx_alsa_pcm(alsa_codec, 0)) < 0)
		goto nodev3;
	strcpy(card->driver, "IMXALSA");
	strcpy(card->shortname, "ARMADEUS IMX_ALSA");
	sprintf(card->longname, "ARMADEUS IMX_ALSA");

	snd_imx_init_mixer();
	snd_card_set_dev(card, &pdev->dev);

	/* Register the created soundcard */
	if ((err = snd_card_register(card)) == 0) {
		printk(KERN_INFO "audio support initialized\n");
		platform_set_drvdata(pdev, card);
		return 0;
	}

nodev3:
	kfree(alsa_codec);
nodev2:	
	snd_card_free(card);
nodev1:

	return err;
}

int snd_imx_alsa_remove(struct platform_device *pdev)
{
	struct snd_card *card = platform_get_drvdata(pdev);
	struct snd_card_imx_codec *chip = card->private_data;
	
	snd_card_free(card);

	alsa_codec = NULL;
	card->private_data = NULL;
	kfree(chip);
	
	platform_set_drvdata(pdev, NULL);
	
	return 0;
}

