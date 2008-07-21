/*  
 * linux/sound/arm/imx/imx-alsa-dma.h
 *
 * Common audio DMA handling for the iMX processors
 *
 * Copyright (C) 2008 Nicolas Colombain <nicolas.colombain@armadeus.com>
 * 
 * Copyright (C) 2006 Mika Laitio <lamikr@cc.jyu.fi>
 *
 * Copyright (C) 2005 Instituto Nokia de Tecnologia - INdT - Manaus Brazil
 * 
 * Copyright (C) 2004 Texas Instruments, Inc.
 *
 * Copyright (C) 2000, 2001 Nicolas Pitre <nico@cam.org>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef __IMX_AUDIO_ALSA_DMA_H
#define __IMX_AUDIO_ALSA_DMA_H

/************************** INCLUDES *************************************/

#include <asm/arch/imx-alsa.h>

/************************** GLOBAL DATA STRUCTURES *********************************/

typedef void (*dma_callback_t) (int lch, u16 ch_status, void *data);

/**************** ARCH SPECIFIC FUNCIONS *******************************************/

void imx_clear_alsa_sound_dma(struct audio_stream * s);

/*int imx_request_alsa_sound_dma(int device_id, const char *device_name,
			   void *data, int **channels);*/
//int imx_free_alsa_sound_dma(void *data, int **channels);

int imx_start_alsa_sound_dma(struct audio_stream *s, dma_addr_t dma_ptr,  u_int dma_size);

void imx_stop_alsa_sound_dma(struct audio_stream *s);

#endif

