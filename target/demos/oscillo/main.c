/*
 * (Kind of) Oscilloscope for the APF  
 *
 * Copyright (C) 2009 <julien.boibessot@armadeus.com>
 *                    Armadeus Project / Armadeus Systems
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h> /* sin() */

#include "lcd.h"


int fbfd;

static void cleanup(void)
{
	close_lcd(fbfd);
}

#define MAX_CHANNELS 8

int main(void /*int argc, char **argv*/)
{
	/*int x[MAX_CHANNELS] = {0, 10, 20, 30, 40, 50, 60, 70};*/
	int y[MAX_CHANNELS];
	int time = 0, i;
	int nb_channels;
	double rad;
	struct color colours[MAX_CHANNELS] = {{120,120,120},{0,150,150},{160,0,160},{170,170,0}};

	atexit(cleanup);

	fbfd = init_lcd();
	if (fbfd < 0)
		return EXIT_FAILURE;

	nb_channels = 3;
	while(1) {
		time += 1;
		if (time >= 479) {
			time = 0;
		}
		for (i=0; i<nb_channels; i++) {
			rad += 0.01745f;
			y[i] = (unsigned int)(sin(rad)*100 + 136 + 5*i);
			//printf("%d: %d %d\n", i, time, y[i]);
			draw_pixel(time, y[i], colours[i]);
		}
		clear_vline(time+1);
		usleep(50000);
	}
	
	return EXIT_SUCCESS;
}

