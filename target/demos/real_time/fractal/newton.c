// Dynamical systems and fractals
// K.-H. Becker & M. D\"orfler
// Cambridge University Press (1990), chapter.4
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "newton.h"

void multiplie(float *r1,float *i1,float *r2,float *i2) {
  float tmp;
  tmp=((*r1) * (*r2)-(*i1) * (*i2));
  *i1=((*i1)*(*r2)+(*i2)*(*r1));
  *r1=tmp;
}

void addition(float *r1,float *i1,float *r2,float *i2) {
  *r1=((*r1)+(*r2));
  *i1=((*i1)+(*i2));
}

void division(float *r1,float *i1,float *r2,float *i2) {
  float tmp;
  tmp=((*r1) * (*r2)+(*i1) * (*i2)) / ((*r2)*(*r2)+(*i2)*(*i2));
  *i1=((*i1) * (*r2)-(*r1) * (*i2)) / ((*r2)*(*r2)+(*i2)*(*i2));
  *r1=tmp;
}

void newton(float *ri,float *ii) {
  float r,i,r2,i2;
  r2=*ri; i2=*ii;
  r=*ri;  i=*ii;
  multiplie(&r2,&i2,&r,&i);  // z -> z^2  dans (r2,i2)
  multiplie(&r,&i,&r2,&i2);  // z -> z^3  dans (r ,i )
  r=r-1.;                    // z^3-1
  r2=r2*3;
  i2=i2*3;
  division(&r,&i,&r2,&i2);   // (z^3-1)/(2*z^2)
  *ri=*ri-r;
  *ii=*ii-i;                 // z-(z^3-1)/(2*z^2)
}

inline float module(float r,float i){
  return(r*r+i*i);
}
