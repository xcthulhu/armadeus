/*
*  Calculates (distributed) and displays a Newton's fractal.
*
* Copyright (C) 2009 <gwenhael.goavec-merou@armadeus.com>
*                         Armadeus Project / Armadeus Systems
*
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

#include "newton.h"
#include "lcd.h"

#define LECTURE 0
#define ECRITURE 1


#define NB_CHILD 10

typedef struct str_color {
  int r;
  int g;
  int b;
} colour;

typedef struct elem_pict {
  float ii;
  int x;
  int y;
  int height;
  int width;
  int numFils;
  colour ligne[24][320];
} elemPict;


/* used by children process */
void workChildren(elemPict *ep)
{
  float ii,ri,r,i;
  int y=0;
  int totaliter=0;
  int x=0, maxiter=0, iter=0;
  /*float fin = (RMAX-RMIN)/POINTSX;*/
  float finY = (IMAX-IMIN)/POINTSY;
  float yDeb = IMIN+(finY*ep->y);
 
  for (ii=yDeb; ii<IMAX; ii+=(IMAX-IMIN)/POINTSY) {   /* terminal : 105x54 */
    for (ri=RMIN; ri<RMAX; ri+=(RMAX-RMIN)/POINTSX) {
      iter=0;
      r=ri; i=ii;
      do {
	newton(&r,&i);
	iter++; 
      } while ((module(r,i)<0.999) || (module(r,i)>1.001));
      if (iter>maxiter) 
	maxiter = iter;
      totaliter += iter;
      if (y >= (ep->height)) return;
      ep->ligne[y][x].r = ep->ligne[y][x].g = ep->ligne[y][x].b = 0;
      if (r>0){
	ep->ligne[y][x].r = 255;
      } else{
	if (i>0){
	  ep->ligne[y][x].g = 255;
	} else{ 
	  ep->ligne[y][x].b = 255;
	}
      }    
      x++;
    }
    y++;
    x=0;
  }
}

/* Used by the master */
void workMaster(int *tabChildPipe, int global)
{
  int nbChild = NB_CHILD;
  int i,lignDeb;
  elemPict ep;

  /* Sending the command processing */
  for (i=0,lignDeb=0; i<NB_CHILD; i++,lignDeb+=24) {
    ep.x=0;
    ep.y=lignDeb;
    ep.height=24;
    ep.numFils=i;
    write(tabChildPipe[i], &ep, sizeof(elemPict));
  }

  /* Queuing of all the calculations */
  do { 	
    int size = read(global, &ep, sizeof(elemPict)+1);
    printf("received from %d start %d end %d\n", ep.numFils, ep.y, ep.height+ep.numFils);
    if (size != sizeof(elemPict)) {
      printf("strange\n");
      continue;
    }
    nbChild--;
    int yy, x, y;
    for (yy=0,y=ep.y; yy<24; y++,yy++) {
      for (x=0; x<POINTSX; x++) {
	print_pix(x, y, ep.ligne[yy][x].r, ep.ligne[yy][x].g, ep.ligne[yy][x].b);
      }    
    }
  } while (nbChild >0);
}

void prepareChild(int *reception, int *versMaster)
{
  elemPict ep;

  close(versMaster[LECTURE]);
  close(reception[ECRITURE]);
  read(reception[LECTURE], &ep, sizeof(elemPict));
  workChildren(&ep);
  write(versMaster[1], &ep, sizeof(elemPict));
  close(versMaster[ECRITURE]);
  close(reception[LECTURE]);
}

int main(void /*int argc, char **argv*/)
{
  int global[2];
  int tabChildPipe[NB_CHILD];
  int pidChildPipe[NB_CHILD];
  
  if (pipe(global) == -1) {
    perror("pipeToMaster");
    return EXIT_FAILURE;
  }
  /* Children process creation */
  int pos;
  int receptionFils[2];
  for (pos=0; pos < NB_CHILD; pos++) {
    if (pipe(receptionFils) == -1){
      perror("pipeToMaster");
      return EXIT_FAILURE;
    }
    int pid = fork();
    switch (pid) {
    case -1:
      printf("problem...\n");
      return -1;
      break;
    case 0: 
      prepareChild(receptionFils, global);
      return EXIT_SUCCESS;
      break;
    default:
      close(receptionFils[LECTURE]); 
      tabChildPipe[pos] = receptionFils[ECRITURE];
      pidChildPipe[pos] = pid;
      break;
    }
  }
  close(global[ECRITURE]);
  int fbfd = init_lcd();
  if (fbfd < 0)
    return EXIT_FAILURE;

  workMaster(tabChildPipe, global[LECTURE]);
  close(global[LECTURE]); 
  
  for (pos = 0; pos < 1; pos++) {
    close(tabChildPipe[pos]);
  }
  for (pos = 0; pos < 1; pos++) {
    waitpid(pidChildPipe[pos], NULL, 0);
  }

  close_lcd(fbfd);

  return EXIT_SUCCESS;
}

