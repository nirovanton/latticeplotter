#ifndef _CONTOUR_H
#define _CONTOUR_H

/* 

This module calculates contourlines for a given two dimensional lattice
with constant latticespacings (assumed to be 1).

The input data is assumed to be of the form f(x,y)= data[x*noy+y] with
nox is the x-dimension and noy is the y-dimension of the lattice.

It puts out data for a contour line (x0,y0,x1,y1,x2,y2,...) as well as 
an array of integers giving
  length[0]= Number of contours
  length[1]= not assigned 
  length[2]= Points for contour 1
  length[3]= Signature for contour 1 (-4, +4 or 0)
  length[4]= Points for contour 2
  length[3]= Signature for contour 2 (-4, +4 or 0)
  ...

   O | X ; X ; X   X   X | O   O
   ;   -   -   ;       ;   ;
   O ; O ; O | X ; X ; X | O   O
           ;   |   ;   |   ;
   O   O   O ; O | X | O ; O   O
               ;   |   ;
   O   O   O   O ; O ; O   O   O
For each step we are looking in one of four directions. This tells us what
the four boundarys are that we have to consider. One of these boundarys will
have a step and we iterate the procedure with the new  direction.

To make this an efficient procedure we have to mark the boundarys that we have
already checked to
  a) not haveing to check them again for one contour (and to mark returning 
     points for closed contours)
  b) not to calculate the same contour again when checking for further 
     contours.



*/

extern void contour(double *data,int nox, int noy, double cut,
	     double **cont,int **length,int periodic,
	     long (*newind)(int, int),int *boundary);
#endif
