#ifndef _DRAW3DCONTOUR_ 
#define _DRAW3DCONTOUR_


extern void draw3dcontour(int xsize,int ysize,Contour  **c, int anzcont,
			  int newdata,
			  double persp_factor, double scalefakt, 
			  double xangle,double yangle,double zangle,
			  double xmax,double xmin,double ymax,double ymin,
			  double zmin,double zmax);

#endif
