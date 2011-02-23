#ifndef COLOR_H
#define COLOR_H

#include "basicdef.h"

/* this module should contain a collection of good color ramps. */
extern void GreenRedColRamp(double frac,double *r,double *g,double *b);

extern void SetColorRamp(void 
			 (*NewColRamp)(double,double *,double *,double *));
#endif
