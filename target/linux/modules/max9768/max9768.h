/*
 * Max9768 audio amplifier driver
 *
 * Copyright (C) 2009 Armadeus Project / Armadeus Systems
 * Author: Nicolas Colombain
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __LINUX_MAX9768_H
#define __LINUX_MAX9768_H

#define MAX9768_FILTERLESS_MODE		0xD5
#define MAX9768_CLASSIC_PWM_MODE	0xD6

struct max9768_platform_data {
	u8 filter_mode;
	int mute_pin;	/* gpio for mute ctrl */
	int shdn_pin;	/* gpio for shutdown ctrl */
	int (*init)(void);
	int (*exit)(void);
};

#endif /* __LINUX_MAX9768_H */
