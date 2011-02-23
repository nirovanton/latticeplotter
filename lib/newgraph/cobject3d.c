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
#include "newgraph/mydraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

# define MAXPOINTS 100

static int
    defaultcolor=20,
    toggle= 1;


#define TV(o,n) if (!o->transfv[n]){veq (o->tva[n],vsum(matvecmul(o->mc2wcm,o->va[n]),o->mc2wcv));o->transfv[n]=1;}



void calcnormals(Gobject *o, int mallocnormals){

  int    aktpoly = -1,
         edges,i1,i2,i3,
         normanz = 0,
         aktnorm = 0;
  
  vec    norm;
  
  double erg;       
         
  while ((edges = o->polygons[++aktpoly])!=0){
    normanz++;
    aktpoly+=edges+2;
  }
/*  printf("Calcnormals.Normanzahl:%i\n",normanz);*/
  if (mallocnormals)
    o->na=malloc(sizeof(vec)*normanz);
  aktpoly=0;  
  for (aktnorm=0;aktnorm<normanz;aktnorm++){
    i1=o->polygons[aktpoly+3];
    i2=o->polygons[aktpoly+4];
    i3=o->polygons[aktpoly+5];
    veq(norm,vnorm(vp(vdiff(o->va[i2],o->va[i1]),vdiff(o->va[i3],o->va[i1]))));
    if (o->type==V3DPOLYHEDRON){
      erg=sp(vdiff(o->va[i1],o->posmc),norm);vreset();
      if (erg<0) veq(norm,sm(-1,norm));
      if (erg==0){printf("Fehler in calcnormals: Normale nicht definiert\n"); }
    }
/*    pv(norm);*/
    veq(o->na[aktnorm],norm);
    o->polygons[aktpoly+1]=aktnorm;
    aktpoly+=o->polygons[aktpoly]+3;
  }
}

/*****************************************
  Mesh-bezogene Funktionen
*****************************************/

Mesh *initmesh(int xdim, int ydim, int mode){
  Mesh *m;
  int i,j,*p,indx1,indx2;
  m=malloc(sizeof(Mesh));
  m->va=malloc(xdim*ydim*sizeof(vec));
  m->tva=malloc(xdim*ydim*sizeof(vec)); 
  m->transf2dc=malloc(xdim*ydim*sizeof(int));
  m->points=malloc(xdim*ydim*sizeof(DPoint));
  m->Garray=malloc((xdim-1)*(ydim-1)*sizeof(Gobject));
  if (mode==MESH){
    m->pol=malloc(((xdim-1)*(ydim-1)*12+1)*sizeof(int));
  }else 
  if (mode==MESHONESIDED){
    m->na=malloc((xdim-1)*(ydim-1)*2*sizeof(vec));
    m->pol=malloc(((xdim-1)*(ydim-1)*12+1)*sizeof(int));
  }else 
  if (mode==SQUAREMESH){
    m->pol=malloc(((xdim-1)*(ydim-1)*7+1)*sizeof(int));
  }else 
  if (mode==SQUAREMESHONESIDED){
    m->na=malloc((xdim-1)*(ydim-1)*2*sizeof(vec));
    m->pol=malloc(((xdim-1)*(ydim-1)*7+1)*sizeof(int));
  }else 
  if (mode==MESHTOGGLE){
    m->pol=malloc(((xdim-1)*(ydim-1)*7+1)*sizeof(int));
  }else {
    printf("incorrect mode choosen in initmesh\n");
    exit(1);
  }  
  m->xdim=xdim;
  m->ydim=ydim;
  m->mode=mode;
  m->normalsinitialized=0;
  p=m->pol;
  for (i=0;i<xdim-1;i++)
    for (j=0;j<ydim-1;j++){
      indx1=i+j*(xdim-1);
      indx2=i+j*xdim;
      m->Garray[indx1].type=mode;
      m->Garray[indx1].mc2wcm=m->mc2wcm;
      m->Garray[indx1].mc2wcmn=m->mc2wcmn;
      m->Garray[indx1].mc2wcv=m->mc2wcv;
      m->Garray[indx1].posmc=m->va[indx2+xdim];
      m->Garray[indx1].poswc=m->tva[indx2+xdim];
      m->Garray[indx1].na=m->na;
      m->Garray[indx1].va=m->va;
      m->Garray[indx1].tva=m->tva;
      m->Garray[indx1].polygons=p;
      m->Garray[indx1].points=m->points;
      if ((mode==SQUAREMESHONESIDED)
        ||(mode==SQUAREMESH)
        ||(mode==MESHTOGGLE)){
        m->Garray[indx1].drawstruct=drawsquaremeshsegment;
        if (mode==MESHTOGGLE)m->Garray[indx1].drawstruct=drawmeshsegment;
        *(p++)=4;            /* Anzahl der Ecken */
        *(p++)=0;            /* Index des Normalenvektors in na */
        *(p++)=defaultcolor; /* Farbe des Mesh-Polygons */
        *(p++)=indx2;        /* Es folgen die Indizes der Eckvektoren */
        *(p++)=indx2+1;      /* Sie werden in va[] tva[] und points[] */
        *(p++)=indx2+1+xdim; /* verwendet. */
        *(p++)=indx2+xdim;
      } else {
	int i1,i2,i3,i4;
	i1=indx2;i2=i1+1;i4=indx2+xdim;i3=i4+1;
        m->Garray[indx1].drawstruct=drawmeshsegment;
        *(p++)=3;            
        *(p++)=0;        
        *(p++)=indx1/*defaultcolor*/; 
        *(p++)=i1;        
        *(p++)=i2;      
        *(p++)=i4;
        *(p++)=3;            
        *(p++)=0;        
        *(p++)=indx1/*defaultcolor*/; 
        *(p++)=i3; 
        *(p++)=i4;
        *(p++)=i2;      
      }
    }
  *p=0;                                     
  return m;   
}

void destroymesh(Mesh *m){
  free(m->va);
  free(m->tva);
  free(m->transf2dc);
  free(m->points);
  free(m->Garray);
  free(m->pol);
  if ((m->mode==SQUAREMESHONESIDED)||(m->mode==MESHONESIDED))
    free(m->na);
  free(m);
}

void putinmesh(Mesh* mymesh, int xind,int yind, vecp v){
  veq(mymesh->va[xind+mymesh->xdim*yind],v);
}

void putinmeshz(Mesh* mymesh, int xind,int yind, double z){
  mymesh->va[xind+mymesh->xdim*yind][2]=z;
}

double getmeshz(Mesh* m,int xind, int yind){
  return m->va[xind+m->xdim*yind][2];
}


/* 
   Die folgende Function verteilt Farbwerte zwischen unterem Index lci und
   lci+cn auf die Polygone eines Meshes, je nach der mittleren z-Hoehe in
   Modellkoordinaten.
*/

void coloronmesh(Mesh* m,int lci,int cn){
  double
    max, min,dc,akt;
  int i,j,dum,*p,*c;
  vec *v;
  v=m->va;
  p=m->pol;
  max=min=v[0][2];
  for (i=0;i<m->xdim*m->ydim;i++){
    if (v[i][2]>max) max=v[i][2];
    if (v[i][2]<min) min=v[i][2];
  }
  dc=(max-min)/cn;
  while (*p>0){
    dum=*p;
    c=p+=2;p++;
    akt=0;
    for (j=0;j<dum;j++)akt+=v[*p++][2];
    if (dc!=0) *c=lci+(akt/dum-min)/dc;
    else *c=lci;
  }
}      

void coloronmeshlim(Mesh* m,int lci,int cn, 
		    double minl, double maxl, int col,
		    double zmin, double zmax)
{
  double dc,akt;
  int j,dum,*p,*c;
  vec *v;
  v=m->va;
  p=m->pol;
  dc=(zmax-zmin)/cn;
  while (*p>0){
    dum=*p;
    c=p+=2;p++;
    akt=0;
    for (j=0;j<dum;j++)akt+=v[*p++][2];
    if ((akt<minl)||(akt>maxl)) *c=lci+(akt/dum-zmin)/dc;
    else *c=col;
  }
}

void mesh(Mesh* mymesh){
  register int i;
  vecp v;
  matp m;
  int *ip,calculatedisplaykoordinates=1;  
  i=mymesh->mode;
  if ((!mymesh->normalsinitialized)
      &&((i==MESHONESIDED)||(i==SQUAREMESHONESIDED))) {
    calcnormals(&mymesh->Garray[0],1);
    mymesh->normalsinitialized=1;
    /*calculatedisplaykoordinates=0*/
  }
  ip=mymesh->transf2dc;  
  meq(mymesh->mc2wcmn,getmc2wcmn());
  meq(mymesh->mc2wcm,getmc2wcm());
  veq(mymesh->mc2wcv,getmc2wcv());
  m=mymesh->mc2wcm;
  v=mymesh->mc2wcv;
  for (i=0;i<mymesh->xdim*mymesh->ydim;i++){
    veq(mymesh->tva[i],vsum(matvecmul(m,mymesh->va[i]),v));
    *(ip++)=0;
  }
  if (calculatedisplaykoordinates)
    for (i=0;i<mymesh->xdim*mymesh->ydim;i++)
      wc2dc(&mymesh->points[i].x,&mymesh->points[i].y,mymesh->tva[i]);
  meq(mymesh->mc2wcmn,getmc2wcmn());
  for (i=0;i<(mymesh->xdim-1)*(mymesh->ydim-1);i++)insertGobject(&mymesh->Garray[i]);
}

void togglemesh(void){
  toggle-=1;
  if(toggle<0) toggle*=-1;
}
           

/*****************************************
  Contour-bezogene Funktionen
*****************************************/


Contour *initcontour(int dummy){
  Contour *c;
  c=malloc(sizeof(Contour));
  c->vamax=100;
  c->vaakt=-1;
  c->polmax=100;
  c->polakt=-1;
  c->va=malloc(c->vamax*sizeof(vec));
  c->pol=malloc((c->polmax*6+1)*sizeof(int));
  *c->pol=0;
  c->initialized=0;                                
  return c;   
}

void destroycontour(Contour *c){
  free(c->va);
  free(c->pol);
  if (c->initialized){
    free(c->na);
    free(c->tva);
    free(c->sp);
    free(c->points);
    free(c->Garray);
  } else 
    printf("cobjects3d:destroycontour: destroying uninitialized contour\n");
  free(c);
}

int putpointintocontour(Contour* mycontour, vecp v){
  if (++mycontour->vaakt>=mycontour->vamax) {
    mycontour->vamax*=1.5;
    mycontour->va=realloc(mycontour->va, mycontour->vamax*sizeof(vec));
  }   
  veq(mycontour->va[mycontour->vaakt],v);
  return mycontour->vaakt;
}

void puttriangleincontour(Contour* mycontour, int ind1, int ind2, int ind3, int basecolor){
  int *i;
  if (++mycontour->polakt>=mycontour->polmax) {
    mycontour->polmax*=1.5;
    mycontour->pol=realloc(mycontour->pol, (mycontour->polmax*6+1)*sizeof(int));
  }
  i=&mycontour->pol[mycontour->polakt*6];
  *(i++)=3; /* drei Ecken */
  *(i++)=mycontour->polakt;
  *(i++)=basecolor;
  *(i++)=ind1;
  *(i++)=ind2;
  *(i++)=ind3;
  *i=0;
}

void contour3d(Contour* c){
  int i,j,*k;
  vecp v;
  matp m;
  if (c->initialized==0){
    c->tva=malloc((c->vaakt+1)*sizeof(vec));
    c->na=malloc((c->polakt+1)*sizeof(vec));
    c->sp=malloc((c->polakt+1)*sizeof(vec));
    c->points=malloc((c->polakt+1)*sizeof(DPoint));    
    c->Garray=malloc((c->polakt+1)*sizeof(Gobject));
    for (i=0;i<=c->polakt;i++) {
      c->Garray[i].na=c->na;
      c->Garray[i].tva=c->tva;
      c->Garray[i].poswc=(vecp) &c->sp[i]; /* compiler distinguishes between
                                              vec* and vecp */
      c->Garray[i].points=c->points;
      c->Garray[i].polygons=&c->pol[i*6];
      c->Garray[i].drawstruct=drawtrianglelight;
      j=c->pol[i*6];
      veq(c->na[i],vnorm(vp(vdiff(c->va[j+4],c->va[j+3]),vdiff(c->va[j+5],c->va[j+3]))));
    }
    c->initialized=1;
  }
  /* Transformation, die jedes Mal durchlaufen wird. */
  meq(c->mc2wcmn,getmc2wcmn());
  meq(c->mc2wcm,getmc2wcm());
  veq(c->mc2wcv,getmc2wcv());
  m=c->mc2wcm;
  v=c->mc2wcv;
  for (i=0;i<=c->vaakt;i++) {
    veq(c->tva[i],vsum(matvecmul(m,c->va[i]),v));
    wc2dc(&c->points[i].x,&c->points[i].y,c->tva[i]);
  }
  for (i=0;i<=c->polakt;i++){ /* Schwerpunkt in Weltkoordinaten */
    k=&c->pol[i*6+3];
    veq(c->sp[i],sm(0.33333,
		    vsum(vsum(c->tva[*k],c->tva[*(k+1)]),c->tva[*(k+2)])));
    insertGobject(&c->Garray[i]);
  }
}
    
