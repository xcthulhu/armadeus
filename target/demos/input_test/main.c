/*
 * Small tool to test user input devices (with the use of SDL library)
 *   for the Armadeus project: http://www.armadeus.org
 *
 * It's mostly a copy of this SDL tutorial:
 *     http://www.gnurou.org/writing/linuxmag/sdl
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
#include "SDL.h"

#ifdef DEBUG
#define debug( args...) printf( ## args )
#else
#define debug( args...) do {;} while(0);
#endif


unsigned char letsexit = 0;
/* Sprite's velocity */
Uint16 xvel = 0, yvel = 0;
/* Sprite's position */
SDL_Rect spritepos;

/* Input events handling */
void process_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
                printf("\rKey %3d down (%s)        ",
                        event.key.keysym.sym,
                        SDL_GetKeyName(event.key.keysym.sym));
                fflush(stdout);
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                        letsexit = 1;
                        break;
                case SDLK_LEFT:
                        xvel = -1;
                        break;
                case SDLK_RIGHT:
                        xvel = 1;
                        break;
                case SDLK_UP:
                        yvel = -1;
                        break;
                case SDLK_DOWN:
                        yvel = 1;
                        break;
                default:
                        break;
                }
                break;

        case SDL_KEYUP:
                printf("\rKey %3d up  ", event.key.keysym.sym);
                fflush(stdout);
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                        xvel =0;
                        break;
                case SDLK_UP:
                case SDLK_DOWN:
                        yvel = 0;
                        break;
                default:
                        break;
                }
                break;

        case SDL_MOUSEMOTION:
                spritepos.x += event.motion.xrel;
                spritepos.y += event.motion.yrel;
                printf("\rMouse coords: (%3d, %3d)", event.motion.x,
                        event.motion.y);
                fflush(stdout);
                break;

        case SDL_MOUSEBUTTONDOWN:
                printf("\rMouse button %d down    ", event.button.button);
                fflush(stdout);
                break;

        case SDL_MOUSEBUTTONUP:
                printf("\rMouse button %d up  ", event.button.button);
                fflush(stdout);
        break;

        case SDL_JOYAXISMOTION:
            printf("\rJoy %d Axis %d = %d     ",
                    event.jaxis.which, event.jaxis.axis, event.jaxis.value);
            fflush(stdout);
            /* Even axes = verticals, odd = horizontals.
               jaxis.value are big so divide it by 2^14 */
            if (event.jaxis.axis % 2)
                    yvel = event.jaxis.value >> 14;
            else
                    xvel = event.jaxis.value >> 14;
            break;

        case SDL_JOYHATMOTION:
            printf("Joy %d Hat %d pos: ",
                    event.jhat.which, event.jhat.hat);
            switch (event.jhat.value)
            {
            case SDL_HAT_CENTERED:
                xvel = 0; yvel = 0;
                printf("centered\n");
                break;
            case SDL_HAT_UP:
                xvel = 0; yvel = -1;
                printf("up\n");
                break;
            case SDL_HAT_RIGHT:
                xvel = 1; yvel = 0;
                printf("right\n");
                break;
            case SDL_HAT_DOWN:
                xvel = 0; yvel = 1;
                printf("down\n");
                break;
            case SDL_HAT_LEFT:
                xvel = -1; yvel = 0;
                printf("left\n");
                break;
            case SDL_HAT_RIGHTUP:
                xvel = 1; yvel = -1;
                printf("right/up\n");
                break;
            case SDL_HAT_RIGHTDOWN:
                xvel = 1; yvel = 1;
                printf("right/down\n");
                break;
            case SDL_HAT_LEFTUP:
                xvel = -1; yvel = -1;
                printf("left/up\n");
                break;
            case SDL_HAT_LEFTDOWN:
                xvel = -1; yvel = 1;
                printf("left/down\n");
                break;
            }
            break;

        case SDL_JOYBALLMOTION:
            spritepos.x += event.jball.xrel;
            spritepos.y += event.jball.yrel;
            printf("Joy %d Trackball %d moved: (%d, %d)\n",
                    event.jball.which, event.jball.ball,
                    event.jball.xrel, event.jball.yrel);
            break;

        case SDL_JOYBUTTONDOWN:
            printf("\rJoy %d button %d down     ",
                    event.jbutton.which, event.jbutton.button);
            fflush(stdout);
            break;

        case SDL_JOYBUTTONUP:
            printf("\rJoy %d button %d up  ",
                    event.jbutton.which, event.jbutton.button);
            fflush(stdout);
            break;

        default:
            break;
        }
    }
}

#ifdef APF
#define SDL_VIDEO_FLAGS (SDL_ANYFORMAT)
#else /* SDL_DOUBLEBUF crashes on APF */
#define SDL_VIDEO_FLAGS (SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT)
#endif
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

int main(int argc, char * argv[])
{
    SDL_Surface * screen;
    SDL_Surface * sprite, * tmp;
    int nbjoysticks;
    int i;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        fprintf(stderr, "Error during SDL init: %s\n",
                SDL_GetError());
        return 1;
    }

    /* Call SDL_Quit() before exiting */
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_VIDEO_FLAGS);
    if (screen) {
        printf("Video mode: %dx%dx%d\n", screen->w, screen->h, 
                screen->format->BitsPerPixel);
    } else {
        printf("Unable to set %dx%d video mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        return 1;
    }

    /* Activate UNICODE support */
    SDL_EnableUNICODE(1);

    /* Find available Joysticks */
    nbjoysticks = SDL_NumJoysticks();
    printf("Number of Joysticks found: %d\n", nbjoysticks);
    for (i = 0; i < nbjoysticks; i++)
    {
        SDL_Joystick * joy = SDL_JoystickOpen(i);
        printf("--- Joystick %d: %s\n", i, SDL_JoystickName(i));
        printf("    Axes: %d\n", SDL_JoystickNumAxes(joy));
        printf("    Buttons: %d\n", SDL_JoystickNumButtons(joy));
        printf("    Trackballs: %d\n", SDL_JoystickNumBalls(joy));
        printf("    Hats: %d\n", SDL_JoystickNumHats(joy));
    }
    printf("\n");

    /* Sprite's image loading */
    tmp = SDL_LoadBMP("sdl.bmp");
    if (!tmp) {
        /* 2nd chance: */
        tmp = SDL_LoadBMP("/usr/share/images/sdl.bmp");
        if (!tmp) {
            printf("Unable to load image for test\n");
            exit(1);
        }
    }
    sprite = SDL_DisplayFormat(tmp);
    SDL_FreeSurface(tmp);
    SDL_SetColorKey(sprite, SDL_SRCCOLORKEY, SDL_MapRGB(sprite->format, 0xff, 0x00, 0xff));

    /* Sprite's initial coords */
    spritepos.x = (screen->w - sprite->w) / 2;
    spritepos.y = (screen->h - sprite->h) / 2;
    spritepos.w = 0;
    spritepos.h = 0;

    /* Main loop */
    while (!letsexit)
    {
        process_events();

        /* Update sprite pos */
        spritepos.x += xvel;
        spritepos.y += yvel;
        /* Sprite clipping */
        if (spritepos.x < 0) spritepos.x = 0;
        if (spritepos.x > screen->w - sprite->w) spritepos.x = screen->w - sprite->w;
        if (spritepos.y < 0) spritepos.y = 0;
        if (spritepos.y > screen->h - sprite->h) spritepos.y = screen->h - sprite->h;

        /* Clear screen */
        SDL_FillRect(screen, NULL, 0);
        /* Blit sprite */
        SDL_BlitSurface(sprite, NULL, screen, &spritepos);
        /* Show screen */
        SDL_Flip(screen);
    }

    printf("\nBye !\n");
    return 0;
}

