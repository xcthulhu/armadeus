#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

#include "SDL.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

SDL_Surface *back;
//SDL_Surface *image;
SDL_Surface* bar;
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
//int xpos=0,ypos=0;
int barxpos=SCREEN_WIDTH/2, barypos= SCREEN_HEIGHT-32;
int remainingBlocks = 0;

#define BLOCK_HORI_SIZE 32
#define BLOCK_VERT_SIZE 24

#define BLOCK_SPACING   4
#define YOFFUP          24
#define YOFFDOWN        50
#define XOFF            48
#define MAX_HORI_BLOCKS ((SCREEN_WIDTH-XOFF*2)/(BLOCK_HORI_SIZE+BLOCK_SPACING))
#define MAX_VERT_BLOCKS ((SCREEN_HEIGHT-YOFFUP*2-YOFFDOWN)/(BLOCK_VERT_SIZE+BLOCK_SPACING))


char blocks[MAX_HORI_BLOCKS][MAX_VERT_BLOCKS];


int loadImages()
{
    int result = 0;
    
    //back  = SDL_LoadBMP("./background.bmp");
    ball.pSurface = SDL_LoadBMP("./ball.bmp");
    bar   = SDL_LoadBMP("./bar.bmp");
    block = SDL_LoadBMP("./block.bmp");
    if( ball.pSurface == NULL || bar == NULL || block ==NULL )
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
    }

    return(result);
}

void DrawIMG(SDL_Surface *img, int x, int y)
{
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  SDL_BlitSurface(img, NULL, screen, &dest);
}

void DrawIMG(SDL_Surface *img, int x, int y,
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

void DrawBG()
{
    //DrawIMG(back, 0, 0);

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

void DrawScene()
{
    //DrawIMG(back, xpos-2, ypos-2, 132, 132, xpos-2, ypos-2);
    DrawBG();
    drawBlocks();
    DrawIMG(ball.pSurface, ball.xpos, ball.ypos);
    DrawIMG(bar,   barxpos, barypos);

    SDL_Flip(screen);
}

bool hasBallCollidedBlock(void)
{
    bool result = false;

    // Test pad collision
    Uint32 distance=0;
/*    distance = (ypos - barypos) * (ypos - barypos) + (xpos - barxpos) * (xpos - barxpos);
    
    if( distance <= 32 )
        result = true;
*/
    Uint32 Xind, Yind;
    if( (ball.xpos + ball.xhot > XOFF) && (ball.ypos + ball.yhot > YOFFUP) )
    {
        if( (ball.xpos + ball.xhot < SCREEN_WIDTH - XOFF) && (ball.ypos + ball.yhot < SCREEN_HEIGHT - YOFFDOWN) )
        {
            Xind = (ball.xpos-XOFF) / (BLOCK_HORI_SIZE + BLOCK_SPACING);
            Yind = (ball.ypos-YOFFUP) / (BLOCK_VERT_SIZE + BLOCK_SPACING);
            if( (Xind < MAX_HORI_BLOCKS) && (Yind < MAX_VERT_BLOCKS) && (blocks[Xind][Yind] == 1) )
            {
                result = true;
                printf("Xpos: %d, Xind: %d, YPos %d, Yind %d \n", ball.xpos, Xind, ball.ypos, Yind);
                blocks[Xind][Yind] = 0;
                remainingBlocks--;
            }
        }
    }
//    printf("Dist: %i\r", distance);
    return(result);	
}

bool hasBallCollidedBar(void)
{
    bool result = false;

    if( (ball.xpos + ball.xhot >= barxpos) && (ball.xpos + ball.xhot <= (barxpos+64)) )
      if( (ball.ypos + ball.yhot >= barypos) && (ball.ypos + ball.yhot <= (barypos+16)) )
         result = true;

    return(result);	
}

int main(int argc, char *argv[])
{
    Uint8* keys;

    //Sint8 xspeed = 1, yspeed =1;

    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 )
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    screen=SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 16, 0/*SDL_HWSURFACE|SDL_DOUBLEBUF*/ );
    if ( screen == NULL )
    {
        printf("Unable to set %dx%dvideo mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }

    if( loadImages() )
    {
        return(1);
    }
    DrawBG();
    initBlocks();

    int done=0;
    printf("Calculated blocks number: Hori: %d, Vert: %d\n", MAX_HORI_BLOCKS, MAX_VERT_BLOCKS);

    sleep(1);
    SDL_ShowCursor(0);
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
        // Check user actions
        keys = SDL_GetKeyState(NULL);
        /*if ( keys[SDLK_UP] ) { ball.ypos -= 1; }
        if ( keys[SDLK_DOWN] ) { ball.ypos += 1; }*/
        if ( keys[SDLK_LEFT] )  { barxpos -= 2; if(barxpos < 0) barxpos = 0;}
        if ( keys[SDLK_RIGHT] ) { barxpos += 2; if( barxpos > (SCREEN_WIDTH-64) ) barxpos = (SCREEN_WIDTH-64);}

        // Move ball depending on speed
        ball.ypos += ball.yspeed;
        ball.xpos += ball.xspeed;

        // Check if ball will go out of playing area
        if( ball.xpos+ball.xhot >= SCREEN_WIDTH ) { ball.xspeed = -(ball.xspeed); printf("Xx: %i\n", ball.xspeed); ball.xhot = 0;}
        if( ball.ypos+ball.yhot >= SCREEN_HEIGHT ) { /*ball.yspeed = -(ball.yspeed); ball.yhot = 0;*/ printf("PERDU !\n"); done=1;}
        if ( ball.xpos+ball.xhot <= 0 ) { ball.xspeed = -(ball.xspeed); ball.xhot = ball.width; printf("Xx: %i\n", ball.xspeed); }
        if ( ball.ypos+ball.yhot <= 0 ) { ball.yspeed = -(ball.yspeed); ball.yhot = ball.height; printf("Yy: %i\n", ball.yspeed);}

        // Check if ball has collided with an other object
        if( hasBallCollidedBar() ) { ball.yspeed = -(ball.yspeed); ball.yhot = abs(ball.yhot-ball.width);};
        if( hasBallCollidedBlock() ) { ball.yspeed = -(ball.yspeed); /*ball.xspeed = -(ball.xspeed);*/ ball.yhot = abs(ball.yhot-ball.width);};

        if( remainingBlocks <= 0 ) { printf("GAGNE !!\n"); done=1;}

        // Update screen
        DrawScene();
    }

    return 0;
}
