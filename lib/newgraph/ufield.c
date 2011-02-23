/***************************************************************************\
graph library
Copyright (C) 1992-2003  Alexander Wagner

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

#include <math.h>
#include "mydraw_2d.h"
#include "ufield.h"

void ufield(int anzy,int xmin, int xmax, int ymin, int ymax,
	    double *u,double scale,
	    int xofs, int yofs, int xsize, int ysize)
{
  double si,co,vecangle=M_PI /4;
  int i,j,x,y,x1,y1,x2,y2,x3,y3,nox,noy;


  nox=xmax-xmin+2; /* +2 to make all arrows visible */
  noy=ymax-ymin+2;
  si=sin(vecangle);
  co=cos(vecangle);
  if (scale>0)
    {
      for (i=xmin;i<xmax;i++)
	for (j=ymin;j<ymax;j++)
	  {
	    x=xofs+(i-xmin+1)*xsize/(nox-1);
	    y=yofs+ysize-(j-ymin+1)*ysize/(noy-1);
	    x1=u[2*(i*anzy+j)]/scale;
	    y1=-u[2*(i*anzy+j)+1]/scale;
	    x2=x+x1-( co*x1+si*y1)/5;
	    y2=y+y1-(-si*x1+co*y1)/5;
	    x3=x+x1-( co*x1-si*y1)/5;
	    y3=y+y1-( si*x1+co*y1)/5;
	    x1+=x;y1+=y;
	    mydrawline(schwarz(),x,y,x1,y1);
	    mydrawline(schwarz(),x1,y1,x2,y2);
	    mydrawline(schwarz(),x1,y1,x3,y3);
	  }
    }
}
