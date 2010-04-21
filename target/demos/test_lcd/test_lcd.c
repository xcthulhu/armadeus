/*
 * Small tool to test LCD pixels (and why not one day repair defect one ! ;-))
 * for the Armadeus project: http://www.armadeus.org
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for sleep() */

#include "SDL.h"

#ifdef DEBUG
#define debug( args...) printf( ## args )
#else
#define debug( args...) do {;} while(0);
#endif


SDL_Surface *screen;


static void free_ressources()
{
    SDL_FreeSurface(screen);
}

void cleanup(void)
{
    free_ressources();
    SDL_Quit();
}

static void fill_screen_and_wait_key(Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 color;

    color = SDL_MapRGB(screen->format, r, g, b);
    SDL_FillRect(screen, NULL, color);
    SDL_Flip(screen);
    printf("Press ENTER to continue\n");
    getchar();
}


int main(int argc, char *argv[])
{
    Uint8* keys;
    int cursorPos = 0;
    const struct SDL_VideoInfo *video_info;
    SDL_Rect **modes;
    int i, height, width;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER /*| SDL_INIT_EVENTTHREAD |SDL_INIT_NOPARACHUTE*/) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(cleanup);

    /* Get available fullscreen/hardware modes */
    modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

    /* Check is there are any modes available */
    if (modes == (SDL_Rect **)0) {
        printf("No modes available!\n");
        exit(-1);
    }

    /* Check if our resolution is restricted */
    if (modes == (SDL_Rect **)-1) {
        printf("All resolutions available.\n");
    } else {
        /* Print valid modes */
        printf("Available Modes\n");
        for (i=0; modes[i]; ++i) {
            printf("  %d x %d\n", modes[i]->w, modes[i]->h);
        }
    }
    width = modes[0]->w;
    height = modes[0]->h;
    printf("Taking greatest one: %d x %d\n\n", width, height);

    /* Get video infos */
    video_info = SDL_GetVideoInfo();
    if (!video_info) {
        printf("Unable to get video infos\n");
        exit(1);
    }
/*    printf("Pixel format:\n");
      printf(" ", video_info->vfmt-> */

    /* Get a screen to display our test */
    screen = SDL_SetVideoMode(width, height, 16, 0);
    if (screen == NULL) {
        fprintf(stderr, "Unable to set %d x %d video mode: %s\n", width, height, SDL_GetError());
        exit(1);
    }

    /* Hide mouse */
    SDL_ShowCursor(0);

    /* Main loop */
    fill_screen_and_wait_key(0, 0, 0); /* black */
    fill_screen_and_wait_key(255, 0, 0); /* red */
    fill_screen_and_wait_key(0, 255, 0); /* green */
    fill_screen_and_wait_key(0, 0, 255); /* blue */
    fill_screen_and_wait_key(255, 255, 255); /* white */

    exit(0);
}

