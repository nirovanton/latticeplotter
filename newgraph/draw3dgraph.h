#ifndef _DRAW3DGRAPH_H
#define _DRAW3DGRAPH_H

extern void draw3dgraph(int xsize,int ysize,grstr graph,double **gp, int *getgraph,
		 int nox, int noy,
		 int *newdim,int newdata, int comments, long no_iterations,
		 double persp_factor, double scalefakt, 
		 double xangle,double yangle,double zangle,
		 double xmax,double xmin,double ymax,double ymin,
		 double zmin,double zmax,double zfakt,
		 Meshp **mep,int *meinit);

#endif
