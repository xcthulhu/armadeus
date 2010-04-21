/*
 * Small tool to demonstrate how to use (or not use ;-) ) Tslib + SDL library
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

#include "SDL.h"
#include "backlight_sysctl.h"


#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 270
#define BAR_MIN_XPOSITION (0+(SCREEN_WIDTH>>4))
#define BAR_MAX_XPOSITION (SCREEN_WIDTH-(SCREEN_WIDTH>>4))
#define BAR_YPOSITION (SCREEN_HEIGHT/2)
#define BAR_WIDTH (BAR_MAX_XPOSITION-BAR_MIN_XPOSITION)

#ifdef DEBUG
#define debug( args...) printf( ## args )
#else
#define debug( args...) do {;} while(0);
#endif

#define bool  unsigned int
#define false 0
#define true  1

#define DEFAULT_DATA_DIR "/usr/share/apps/backlight/data/"

SDL_Surface *cursor;
SDL_Surface *full_end;
SDL_Surface *full;
SDL_Surface *empty;
SDL_Surface *empty_end;
SDL_Surface *screen;

const int NB_FRAMES_PER_SECOND = 50;


//---- Private functions ----

// Load all images needed by the tool (from .bmp files)
static int loadImages()
{
    int result = 0;

    full_end  = SDL_LoadBMP(DEFAULT_DATA_DIR "full_end.bmp");
    full      = SDL_LoadBMP(DEFAULT_DATA_DIR "full.bmp");
    cursor    = SDL_LoadBMP(DEFAULT_DATA_DIR "cursor.bmp");
    empty     = SDL_LoadBMP(DEFAULT_DATA_DIR "empty.bmp");
    empty_end = SDL_LoadBMP(DEFAULT_DATA_DIR "empty_end.bmp");

    /* Check if we got all images */
    if (full_end == NULL || full == NULL || cursor == NULL || empty == NULL || empty_end == NULL) {
        printf("Unable to load image files !\n");
        result = -1;
    } else {
        //SDL_SetAlpha( image, 0/*SDL_SRCALPHA*/, SDL_ALPHA_TRANSPARENT );
        if (SDL_SetColorKey(cursor, SDL_SRCCOLORKEY, SDL_MapRGB(cursor->format, 0xFF, 0xFF, 0xFF))) {
            printf("Error while setting transparent color\n");
        }
    }

    return result;
}

//
static void freeRessources()
{
    SDL_FreeSurface(screen);
    SDL_FreeSurface(full_end);
    SDL_FreeSurface(full);
    SDL_FreeSurface(cursor);
    SDL_FreeSurface(empty);
    SDL_FreeSurface(empty_end);
}

//
void drawImage(SDL_Surface *img, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(img, NULL, screen, &dest);
}

//
void drawBackground()
{
    SDL_Rect dest;
    dest.x = 0; dest.y = 0; dest.w = SCREEN_WIDTH; dest.h = SCREEN_HEIGHT;
    /* Fill background */
    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
}

//
static int paddXPos(int xpos)
{
    int x = 0;

    if (xpos <= BAR_MIN_XPOSITION) {
        x = BAR_MIN_XPOSITION;
    } else if (xpos >= BAR_MAX_XPOSITION) {
        x = BAR_MAX_XPOSITION;
    } else {
        x = xpos;
    }

    return x;
}

//
static void drawBar(int xpos)
{
    int i = BAR_MIN_XPOSITION;

    if (xpos <= BAR_MIN_XPOSITION) {
        xpos = BAR_MIN_XPOSITION;
    } else if (xpos >= BAR_MAX_XPOSITION) {
        xpos = BAR_MAX_XPOSITION;
    }

    drawImage(full_end, i, BAR_YPOSITION);
    i+=5;

    for ( ; i<=xpos; i+=5) {
        drawImage(full, i, BAR_YPOSITION);
    }

    for ( ; i<=BAR_MAX_XPOSITION; i+=5) {
        drawImage(empty, i, BAR_YPOSITION);
    }

    drawImage(empty_end, i, BAR_YPOSITION);
}

//
static unsigned char calculateBacklight(int xpos)
{
    int x = 0;
    unsigned char backlight = 0; /* 0-100 % */

    x = paddXPos(xpos);
    backlight = (unsigned char) (((x - BAR_MIN_XPOSITION)*100) / BAR_WIDTH);

    return backlight;
}

//
static void drawCursor(int xpos)
{
    int x = 0;

    x = paddXPos(xpos);
    drawImage(cursor, x, BAR_YPOSITION-5);
}

//
void updateCursor(int xpos)
{
    drawBackground();
    drawBar(xpos);
    drawCursor(xpos);
    SDL_Flip(screen);
}

void cleanup(void)
{
    /* Cleanup what we used */
    releaseBacklightControl();
    freeRessources();
    SDL_Quit();
}

//---- Main ----

#define MAX_BAR_SPEED 10
int main(int argc, char *argv[])
{
    Uint8* keys;
    int cursorPos = 0;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER /*| SDL_INIT_EVENTTHREAD |SDL_INIT_NOPARACHUTE*/) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    /* Ask SDL to cleanup when exiting */
    atexit(cleanup);

    /* Get a screen to display our GUI */
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0/*SDL_HWPALETTE*//*SDL_HWSURFACE|| SDL_DOUBLEBUF*/);
    if (screen == NULL) {
        fprintf(stderr, "Unable to set %dx%dvideo mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    /* Load our graphics */
    if (loadImages()) {
        return 1;
    }
    drawBackground();

    /* Initialize Backlight control */
    if (initBacklightControl()) {
        printf("Unable to find Backlight control on your system -> won't be activated\n");
    }
    updateCursor((getBrightness() * BAR_WIDTH / 100) + BAR_MIN_XPOSITION);
    int done=0, move=0;

    /* Hide mouse */
    SDL_ShowCursor(0);

    /* Main loop */
    while (done == 0)
    {
        SDL_Event event;
        /* Wait for SDL events */
        while (SDL_PollEvent(&event))
        {
            cursorPos = 0;
            switch (event.type)
            {
                case SDL_QUIT:  {  done = 1; break; }
                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        done = 1;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: 
                {
                    move = 1;
                    cursorPos = event.motion.x;
                    break; 
                }

                case SDL_MOUSEBUTTONUP: { move = 0; break; }

                case SDL_MOUSEMOTION:
                {
                    if (move == 1) {
                        debug("Current mouse position is: (%d, %d)\n", event.motion.x, event.motion.y);
                        cursorPos = event.motion.x;
                    }
                    break;
                }
            }
        }

        if (cursorPos != 0 && move == 1) {
            setBrightness(calculateBacklight(cursorPos));
            /* Update screen */
            updateCursor(cursorPos);
        }

        SDL_Delay(50);
    }

    return 0;
}
