/***************************************************************************\
graph library
Copyright (C) 1992-2003  Alexander Wagner and Johannes Schlosser

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

e-mail: Alexander.Wagner@ndsu.nodak.edu
\***************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "graph/view3d.h"
#include "graph/mydraw.h"
#include "graph/koord3d.h"
#include "graph/objects3d.h"
#include "graph/mygraph.h"
#include "graph/draw3dcontour.h"

/* The draw3dcontour routine is different from the draw3dgraph routine
   in that the graphic data and initialization of these data have been
   perfromed already by the user. So the reoutine is only required to 
   draw the structure with some given viewport data. */

void draw3dcontour(int xsize,int ysize,Contour  **c, int anzcont,
		 int newdata,
		 double persp_factor, double scalefakt, 
		 double xangle,double yangle,double zangle,
		 double xmax,double xmin,double ymax,double ymin,
		 double zmin,double zmax)
{
  int i;

  myclear();
  setviewglobal(xsize, ysize, persp_factor, persp_factor);
  pushtrafs();
  scale(scalefakt,scalefakt,scalefakt);
  /*  rotate(zaxis,1.0*zangle);*/
  rotate(xaxis,1.0*xangle);
  rotate(yaxis,1.0*yangle);
  /*  rotate(zaxis,1.0*zangle);*/
  rotate(xaxis,-90);
  if (fabs(xmax-xmin)>fabs(ymax-ymin))
    scale(1,(ymax-ymin)/(xmax-xmin),1);
  else scale((xmax-xmin)/(ymax-ymin),1,1);

  for (i=0; i<anzcont; i++)
    contour3d(c[i]);
  
  /* Here should perhaps be the option of not drawing a complete koordinate
     system but just a box or even nothing. */
  if (anzcont==0){ xmin=0;xmax=1;ymin=0;ymax=1;zmin=0;zmax=1;}

  Koordinatensystem3D(schwarz(),
		      xmin,xmax,ymin,ymax,zmin,zmax,
		      1.,1.,1.,
		      "X","Y","Z",
		      "","","");
  
  poptrafs();  
  DRAW();
  myshow();
}



