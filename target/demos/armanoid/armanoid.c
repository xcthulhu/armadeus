/*
 *
 * Small game to demonstrate how to use (or not use ;-) ) SDL library
 *   for the Armadeus project. www.armadeus.og
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
// For collision detection:
#include "collide.h"
// For timer handling (g++ only)
//#include "timer.h"
// To play sound:
#include "buzzer.h"


#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#ifdef DEBUG
#define debug( args...) printf( ## args )
#else
#define debug( args...) do {;} while(0);
#endif

#define bool  unsigned int
#define false 0
#define true  1

#define DEFAULT_DATA_DIR "/usr/share/games/armanoid/data/"

SDL_Surface *back;
SDL_Surface* block;
SDL_Surface *screen;

struct sprite {
    int xpos;  // Current pos on screen
    int ypos;

    Sint8 xspeed; // Current speed
    Sint8 yspeed;

    int width; // Size of sprite
    int height;

    int xhot; // Hostspot coordinates for collison detection
    int yhot;

    SDL_Surface* pSurface; // Bitmap
};

static struct sprite ball;
static struct sprite bar;

static bool ball_isfree = false;
int remainingBlocks = 0;

static int BLOCK_HORI_SIZE = 32;
static int BLOCK_VERT_SIZE = 24;

#define BLOCK_SPACING   4
#define YOFFUP          24
#define YOFFDOWN        50
#define XOFF            48
#define MAX_HORI_BLOCKS ((SCREEN_WIDTH-XOFF*2)/(BLOCK_HORI_SIZE+BLOCK_SPACING))
#define MAX_VERT_BLOCKS ((SCREEN_HEIGHT-YOFFUP*2-YOFFDOWN)/(BLOCK_VERT_SIZE+BLOCK_SPACING))

const int NB_FRAMES_PER_SECOND = 50;

char blocks[32][32];


// Load an image from a filename to a sprite surface
static int loadImage( struct sprite* aSprite, char* afilename )
{
    int result = 0;
    SDL_Surface* loadedImage = NULL; 

    loadedImage= SDL_LoadBMP( afilename );
    if( loadedImage == NULL )
    {
        printf("Error while loading image: %s\n", afilename);
        result = -1;
    }
    else
    {
        aSprite->pSurface = SDL_DisplayFormat( loadedImage ); 
        SDL_FreeSurface( loadedImage ); 
        aSprite->width = aSprite->pSurface->w;
        aSprite->height = aSprite->pSurface->h;
    }
    return( result );
}

// Load all images needed by the game (from .bmp files)
static int loadImages()
{
    int result = 0;

    loadImage( &ball, DEFAULT_DATA_DIR "/ball.bmp");
    loadImage( &bar, DEFAULT_DATA_DIR "/bar.bmp");

    block = SDL_LoadBMP( DEFAULT_DATA_DIR "/block.bmp" );
    BLOCK_HORI_SIZE = block->w;
    BLOCK_VERT_SIZE = block->h;
    // Check if we got all images
    if( ball.pSurface == NULL || bar.pSurface == NULL || block ==NULL )
    {
        printf("Unable to load image files !\n");
        result = -1;
    } else {
        ball.xpos   = ball.ypos = 0;
        ball.xspeed = ball.yspeed = 1;
        ball.width  = ball.pSurface->w;
        ball.height = ball.pSurface->h;
        ball.xhot = ball.width; ball.yhot = ball.height;
        
        //SDL_SetAlpha( image, 0/*SDL_SRCALPHA*/, SDL_ALPHA_TRANSPARENT );
        if( SDL_SetColorKey( ball.pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(ball.pSurface->format, 0xFF, 0xFF, 0xFF) ) )
        {
            printf("Error while setting transparent color\n");
        }

        bar.xpos=SCREEN_WIDTH/2; bar.ypos= SCREEN_HEIGHT-(bar.height*2);
    }

    return(result);
}

static void freeRessources()
{
    SDL_FreeSurface( screen );
    SDL_FreeSurface( ball.pSurface );
    SDL_FreeSurface( bar.pSurface );
    SDL_FreeSurface( block );
}

void DrawIMG(SDL_Surface *img, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(img, NULL, screen, &dest);
}

void DrawIMGRect(SDL_Surface *img, int x, int y,
                                int w, int h, int x2, int y2)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_Rect dest2;
    dest2.x = x2;
    dest2.y = y2;
    dest2.w = w;
    dest2.h = h;
    SDL_BlitSurface(img, &dest2, screen, &dest);
}

void drawBackground()
{
    SDL_Rect dest;
    dest.x = 0; dest.y = 0; dest.w = SCREEN_WIDTH; dest.h = SCREEN_HEIGHT;
    // Fill background
    SDL_FillRect( screen, &dest, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
}

void initBlocks()
{
    Uint16 i=0, j=0;

    for( i=0; i< MAX_HORI_BLOCKS; i++)
    {
        for( j=0; j< MAX_VERT_BLOCKS; j++)
        {
            blocks[i][j] = 1;
            remainingBlocks++;
        }
    }
}

void drawBlocks()
{
    Uint16 i=0, j=0;

    for( i=0; i< MAX_HORI_BLOCKS; i++)
    {
        for( j=0; j< MAX_VERT_BLOCKS; j++)
        {
            if( blocks[i][j] != 0 )
            {
                DrawIMG( block, (XOFF + i*(BLOCK_HORI_SIZE + BLOCK_SPACING)), (YOFFUP + j*(BLOCK_VERT_SIZE + BLOCK_SPACING)) );
            }
        }
    }
}

void drawScene()
{
    drawBackground();
    drawBlocks();
    DrawIMG(ball.pSurface, ball.xpos, ball.ypos);
    DrawIMG(bar.pSurface,  bar.xpos, bar.ypos);

    SDL_Flip(screen);
}

bool hasBallCollidedBlock(void)
{
    bool result = false;

//     SDL_CollideBoundingBox(&ball , ball.xpos , ball.ypos ,
//                              SDL_Surface *sb , int bx , int by)

    Uint32 Xind, Yind;
    if( (ball.xpos + ball.width > XOFF) && (ball.ypos + ball.height > YOFFUP) )
    {
        if( (ball.xpos + ball.width < SCREEN_WIDTH - XOFF) && (ball.ypos + ball.height < SCREEN_HEIGHT - YOFFDOWN) )
        {
            Xind = (ball.xpos-XOFF) / (BLOCK_HORI_SIZE + BLOCK_SPACING);
            Yind = (ball.ypos-YOFFUP) / (BLOCK_VERT_SIZE + BLOCK_SPACING);
            if( (Xind < MAX_HORI_BLOCKS) && (Yind < MAX_VERT_BLOCKS) && (blocks[Xind][Yind] == 1) )
            {
                result = true;
                debug("Xpos: %d, Xind: %d, YPos %d, Yind %d \n", ball.xpos, Xind, ball.ypos, Yind);
                blocks[Xind][Yind] = 0;
                if( (ball.xpos + ball.width / 2) < (XOFF + (Xind) * (BLOCK_HORI_SIZE + BLOCK_SPACING)) )
                {
                    ball.xspeed = -(ball.xspeed);
                    debug("SideG coll\n");
                }
                else if( (ball.xpos + ball.width / 2) > (XOFF + (Xind+1) * (BLOCK_HORI_SIZE + BLOCK_SPACING)) )
                {
                    ball.xspeed = -(ball.xspeed);
                    debug("SideD coll\n");
                } else
                    ball.yspeed = -(ball.yspeed);
                remainingBlocks--;
            }
        }
    }

    return( result );	
}

bool hasBallCollidedBar(void)
{
    bool result = false;

//     if( (ball.xpos + ball.xhot >= barxpos) && (ball.xpos + ball.xhot <= (barxpos+64)) )
//       if( (ball.ypos + ball.yhot >= barypos) && (ball.ypos + ball.yhot <= (barypos+16)) )
//          result = true;
    if( SDL_CollideBoundingBox(ball.pSurface , ball.xpos , ball.ypos , bar.pSurface, bar.xpos , bar.ypos) )
        result = true;

    return(result);	
}

bool hasBallCollidedBarOnSide(void)
{
    bool result = false;

    if( (ball.ypos > bar.ypos) && ( SDL_CollideBoundingBox(ball.pSurface , ball.xpos , ball.ypos , bar.pSurface, bar.xpos , bar.ypos) ) )
        result = true;

    return(result);	
}

#define MAX_BAR_SPEED 10
int main(int argc, char *argv[])
{
    Uint8* keys;
    int nbFrames = 0;
    //Timer fps, update;
    //Sint8 xspeed = 1, yspeed =1;

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER /*| SDL_INIT_EVENTTHREAD |SDL_INIT_NOPARACHUTE*/) < 0 )
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    if (SDL_InitSubSystem(SDL_INIT_TIMER) == -1)
    {
        fprintf(stderr, "SDL timer failed to initialize! Error: %s\n", SDL_GetError());
        exit(1);
    }
    else
    {
        fprintf(stdout, "SDL timer initialized properly!\n");
    }
    // Ask SDL to cleanup when exiting
    atexit(SDL_Quit);

    // Get a screen to display our game
    screen=SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0/*SDL_HWPALETTE*//*SDL_HWSURFACE|| SDL_DOUBLEBUF*/ );
    if( screen == NULL )
    {
        printf("Unable to set %dx%dvideo mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    // Load our graphics
    if( loadImages() )
    {
        return(1);
    }
    drawBackground();
    initBlocks();

    // Initialize sound
    if( initBuzzer() )
    {
        printf("Unable to find PWM on your system, sound won't be activated\n");
    }

    int done=0;
    debug("Calculated blocks number: Hori: %d, Vert: %d\n", MAX_HORI_BLOCKS, MAX_VERT_BLOCKS);

    // Hide mouse
    SDL_ShowCursor(0);

    /* // Start timer for Frame Rate calculation
    update.start();
    fps.start();*/

    // Main loop
    while(done == 0)
    {
        SDL_Event event;
        // Wait for SDL events
        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {  done = 1;  }
            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
            }
        }

        bar.xpos += 1*(bar.xspeed/2);
        if( bar.xspeed < 0 ) bar.xspeed++; else if( bar.xspeed > 0 ) bar.xspeed--;
        if(bar.xpos < 0) bar.xpos = 0;
        if( bar.xpos > (SCREEN_WIDTH-bar.width) ) bar.xpos = (SCREEN_WIDTH-bar.width);

        // Check user actions and move pad
        keys = SDL_GetKeyState(NULL);
        /*if ( keys[SDLK_UP] ) { ball.ypos -= 1; }
        if ( keys[SDLK_DOWN] ) { ball.ypos += 1; }*/
        if ( keys[SDLK_LEFT] )
        {
            bar.xspeed -= 2; if( bar.xspeed <= -MAX_BAR_SPEED ) bar.xspeed = -MAX_BAR_SPEED;
        }
        if ( keys[SDLK_RIGHT] )
        {
            bar.xspeed += 2; if( bar.xspeed >= MAX_BAR_SPEED ) bar.xspeed = MAX_BAR_SPEED; 
        }

        if ( keys[SDLK_SPACE] ) { ball_isfree = true; ball.yspeed = -1; }

        // Move ball depending on speed
        if( ball_isfree )
        {
            ball.ypos += ball.yspeed;
            ball.xpos += ball.xspeed;
        } else // or from the pad if captured
        {
            ball.ypos = bar.ypos - ball.height;
            ball.xpos = bar.xpos + bar.width/2 - ball.width/2;
            if( bar.xspeed > 0 )
                ball.xspeed = 1;
            else if( bar.xspeed < 0 )
                ball.xspeed = -1; 
        }

        // Check if ball will go out of playing area
        if( ball.xpos + ball.width >= SCREEN_WIDTH ) { ball.xspeed = -(ball.xspeed); debug("Xx: %i\n", ball.xspeed); ball.xhot = 0;}
        if( ball.ypos + ball.height >= SCREEN_HEIGHT ) { printf("PERDU !\n"); done=1;}
        if( ball.xpos <= 0 ) { ball.xspeed = -(ball.xspeed); ball.xhot = ball.width; debug("Xx: %i\n", ball.xspeed); }
        if( ball.ypos <= 0 ) { ball.yspeed = -(ball.yspeed); ball.yhot = ball.height; debug("Yy: %i\n", ball.yspeed);}

        // Check if ball has collided with an other object
        if( hasBallCollidedBar()  && ball_isfree ) { ball.yspeed = -(ball.yspeed); ball.yhot = abs(ball.yhot-ball.width); playSound(1000,100);};
        if( hasBallCollidedBarOnSide() ) { ball.xspeed = -(ball.xspeed); playSound(1000,100);};
        if( hasBallCollidedBlock() ) { playSound(500,100); };

        if( remainingBlocks <= 0 ) { printf("GAGNE !!\n"); done=1;}

        // Update screen
        drawScene();
        nbFrames++;

        // Calculate and stabilize Frame Rate
/*        if( update.getTicks() > 1000 )
        {
            printf("FPS: %f\n", (float)nbFrames  );
            update.start();
            nbFrames = 0;
        }
        while( fps.getTicks() < (1000 / NB_FRAMES_PER_SECOND) )
        {
            ;
        }
        fps.start();*/
        SDL_Delay(20);
    }

    // Cleanup what we used
    releaseBuzzer();
    freeRessources();
    SDL_QuitSubSystem( SDL_INIT_TIMER );
    SDL_Quit();

    return 0;
}
