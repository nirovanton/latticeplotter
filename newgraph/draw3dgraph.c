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

#include "view3d.h"
#include "mydraw.h"
#include "koord3d.h"
#include "objects3d.h"
#include "mygraph.h"
#include "draw3dgraph.h"

#define maxgraphs 20


void flaeche(grstr graph,double **gp, int *getgraph,
	     int nox, int noy,
	     double zmin, double zmax,int *newdim,
	     int newdata,Meshp **me, int *initialized, double zfakt)
{
  vec v={0,0,0};
  int i,j,g;

  if ((*newdim)&&(*initialized))
    {
      for (g=0;g<graph.anzNxN_R; g++)
	destroymesh((*me)[g]);
      free(*me);
      *newdim=0;
      *initialized=0;
      newdata=1;
    }
  if (!*initialized)
    {
      (*me)= (Meshp *) malloc(graph.anzNxN_R*sizeof(Meshp));
      for (g=0;g<graph.anzNxN_R; g++)
	(*me)[g]=initmesh(nox,noy,MESH);
      *initialized=1;
    }
  if (newdata==1)
    {
      for (g=0;g<graph.anzNxN_R; g++)
	if (getgraph[g]!=0)
	{
	  for (i=0;i<nox;i++)
	    for (j=0;j<noy;j++){
	      v[0]=i;v[1]=j;v[2]=gp[g][i*noy+j];
	      putinmesh((*me)[g],i,j,v);
	  }
	/*  coloronmeshlim((*me)[g],2,58,-1e-3,1e-3,rot,zmin,zmax);*/
	  coloronmesh((*me)[g],ColorRampStart(),
		      ColorRampEnd()-ColorRampStart());
	}
    }
  pushtrafs();
  /*    rotate(xaxis,90);*/
  scale(1.0/(nox-1),
	1.0/(noy-1),zfakt/(zmax-zmin));
  translate(-(nox-1)/2.0,
	    -(noy-1)/2.0,-(zmax+zmin)/2.0);
  for (g=0;g<graph.anzNxN_R; g++)
    if(getgraph[g]!=0) mesh((*me)[g]);
  poptrafs();
}

void draw3dgraph(int xsize,int ysize,grstr graph,double **gp, int *getgraph,
		 int nox, int noy,
		 int *newdim,int newdata, int comments, long no_iterations,
		 double persp_factor, double scalefakt, 
		 double xangle,double yangle,double zangle,
		 double xmax,double xmin,double ymax,double ymin,
		 double zmin,double zmax,double zfakt,
		 Meshp **mep,int *meinit)
{
  static  vec 
    vt={0.,-0.9,0.},vd={1.,0.,0.};
  int i;
  char comm[60];

  myclear();
  setviewglobal(xsize, ysize, persp_factor, persp_factor);
  pushtrafs();
  scale(scalefakt,scalefakt,scalefakt);
  if (comments) 
    {
      sprintf(comm,"%li iterations",no_iterations);
      for (i=0; i<graph.anzNxN_R; i++)
	if (getgraph[i]!=0)
	  sprintf(comm,"%s %s",graph.NxN_Rname[i],comm);
      text_3d(schwarz(),vt,vd,comm,comm,sizeinwc(0.4));
      
    }
  rotate(zaxis,1.0*zangle);
  rotate(xaxis,1.0*xangle);
  rotate(yaxis,1.0*yangle);
  rotate(zaxis,1.0*zangle);
  rotate(xaxis,-90);
  if (fabs(xmax-xmin)>fabs(ymax-ymin))
    scale(1,(ymax-ymin)/(xmax-xmin),1);
  else scale((xmax-xmin)/(ymax-ymin),1,1);
  flaeche(graph,gp,getgraph,nox,noy,zmin,zmax,newdim,
	  newdata,mep,meinit,zfakt);
  
  /**** Warum wird die koordinaten-neu definition gebraucht??? **/
  /* Ich muss es umskalieren, darum vielleicht zerstoeren, wenn es
     neu skaliert werden muss? */ 
  
  Koordinatensystem3D(schwarz(),
		      xmin,xmax,ymin,ymax,zmin,zmax,
		      1.,1.,zfakt,
		      "X","Y","Z",
		      "","","");
  
  poptrafs();  
  DRAW();
  myshow();
}



