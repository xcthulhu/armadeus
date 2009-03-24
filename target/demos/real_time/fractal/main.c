/*
*  Calculation and displays a fractal newton distributed.
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


/* methode utilisee pour le fils pour
 * faire son calcul
 */
void workChildren(elemPict *ep) {
  float ii,ri,r,i;
  int yy = 0,y=0;
  int totaliter=0;
  int x=0,maxiter=0, iter=0;//, yy = ep->y;
  float fin = (RMAX-RMIN)/POINTSX;
  float finY = (IMAX-IMIN)/POINTSY;
  float yDeb = IMIN+(finY*ep->y);
 

 for (ii=yDeb;ii<IMAX;ii+=(IMAX-IMIN)/POINTSY){   // terminal : 105x54
    for (ri=RMIN;ri<RMAX;ri+=(RMAX-RMIN)/POINTSX) {
      iter=0;
      r=ri; i=ii;
      do {
	newton(&r,&i);
	iter++; 
      }while ((module(r,i)<0.999) || (module(r,i)>1.001));
      if (iter>maxiter) 
	maxiter=iter;
      totaliter+=iter;
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
    y++;x=0;
  }
}


/* Travail fait par le maitre */
void workMaster(int *tabChildPipe, int global) {
  int nbChild = NB_CHILD;
  int i,lignDeb;
  // Envoi de la commande de traitement
  elemPict ep;
  for (i=0,lignDeb=0;i<NB_CHILD;i++,lignDeb+=24) {
    ep.x=0;
    ep.y=lignDeb;
    ep.height=24;
    ep.numFils=i;
    write(tabChildPipe[i],&ep,sizeof(elemPict));
  }

  // Mise en attente de l'ensemble des calculs
  do {
    int size = read(global,&ep,sizeof(elemPict)+1);
    printf("recu de %d debut %d fin %d\n",ep.numFils,ep.y,ep.height+ep.numFils);
    if (size != sizeof(elemPict)) {
      printf("bizarre\n");
      continue;
    }
    nbChild --;
    int yy,x,y;
    for (yy=0,y=ep.y;yy<24;y++,yy++) {
      for (x=0;x<POINTSX;x++){
	print_pix(x,y,ep.ligne[yy][x].r,ep.ligne[yy][x].g,ep.ligne[yy][x].b);
      }    
    }
  } while (nbChild >0);
}

void prepareChild(int *reception, int *versMaster) {
  close(versMaster[LECTURE]);
  close(reception[ECRITURE]);
  elemPict ep;
  read(reception[LECTURE], &ep, sizeof(elemPict));
  workChildren(&ep);
  write(versMaster[1],&ep,sizeof(elemPict));
  close(versMaster[ECRITURE]);
  close(reception[LECTURE]);
}

/* pipe : 0 => lecture dans le pipe , 1 => ecriture dans le pipe */

int main(int argc, char **argv) {
  int global[2];
  int tabChildPipe[NB_CHILD];
  int pidChildPipe[NB_CHILD];
  /* Preparation de la pipe commune 
   * enfant vers pere
   */
  if (pipe(global) == -1) {
    perror("pipeToMaster");
    return(EXIT_FAILURE);
  }
  /* Création de tous les processus fils */
  int pos;
  int receptionFils[2];
  for (pos = 0; pos < NB_CHILD; pos ++ ) {
    if (pipe(receptionFils) == -1){
      perror("pipeToMaster");
      return(EXIT_FAILURE);
    }
    int pid = fork();
    switch (pid) {
    case -1:
      printf("ca merde\n");
      return -1;
      break;
    case 0: // Fiston
      prepareChild(receptionFils, global);
      return EXIT_SUCCESS;
      break;
    default: // C'est le pere
      // Fermeture de la lecture
      close(receptionFils[LECTURE]); 
      tabChildPipe[pos] = receptionFils[ECRITURE];
      pidChildPipe[pos] = pid;
      break;
    }
  }
  // C'est plus que le code du pere 
  close(global[ECRITURE]);
  int fbfd = init_lcd();
  workMaster(tabChildPipe,global[LECTURE]);
  close(global[LECTURE]); // Fermeture du pipe d'ecoute
  // Fermeture des pipes des enfants
  for (pos = 0; pos < 1; pos++) {
    close (tabChildPipe[pos]);
  }
  // Fermeture du framebuffer
  close(fbfd);
  // Mise en attente de la fin des autres
  for (pos = 0; pos<1;pos ++) {
    waitpid(pidChildPipe[pos],NULL,0);
  }
  return EXIT_SUCCESS;
}
