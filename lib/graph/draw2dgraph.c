/***************************************************************************\
graph library
Copyright (C) 1992 - 2003  Alexander Wagner and Johannes Schlosser

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
#include <X11/Xlib.h>

#include "graph/mydraw_2d.h"
#include "graph/koord2d.h"
#include "graph/mygraph.h"
#include "graph/draw2dgraph.h"
#include "graph/graph.h"

void DrawUpTriangles(int mycolor, XPoint *myp, int no, double size,int fill){
  int i;
  XPoint points[4];

  for (i=0;i<no;i++){
    points[0].x=myp[i].x-size;
    points[0].y=myp[i].y-size*0.33333333333333333333*sqrt(3.0);
    points[1].x=myp[i].x+size;
    points[1].y=myp[i].y-size*0.33333333333333333333*sqrt(3.0);
    points[2].x=myp[i].x;
    points[2].y=myp[i].y+size*0.66666666666666666666*sqrt(3.0);

    switch (fill){
    case 0: 
      points[3].x=points[0].x;
      points[3].y=points[0].y;
      myline_polygon(mycolor,points,4); 
      break;
    case 1: mypolygon(mycolor,points,3); break;
    default: printf("draw2dgraph:DrawTriangles: fill type %i not defined\n",
		    fill);
    }
  }
}

void DrawDownTriangles(int mycolor, XPoint *myp, int no, double size,int fill){
  int i;
  XPoint points[4];

  for (i=0;i<no;i++){
    points[0].x=myp[i].x-size;
    points[0].y=myp[i].y+size*0.33333333333333333333*sqrt(3.0);
    points[1].x=myp[i].x+size;
    points[1].y=myp[i].y+size*0.33333333333333333333*sqrt(3.0);
    points[2].x=myp[i].x;
    points[2].y=myp[i].y-size*0.66666666666666666666*sqrt(3.0);

    switch (fill){
    case 0: 
      points[3].x=points[0].x;
      points[3].y=points[0].y;
      myline_polygon(mycolor,points,4); 
      break;
    case 1: mypolygon(mycolor,points,3); break;
    default: printf("draw2dgraph:DrawTriangles: fill type %i not defined\n",
		    fill);
    }
  }
}

void DrawSquare(int mycolor, XPoint *myp, int no, double size,int fill){
  int i;
  XPoint points[5];

  for (i=0;i<no;i++){
    points[0].x=myp[i].x-size;
    points[0].y=myp[i].y-size;
    points[1].x=myp[i].x+size;
    points[1].y=myp[i].y-size;
    points[2].x=myp[i].x+size;
    points[2].y=myp[i].y+size;
    points[3].x=myp[i].x-size;
    points[3].y=myp[i].y+size;

    switch (fill){
    case 0: 
      points[4].x=points[0].x;
      points[4].y=points[0].y;
      myline_polygon(mycolor,points,5); 
      break;
    case 1: mypolygon(mycolor,points,4); break;
    default: printf("draw2dgraph:DrawSquare: fill type %i not defined\n",
		    fill);
    }
  }
}

void DrawCircle(int mycolor, XPoint *myp, int no, double size,int fill){
  int i,x,y,r;

  for (i=0;i<no;i++){
    x=myp[i].x;
    y=myp[i].y;
    r=size;
    switch (fill){
    case 0:  mycircle(mycolor,x,y,r); break;
    case 1:  myfilledcircle(mycolor,x,y,r); break;
    default: printf("draw2dgraph:DrawCircle: fill type %i not defined\n",
		    fill);
    }
  }
}

#define fmin(x,y) ((x<y)?x:y)
#define fmax(x,y) ((x>y)?x:y)


void draw2dgraph(int xsize,int ysize,double **gp, 
		 dim1 *no, int *draw, int nograph,
		 int comments,  
		 double xmin,double xmax,int logx,
                 double ymin,double ymax,int logy, 
		 int type,grdat *gr)
{
  int i,j,xofs,yofs,maxno;
  XPoint *myp;

  /*
  char comm[60];
  if (comments) 
    {
      sprintf(comm,"%li iterations",get_time());
      for (i=0; i<graph.anz; i++)
	if (getgraph[i]&mask)
	  sprintf(comm,"%s %s",graph.name[i],comm);
      text_3d(schwarz(),vt,vd,comm,comm,sizeinwc(0.4));
      
    }
    */
  Koordinatensystem2D(schwarz(),xmin,xmax,logx,ymin,ymax,logy,&xofs,&yofs,
		      &xsize,&ysize,"X","Y","","");
  for (maxno=0,i=0;i<nograph;i++) 
    if (draw[i] && (*no[i][0]>maxno)) maxno=*no[i][0];

  myp= (XPoint *) malloc(maxno*sizeof(XPoint));
  if (myp==NULL) fprintf(stderr,"draw2dgraph:156 XPoint->malloc failed");
 for (i=0;i<nograph;i++){
    if (draw[i]){
      switch (type){
      case 0:
	for (j=0;j<*no[i][0];j++){
	  if (logx)
	    myp[j].x=xofs+log(fmax(fmin(j,xmax),xmin)*xsize)/
	      (log(*no[i][0]-1));
	  else
	    myp[j].x=xofs+(fmax(fmin(j,xmax),xmin)-xmin)/(xmax-xmin)*xsize;
	  if (logy)
	    myp[j].y=yofs+ysize-(log(fmax(fmin(gp[i][j],ymax),ymin))-log(ymin))
	      /(log(ymax)-log(ymin))*ysize;
	  else
	    myp[j].y=yofs+ysize-(fmax(fmin(gp[i][j],ymax),ymin)-ymin)/
	      (ymax-ymin)*ysize;
	}
	break;
      case 1:
	for (j=0;j<*no[i][0];j++){
	  if (logx)
	    myp[j].x=xofs+(log(fmax(fmin(gp[i][j*2],xmax),xmin))-log(xmin))
	      /(log(xmax)-log(xmin))*xsize;
	  else
	    myp[j].x=xofs+(fmax(fmin(gp[i][j*2],xmax),xmin)-xmin)/
	      (xmax-xmin)*xsize;
	  if (logy)
	    myp[j].y=yofs+ysize-(log(fmax(fmin(gp[i][j*2+1],ymax),ymin))-log(ymin))
	      /(log(ymax)-log(ymin))*ysize;
	  else
	    myp[j].y=yofs+ysize-(fmax(fmin(gp[i][j*2+1],ymax),ymin)-ymin)/(ymax-ymin)*ysize;
	}
	break;
      }
      /*mycolor=schwarz();*/ /* A color needs to be choosen in the menu. 
			    Should we allow for a color to be preset?
			    Maybe as a SetDefaultColor() routine?
			    SetDefaultLine(), SetDefaultShape(),
			    SetDefaultSize(). Each
			    graph will then require some extra datafields
			    for linestyle, Shape and color! */
      if (gr[i].linetype & 1) myline_polygon(gr[i].color,myp,*no[i][0]);
      switch (gr[i].shape){
      case 0: break;
      case UpTriangle: DrawUpTriangles(gr[i].color,myp,*no[i][0],
				       gr[i].size*xsize/100,gr[i].fill); break;
      case DownTriangle: DrawDownTriangles(gr[i].color,myp,*no[i][0],
				       gr[i].size*xsize/100,gr[i].fill); break;
      case Square: DrawSquare(gr[i].color,myp,*no[i][0],
				       gr[i].size*xsize/100,gr[i].fill); break;
      case Circle: DrawCircle(gr[i].color,myp,*no[i][0],
				       gr[i].size*xsize/100,gr[i].fill); break;
      default: printf("draw2dgraph: shape %i not defined",gr[i].shape);
      }
    }
  }
  free(myp);
  /*myshow();*/ /* This should be left up to neuzeichnen, since you may want 
		   to use several of these graphs in one graphic! */
}


