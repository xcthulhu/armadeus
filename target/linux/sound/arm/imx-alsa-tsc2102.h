/*
 * sound/arm/imx-alsa-tsc2102.h
 * 
 * Alsa codec driver for TSC2102 chip for IMX1 platforms.
 *
 * Copyright (c) 2008 Jorasse  <jorasse@armadeus.com>
 * Code based on the TSC2101 ALSA driver for omap platforms.
 * Copyright (c) 2006 Andrzej Zaborowski  <balrog@zabor.org>
 * Code based on the TSC2101 ALSA driver.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#ifndef IMX_ALSA_TSC2102_H_
#define IMX_ALSA_TSC2102_H_

#include <linux/types.h>

/* Define to set the TSC as the master, otherwise slave */
#define TSC_MASTER

/*
 * Audio related macros
 */
#ifndef DEFAULT_BITPERSAMPLE
#define DEFAULT_BITPERSAMPLE		16
#endif

#define DEFAULT_SAMPLE_RATE		48000
#define CODEC_CLOCK			16000000
#define AUDIO_MCBSP			OMAP_MCBSP1 /* to remove */

/*
 * ALSA mixer related macros
 */
#define OUTPUT_VOLUME_MIN		0x7f	/* 1111111 = -63.5 dB */
#define OUTPUT_VOLUME_MAX		0x00	/* 0000000 */
#define OUTPUT_VOLUME_RANGE		(OUTPUT_VOLUME_MIN - OUTPUT_VOLUME_MAX)

#define DEFAULT_OUTPUT_VOLUME		90	/* Default output volume */

#endif	/* IMX_ALSA_TSC2102_H_ */

