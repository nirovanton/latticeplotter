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

/* 

This module calculates contourlines for a given two dimensional lattice
with constant latticespacings (assumed to be 1).

The input data is assumed to be of the form f(x,y)= data[x*noy+y] with
nox is the x-dimension and noy is the y-dimension of the lattice.

It puts out data for a contour line (x0,y0,x1,y1,x2,y2,...) as well as 
an array of integers giving
  length[0]= Number of contours
  length[1]= empty
  length[2]= Points for contour 1
  length[3]= Number of corners  (i.e. -4,0,4 for positive,periodic
                                 and negative domain)
  ...
  length[2*i  ]= Points for contour i
  length[2*i+1]= Number of corners of contour i.

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newgraph/basicdef.h" /*for typedef int indfn(int,int)*/
#include <contour.h>

#define horizontal 0
#define vertical 1


static short *excl;
static int lattx,latty;
static int contl=50,indexmax;
static long (*ind)(int, int);

static long stdind(int x, int y)
{
  if (x<0) x=(x+1)%lattx-1+lattx; else x%=lattx;
  if (y<0) y=(y+1)%latty-1+latty; else y%=latty;
  return x*latty+y;
}

static int excluded(int i,int j,int mode)
{
  return (excl[(*ind)(i,j)]>>(mode*2))&3;
}

static void set_exclude(int i, int j,int mode,int set)
{
  excl[(*ind)(i,j)]&=(3 <<((mode^1)*2)); 
  excl[(*ind)(i,j)]|=(set <<((mode)*2));
}



static void border(int *b,int i,int j,double d0,double d1,
	    double cut,double *cutp,int dirmode)
{
  int t;
  t=excluded(i,j,dirmode);
  switch (t)
    {
    case 0:
      if (((d0>=cut)&&(d1<cut))||((d0<cut)&&(d1>=cut)))
	{
	  set_exclude(i,j,dirmode,3);
	  *cutp=(cut-d0)/(d1-d0);
	  *b=4;
	}
      else
	{
	  *b=0;
	}
      break;
    case 1:
      *cutp=(cut-d0)/(d1-d0);
      *b=5;
      break;
    case 3:
      *cutp=(cut-d0)/(d1-d0);
      *b=3;
      break;
    default :
      /*      printf("Error in contour b=%d encountered\n",t);*/
      *b=0;
    }
}

static int next_contour(double *data,double cut, int *i,int *j,
			int dir[2],double *x, double *y, int *charac,
			int periodic,int *boundary)
{
  double d0,d1,d2,d3,cutp[4];
  int k,dirmode,finish=0,dird[2],b[3],bsum,dirt,side,orient;


  /* Check  if contour is runing on the boundary. 
     If that is the case run on the boundary until new contour line
     or corner encountered. For the exclusion the contours
     run along -1 rather than 0.*/
  if (periodic==0){
    if ((dir[0] ==0)&&((*i==lattx-1)||(*i==-1))){
      *charac+=1;
      dirmode=vertical;
      if (*i==-1) {side=1;} 
      else        {side=0;}
      if (dir[1]==-1){ 
	for(*j-=1;(*j>=0)&&(data[(*ind)(*i+side,*j)]>cut);(*j)--);
	if (*j<0){
	  *i+=side;
	  *x=*i;
	  *y=0;
	  if (*i==0) dir[0]=1;
	  else dir[0]=-1;
	  dir[1]=0;
	  return 0;
	}
      }
      else{
	for((*j)++;(*j<latty)&&(data[(*ind)(*i+side,*j)]>cut);(*j)++);
	if (*j>=latty) {
	  *i+=side;
	  *j=latty-1;
	  *x=*i;
	  *y=*j;
	  if (*i==0) dir[0]=1;
	  else dir[0]=-1;
	  dir[1]=0;
	  return 0;
	}
      }
   
      d0=data[(*ind)(*i+side,*j)];
      d1=data[(*ind)(*i+side,*j-dir[1])]; 
      *x=*i+side;
      *y=*j-dir[1]*(cut-d0)/(d1-d0);
      *i+=side;
      if (dir[1]==1) (*j)--;
      if (*i==0) dir[0]=1;
      else dir[0]=-1;
      dir[1]=0;
      if (excluded(*i,*j,dirmode^1)==1) {
	set_exclude(*i,*j,dirmode^1,3);
	return 1;
      }
      else set_exclude(*i,*j,dirmode^1,3); /* was 2...*/
      return 0;
    }
    
    if ((dir[1]==0)&&((*j==latty-1)||(*j==-1))){
      *charac+=1;
      dirmode=horizontal;
      if (*j==-1) {side=1;} 
      else        {side=0;}
      if (dir[0]==-1){ 
	for((*i)--;(*i>=0)&&(data[(*ind)(*i,*j+side)]>cut);(*i)--);
	if (*i<0) {
	  *j+=side;
	  *x=0;
	  *y=*j;
	  if (*j==0) dir[1]=1;
	  else dir[1]=-1;
	  dir[0]=0;
	  return 0;
	}
      }
      else {
	for((*i)++;(*i<lattx)&&(data[(*ind)(*i,*j+side)]>cut);(*i)++);
	if (*i>=lattx) {
	  *j+=side;
	  *i=lattx-1;
	  *x=*i;
	  *y=*j;
	  if (*j==0) dir[1]=1;
	  else dir[1]=-1;
	  dir[0]=0;
	  return 0;
	}
      }
      
      d0=data[(*ind)(*i,*j+side)];
      d1=data[(*ind)(*i-dir[0],*j+side)]; 
      *x=*i-dir[0]*(cut-d0)/(d1-d0);
      *y=*j+side;
      *j+=side;
      if (dir[0]==1) (*i)--;
      if (*j==0) dir[1]=1;
      else dir[1]=-1;
      dir[0]=0;
      if (excluded(*i,*j,dirmode^1)==1) {
	set_exclude(*i,*j,dirmode^1,3);
	return 1;
      }
      else set_exclude(*i,*j,dirmode^1,3);/* was 2...*/
      return 0;
    }
  }
  
  if (dir[0]==0) dirmode=vertical; else dirmode=horizontal;
  if (dir[0]<0) dird[0]=-1; else dird[0]=0;
  if (dir[1]<0) dird[1]=-1; else dird[1]=0;
  d0=data[(*ind)(*i,*j)];
  d1=data[(*ind)(*i+dir[0],*j+dir[1])]; 
  d2=data[(*ind)(*i+dir[0]+abs(dir[1]),*j+dir[1]+abs(dir[0]))];
  d3=data[(*ind)(*i+abs(dir[1]),*j+abs(dir[0]))];
  if (d0>d3) orient=1; else orient=-1;

  border(b,*i+dird[0],*j+dird[1],d0,d1,cut,cutp,dirmode^1);
  border(b+1,*i+dir[0],*j+dir[1],d1,d2,cut,cutp+1,dirmode);
  border(b+2,*i+abs(dir[1])+dird[0],*j+abs(dir[0])+dird[1],
	 d3,d2,cut,cutp+2,dirmode^1);
  bsum=(b[0]&4)+(b[1]&4)+(b[2]&4);
  if ((12==bsum))
    {
/*      cutp[3]=(cut-d3)/(d0-d3);
      if (cutp[0]+cutp[2]<cutp[1]+cutp[3])*/
      if ((((d0+d1+d2+d3)/4-cut)*(d3-d0)>0))
	{
	  if (!(b[1]&1))
	  set_exclude(*i+dir[0],*j+dir[1],dirmode,0);
	  b[1]=0;
	  if (!(b[2]&1))
	  set_exclude(*i+abs(dir[1])+dird[0],
		      *j+abs(dir[0])+dird[1],dirmode^1,0);
	  b[2]=0;
	  if (b[0]==5) {finish=1; b[0]=4;}
	  set_exclude(*i+dird[0],*j+dird[1],dirmode^1,3);
	}
      else
	{
	  if (!(b[0]&1))
	  set_exclude(*i+dird[0],*j+dird[1],dirmode^1,0);
	  b[0]=0;
	  if (!(b[1]&1))
	  set_exclude(*i+dir[0],*j+dir[1],dirmode,0);
	  b[1]=0;
	  if (b[2]==5){ finish=1;b[2]=4;}
	  set_exclude(*i+abs(dir[1])+dird[0],
		      *j+abs(dir[0])+dird[1],dirmode^1,3);
	}
    }
  if (bsum==8) {for (k=0;k<3;k++) {if (b[k]==5) b[k]=0;}}
  if (bsum==0) printf("Error in contour: could not close contour: bsum=0");
  if (b[0]&4)
    {
      *charac+=orient;
      if (b[0]&1) 
	{
	  set_exclude(*i+dird[0],*j+dird[1],dirmode^1,3);
	  finish=1;
	}
      *x=*i+dir[0]*cutp[0];
      *y=*j+dir[1]*cutp[0];
      *i+=dird[0];
      *j+=dird[1];
      dirt=dir[0];
      dir[0]=-abs(dir[1]);
      dir[1]=-abs(dirt);
    }
  else if (b[1]&4)
    {
      if (b[1]&1)
	{
	  set_exclude(*i+dir[0],*j+dir[1],dirmode,3);
	  finish=1;
	}
      *x=*i+dir[0]+abs(dir[1])*cutp[1];
      *y=*j+dir[1]+abs(dir[0])*cutp[1];
      *i+=dir[0];
      *j+=dir[1];
    }
  else if (b[2]&4)
    {
      *charac-=orient;
      if (b[2]&1)
	{
	  set_exclude(*i+abs(dir[1])+dird[0],
		      *j+abs(dir[0])+dird[1],dirmode^1,3);
	  finish=1;
	}
      *x=*i+abs(dir[1])+dir[0]*cutp[2];
      *y=*j+abs(dir[0])+dir[1]*cutp[2];
      *i+=abs(dir[1])+dird[0];
      *j+=abs(dir[0])+dird[1];
      dirt=dir[0];
      dir[0]=abs(dir[1]);
      dir[1]=abs(dirt);
    }
  else
    {
      fprintf(stderr,"Error in contour: could not close contour:bsum=%d\n",
	      bsum);
      return 1;
    }

/* This condition checks   */
/* wether the contour runs */
/* into the boundary.      */

  if(periodic==0){
    if (((dir[0]==1)&&(*i==lattx-1))||((dir[0]==-1)&&(*i==0))){
      *boundary=1;
      *charac+=1;
      dir[0]=0;
      if (data[(*ind)(*i,*j)]>cut) dir[1]=-1;
      else dir[1]=1;
      if (*i==0) *i=-1;
    }
    else
      if (((dir[1]==1)&&(*j==latty-1))||((dir[1]==-1)&&(*j==0))){
	*boundary=1;
	*charac+=1;
	dir[1]=0;
	if (data[(*ind)(*i,*j)]>cut) dir[0]=-1;
	else dir[0]=1;
	if (*j==0) *j=-1;
      }
  }
  
  return finish;

}


void startcontour(double *data,int x,int y,double cut, double **cont,
		  int *index,int startdir[2],int *charact,int periodic,
		  int *boundary)
{
  int dirmode,dir[2],endcontour=0;
  double d0,d3,cutp;

  *boundary=0;
  *charact=0;
  dir[0]=startdir[0];
  dir[1]=startdir[1];
  if (dir[0]==0) dirmode=vertical; else dirmode=horizontal;
  set_exclude(x,y,dirmode,1);
  d0=data[(*ind)(x,y)];
  d3=data[(*ind)(x+abs(dir[1]),y+abs(dir[0]))];
  cutp=(cut-d0)/(d3-d0);
  (*cont)[*index*2]=x+abs(dir[1])*cutp;
  (*cont)[*index*2+1]=y+abs(dir[0])*cutp;
  (*index)++;
  if((*index)>=indexmax)
    {
      indexmax+=contl;
      *cont=(double*)realloc(*cont,(indexmax*2+10)*sizeof(double));
      if (*cont==NULL) { 
	fprintf(stderr,"Error in realloc in contour() for *cont\n"); 
	exit(1);}
      if (*index> 4*lattx*latty) {
	fprintf(stderr,
		"contour.c: startcontour:"
		"More contour points than lattice points %d",
		*index);
	return;
      }
    }
  
  for(;0==endcontour;(*index)++){
    
    if((*index)>=indexmax)
      {
	indexmax+=contl;
	*cont=(double*)realloc(*cont,(indexmax*2+10)*sizeof(double));
	if (*cont==NULL) { fprintf(stderr,"Error in malloc in contour()" 
				   " for *cont\n"); 
			   exit(1);}
	if (*index>4*lattx*latty) return; 
      }
    endcontour=next_contour(data,cut,&x,&y,dir,&(*cont)[*index*2],
			    &(*cont)[*index*2+1],charact,periodic,
			    boundary);
  }

  if((*index)>=indexmax)
    {
      indexmax+=contl;
      *cont=(double*)realloc(*cont,(indexmax*2+10)*sizeof(double));
      if (*cont==NULL) { fprintf(stderr,"Error in malloc in contour()" 
				 " for *cont\n"); 
			 exit(1);}
      if (*index>4*lattx*latty) return; 
    }
}

void contour(double *data,int nox, int noy, double cut,
	     double **cont,int **length,int periodic,
	     long (*newind)(int, int),int *boundary)
{
  int i,j,L,dir[2],index=0,index_old,charact;

  if (newind==NULL) ind = &stdind;
  else ind = newind;
  lattx=nox;
  latty=noy;
  excl=(short*) malloc(lattx*latty*sizeof(short));
  if (excl==NULL) { fprintf(stderr,"Error in malloc in contour() for excl\n"); 
		    exit(1);}
  memset(excl,0,lattx*latty*sizeof(short));
  /* for (i=0;i<lattx*latty;i++) excl[i]=0;*/
  *length= (int*) malloc(11*2*sizeof(int)); /* ??? */
  if (*length==NULL) 
    { fprintf(stderr,"Error in malloc in contour() for *length\n"); 
      exit(1);}
  (*length)[0]=0;
  indexmax=contl;
  *cont=(double*) malloc((indexmax*2+10)*sizeof(double));
  if (*cont==NULL) 
    { fprintf(stderr,"Error in malloc in contour() for *cont\n"); 
      exit(1);}

  /* purely vertical Contours */
  dir[0]=1; dir[1]=0;
  for (i=0;i<noy-1;i++)
    {
      if (!excluded(0,i,horizontal)&&
	  (((data[(*ind)(0,i)]>=cut)&&(data[(*ind)(0,i+1)]<cut))
	   ||((data[(*ind)(0,i)]<cut)&&(data[(*ind)(0,i+1)]>=cut)))) 
	{
	  index_old=index;
	  startcontour(data,0,i,cut,cont,&index,dir,&charact,periodic,
		       boundary);
	  L=++(*length)[0];
	  if (L%10==0)
	    *length=realloc(*length,(L+11)*2*sizeof(int));
	  if (*length==NULL) 
	    { fprintf(stderr,"Error in realloc in contour() for *length\n"); 
	      exit(1);}
	  (*length)[L*2]=index-index_old;
	  (*length)[L*2+1]=charact;
	}
      else set_exclude(0,i,horizontal,2);
    }
/* Now we look for contours in the bulk (Only need horizontal, since 
     there are no purely vertical contours left... ) */
  dir[0]=0; dir[1]=1;
  for (i=0;i<lattx-1;i++)
    for (j=0;j<latty-1;j++)
      {
	if (!excluded(i,j,vertical)&&
	    (((data[(*ind)(i,j)]>=cut)&&(data[(*ind)(i+1,j)]<cut))
	     ||((data[(*ind)(i,j)]<cut)&&(data[(*ind)(i+1,j)]>=cut)))) 
	  {
	    index_old=index;
	    startcontour(data,i,j,cut,cont,&index,dir,&charact,periodic,
			 boundary);
	    L=++(*length)[0];
	    if (L%10==0) *length=realloc(*length,(L+11)*2*sizeof(int));
	    if (*length==NULL) { fprintf(stderr,
	       "Error in realloc in contour() for *length\n"); exit(1);}
	    (*length)[L*2]=index-index_old;
	    (*length)[L*2+1]=charact;
	  }
	else set_exclude(i,j,vertical,2);
      }
  free(excl);
}

#undef horizontal
#undef vertical
