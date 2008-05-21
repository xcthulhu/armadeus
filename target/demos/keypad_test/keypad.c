/*
 * Small tool to test keypad (with the use of SDL library)
 *   for the Armadeus project. www.armadeus.org
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
#include <unistd.h> // for sleep()

#include "SDL.h"

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 270

#ifdef DEBUG
#define debug( args...) printf( ## args )
#else
#define debug( args...) do {;} while(0);
#endif

#define bool  unsigned int
#define false 0
#define true  1

SDL_Surface *screen;


//---- Main ----

int main(int argc, char *argv[])
{
    Uint8* keys;
    int nbFrames = 0;
    int cursorPos = 0;

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER /*| SDL_INIT_EVENTTHREAD |SDL_INIT_NOPARACHUTE*/) < 0 )
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    // Ask SDL to cleanup when exiting
    atexit(SDL_Quit);

    // Get a screen to display some stuff
    screen=SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0/*SDL_HWPALETTE*//*SDL_HWSURFACE|| SDL_DOUBLEBUF*/ );
    if( screen == NULL )
    {
        printf("Unable to set %dx%dvideo mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    int done=0, move=0;

    // Hide mouse
    SDL_ShowCursor(0);

    // Main loop
    while(done == 0)
    {
        SDL_Event event;
        // Wait for SDL events
        while ( SDL_PollEvent(&event) )
        {
            cursorPos = 0;
            switch( event.type )
            {
                case SDL_QUIT:  {  done = 1; break; }

                case SDL_KEYDOWN:
                {
			//if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
			printf("Key down: %s\n", SDL_GetKeyName(event.key.keysym.sym));
			break;
                }

                case SDL_KEYUP:
                {
                    //if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
                    printf("Key up: %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    break;
                }

                default:
		break;
            }
        }

        SDL_Delay(50);
    }

    return 0;
}
