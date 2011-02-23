#ifndef DRAW2DGRAPH_H
#define DRAW2DGRAPH_H

#include "graph.h"

#define NumberOfShapes 5
#define NumberOfLinetypes 2

#define NoShape      0
#define UpTriangle   1
#define DownTriangle 2
#define Square       3
#define Circle       4

extern void draw2dgraph(int xsize,int ysize,double **gp, 
			dim1 *no, int *draw, int nograph,
			int comments,  
			double xmin,double xmax,int logx,
			double ymin,double ymax,int logy, 
			int type, grdat *gr);
#endif


