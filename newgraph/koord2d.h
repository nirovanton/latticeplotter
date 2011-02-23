#ifndef KOORD2D_H
#define KOORD2D_H

extern void Koordinatensystem2D(int c,
				double xmin, double xmax,int logx, 
				double ymin, double ymax,int logy,
				int *xofs, int *yofs, int *xsize,int *ysize,
				char *xstr, char *ystr,char *xstru, 
				char *ystru);
void CoordLineY(int c,double min, double max,int log,
		int xofs, int yofs, int xsize,int ysize);
#endif











