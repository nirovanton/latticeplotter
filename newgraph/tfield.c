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
#include "tfield.h"

void tfield(int anzy,int xmin, int xmax, int ymin, int ymax,
	    double *t,double scale,
	    int xofs, int yofs, int xsize, int ysize, int rough)
{
  double si,co,l1,l2,tet,dxx,dxy,dyy,dd1,dd2,sq,*pos;
  int i,j,x,y,x1,y1,x2,y2,x3,y3,x4,y4,dx1,dy1,dx3,dy3,nox,noy,myxofs,myyofs;
  XPoint mypoint[4];

  nox=(xmax-xmin)+2; /* +2 to make all blobs visible */
  noy=(ymax-ymin)+2;
  myxofs=((xmax-xmin-1)%rough)/2;
  myyofs=((ymax-ymin-1)%rough)/2;
  if (scale>0)
    {
      for (i=0;i<(xmax-xmin-1)/rough+1;i++)
	for (j=0;j<(ymax-ymin-1)/rough+1;j++)
	  {
	    pos=&t[3*(((xmin+i)*rough+myxofs)*anzy+(ymin+j)*rough+myyofs)];
	    /* we do not need this address calculation!!! */
	    dxx=*(pos++);
	    dxy=*(pos++);
	    dyy=*(pos);
	    dd1=(dxx+dyy)/2;
            dd2=(dxx-dyy)/2;
	    sq=sqrt(dd2*dd2+dxy*dxy);
	    l1=dd1+sq;
	    l2=dd1-sq;
	    tet=-atan2(dxy,l1-dyy);
	    si=sin(tet);
	    co=cos(tet);
	    
	    x=xofs+(i*rough+myxofs+1)*xsize/(nox-1);
	    y=yofs+ysize-(j*rough+myyofs+1)*ysize/(noy-1);
	    x1=-co*l1/scale*rough;
	    y1=-si*l1/scale*rough;
	    dx1=y1*0.2;
            dy1=-x1*0.2;
	    x2=x-x1;
	    y2=y-y1;
	    x1+=x;
	    y1+=y;
	    x3=+si*l2/scale*rough;
	    y3=-co*l2/scale*rough;
	    dx3=y3*0.2;
            dy3=-x3*0.2;
	    x4=x-x3;
	    y4=y-y3;
	    x3+=x;
	    y3+=y;
	    if (l1>0){
	      mypoint[0].x=x1;
              mypoint[0].y=y1;
	      mypoint[1].x=x+dx1;
              mypoint[1].y=y+dy1;
	      mypoint[2].x=x2;
              mypoint[2].y=y2;
	      mypoint[3].x=x-dx1;
              mypoint[3].y=y-dy1;
	      mypolygon(schwarz(),mypoint,4);
	    }
	    else{
	      mypoint[0].x=x1+dx1;
              mypoint[0].y=y1+dy1;
	      mypoint[1].x=x2-dx1;
              mypoint[1].y=y2-dy1;
	      mypoint[2].x=x2+dx1;
              mypoint[2].y=y2+dy1;
	      mypoint[3].x=x1-dx1;
              mypoint[3].y=y1-dy1;
	      mypolygon(rot(),mypoint,4);
	    }
	    if (l2>0){
	      mypoint[0].x=x3;
              mypoint[0].y=y3;
	      mypoint[1].x=x+dx3;
              mypoint[1].y=y+dy3;
	      mypoint[2].x=x4;
              mypoint[2].y=y4;
	      mypoint[3].x=x-dx3;
              mypoint[3].y=y-dy3;
	      mypolygon(schwarz(),mypoint,4);
	    }
	    else{
	      mypoint[0].x=x3+dx3;
              mypoint[0].y=y3+dy3;
	      mypoint[1].x=x4-dx3;
              mypoint[1].y=y4-dy3;
	      mypoint[2].x=x4+dx3;
              mypoint[2].y=y4+dy3;
	      mypoint[3].x=x3-dx3;
              mypoint[3].y=y3-dy3;
	      mypolygon(rot(),mypoint,4);
	    }
	  }
    }
}
