#include <stdlib.h>
#include <math.h>
#include "color.h"
#include "mydraw_2d.h"


Colormap     cmap;

void (*ColRamp)(double ,double *,double *,double *);

void MyColRamp(double frac,double *red,double *green,double *blue){
  *red   = 0.991* (0.80*frac+0.2*frac);
  *green = 0.991* (0.80*frac+0.1); 
  *blue  = 0.991* (0.80*frac+0.2-0.2*frac);
}

void GreenRedColRamp(double frac,double *red,double *green,double *blue){
  *red   =  (1.0-frac);
  *green =  (0.80*frac+0.1); 
  *blue  =  (0.80*sin(M_PI*frac)+0.1);
}

void SetColorRamp(void (*NewColRamp)(double ,double *,double *,double *))
{
  unsigned int
    myncolors;
  XColor *mymap;
  int i;
  double red,green,blue;
  if (NewColRamp==NULL) ColRamp=MyColRamp;
  else ColRamp=NewColRamp;
  myncolors=GetFreeColors();

  if (myncolors<6){
    printf("Not enough colors (%i) to allocate standard colors!",
	   myncolors);
  }
  mymap = (XColor *) malloc(myncolors*sizeof(XColor));

  set_weiss(0);
  mymap [0].red   = 65000; 
  mymap [0].green = 65000;
  mymap [0].blue  = 65000;
  set_schwarz(1);
  mymap [1].red   = 0; 
  mymap [1].green = 0;
  mymap [1].blue  = 0;
  set_rot(2);
  mymap [2].red  = 65000; 
  mymap [2].green= 05000;
  mymap [2].blue = 0;
  set_gruen(3);
  mymap [3].red  = 0; 
  mymap [3].green= 65000;
  mymap [3].blue = 0;
  set_blau(4);
  mymap [4].red  = 0; 
  mymap [4].green= 20000;
  mymap [4].blue = 65000;
  set_gelb(5);
  mymap [5].red  = 65000; 
  mymap [5].green= 65000;
  mymap [5].blue = 30000;
  for (i=0; i<myncolors-6; i++) {
    ColRamp(1.0*i/(myncolors-7),&red,&green,&blue);
    mymap [i+6].red   = 65535*red;
    mymap [i+6].green = 65535*green;
    mymap [i+6].blue  = 65535*blue;
  }
  SetColorRampStart(6);
  SetColorRampEnd(myncolors-1);
  setcolormap(mymap, myncolors);
}
