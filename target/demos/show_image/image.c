/*
 * Small tool to demonstrate how to use SDL_image library
 *   for the Armadeus project: http://www.armadeus.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272


void wait()
{
	int end = 0;
	SDL_Event event;

	while (!end)
	{
		SDL_WaitEvent(&event);
		switch(event.type)
		{
			case SDL_QUIT:
				end = 1;
				break;
		}
	}
}

int main(int argc, char *argv[])
{
	SDL_Surface *screen = NULL, *image = NULL;
	SDL_Rect coord;

	if (argc != 2) {
		printf("Usage: %s file\n", argv[0]);
		return EXIT_FAILURE;
	}
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_ANYFORMAT);
	if (!screen) {
		printf("Unable to open display (%dx%d)\n", SCREEN_WIDTH, SCREEN_HEIGHT);
		printf(" Have you tried with:  export SDL_NOMOUSE=1  ??\n");
		return EXIT_FAILURE;
	}

	image = IMG_Load(argv[1]);
	if (!image) {
		printf("Unable to load image file: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	SDL_BlitSurface(image, NULL, screen, 0);
	SDL_Flip(screen);

	wait();

	SDL_FreeSurface(image);

	return EXIT_SUCCESS;
}
