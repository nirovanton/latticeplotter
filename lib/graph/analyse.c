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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph/mypow.h"
#include "graph/mygraph.h"
#include "graph/contour.h"
#include "graph/fftn.h"
#include "graph/analyse.h"

void savegrdata(grstr graph,char *name,int no,int rough)
{
  double *gp=NULL;
  FILE *gfile;
  int x,y,xmax,ymax;
  /*
  getgraph3d(graph,Scalar,&gp,&xmax,&ymax,no,rough,1,1);
  */
  gfile=fopen(name,"w");
  for (x=0; x<xmax; x++)
    {
      for (y=0; y<ymax; y++)
	fprintf(gfile,"%e  ",gp[x*ymax+y]);
      fprintf(gfile,"\n");
    }
  fclose(gfile);
  free(gp);
}

double calc_vol(grstr graph, int nogr, double cut)
{
  int i,x_size,y_size;
  double *g;
  register  double gt,vol=0,n;
  /*
  getgraph3d(graph,Scalar,&g,&x_size,&y_size,nogr,1,1,1);
  */
  n=0;
  for (i=0; i<x_size*y_size; i++)
    {
      gt=g[i];
      if (gt>cut) vol+=gt;
    }
  return vol;
}

int my_calc_center(double *g,double *center,int x_size,int y_size)
{
  int x,y;
  register  double n=0,gt;

  center[0]=center[1]=0;
  for (x=0; x<x_size; x++)
    for (y=0; y<y_size; y++)
      {
	gt=g[x*y_size+y];
	if (gt>0)
	  {
	    center[0]+=gt*x;
	    center[1]+=gt*y;
	    n+=gt;
	  }
      }
  if (n!=0)
    { 
      center[0]/=n;
      center[1]/=n;
      return 0;
    }
  else return 1;
}



int calc_ABtet(grstr graph,double *a, double *b, double *tet)
{
  int x,y,i,x_size,y_size;
  register  double n=0,gt;
  double center[2],T[2][2] = {{0.,0.},{0.,0.}},l[2],tmp,*g;
  /*
  getgraph3d(graph,Scalar,&g,&x_size,&y_size,0,1,1,1);
  */
  if (!my_calc_center(g,center,x_size,y_size))
    {
      for (x=0; x<x_size; x++)
	for (y=0; y<y_size; y++)
	  for (i=0; i<9; i++)
	    {
	      gt=g[x*y_size+y];
	      if (gt>0)
		{
		  T[0][0]+=gt*(x-center[0])*(x-center[0]);
		  T[0][1]+=gt*(x-center[0])*(y-center[1]);
		  T[1][0]+=gt*(x-center[0])*(y-center[1]);
		  T[1][1]+=gt*(y-center[1])*(y-center[1]);
		  n+=gt;
		}
	    }
      T[0][0]/=n;
      T[0][1]/=n;
      T[1][0]/=n;
      T[1][1]/=n;
#ifdef DEBUG
      printf("    / %e \\\n"
	     "c = |              |\n"
	     "    \\ %e /\n\n",center[0],center[1]);
      printf("    / %e  %e \\\n"
	     "T = |                            |\n"
	     "    \\ %e  %e /\n\n",T[0][0],T[0][1],T[1][0],T[1][1]);
#endif
      l[0]=(T[0][0]+T[1][1])/2
	-sqrt(pow2((T[0][0]-T[1][1])/2)
	      +T[0][1]*T[1][0]);
      l[1]=(T[0][0]+T[1][1])/2
	+sqrt(pow2((T[0][0]-T[1][1])/2)
	      +T[0][1]*T[1][0]);
      if ((l[0]-l[1])*(T[0][0]-T[1][1])<0) 
	{ tmp=l[0];l[0]=l[1];l[1]=tmp;}
#ifdef DEBUG
      double R[2][2];
      R[0][0]=-1/sqrt(pow2((T[1][1]-l[0]))+pow2(T[0][1]))*(T[1][1]-l[0]);
      R[0][1]=1/sqrt(pow2((T[1][1]-l[0]))+pow2(T[0][1]))*T[0][1];
      R[1][0]=-1/sqrt(pow2((T[0][0]-l[1]))+pow2(T[0][1]))*T[0][1];
      R[1][1]=1/sqrt(pow2((T[0][0]-l[1]))+pow2(T[0][1]))*(T[0][0]-l[1]);
      printf("    / %e  %e \\\n"
	     "R = |                            |\n"
	 "    \\ %e  %e /\n\n",R[0][0],R[0][1],R[1][0],R[1][1]);
#endif

      if (T[0][1]==0) *tet=0;
      else *tet=asin(T[0][1]/sqrt(pow2(T[1][1]-l[0])+pow2(T[0][1])));
      l[0]=sqrt(l[0]);
      l[1]=sqrt(l[1]);
      if (l[0]>l[1]) {*a=l[0];*b=l[1];}
      else {*a=l[1];*b=l[0];}
      return 1;
    }
  else 
    {  /* No Bubble */
      *a=0;*b=0;*tet=0;
      return 0;
    }
}

double calc_D(grstr graph,double *tet)
{
  double a,b;

  if (calc_ABtet(graph,&a,&b,tet)) return (a-b)/(a+b);
  else return 0;
}


int checkcontours(grstr graph,double cut)
{
  int l,*length,nox,noy,boundary=0;
  double *cont,*gp=NULL;

  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,0,1,1,1);  
  */
  contour(gp,nox,noy,
	  cut,&cont,&length,0,NULL,&boundary);
  l=length[0];
  free(cont);
  free(length);
  free(gp);
  return l;
}

void data_from_contour(double *cont, int anz,
		       double *a, double *b, double *tet)
{ 
  int i,ai,bi; 
  double d,center[2];

  /*
  getcenter(center);
  */
  ai=0;
  *a=*b=sqrt(pow2(cont[0]-center[0])+pow2(cont[1]-center[1]));
  for (i=0;i<anz;i++)
    {
      d=sqrt(pow2(cont[i*2]-center[0])+pow2(cont[i*2+1]-center[1]));
      if (*a<d) {ai=i; *a=d;}
      if (*b>d) {bi=i; *b=d;}
    }
  *tet=fmod(atan2(cont[ai*2+1]-center[1],cont[ai*2]-center[0]),90);
}

double pressure_difference(grstr graph)
{
 double center[2],*gp=NULL,res;
 int ci[2],nox,noy;
 /*
 getgraph3d(graph,Scalar,&gp,&nox,&noy,8,1,1,1);  
 getcenter(&(center[0]));
 */
 ci[0]=center[0];ci[1]=center[1];
 res=gp[ci[0]*noy+ci[1]]
    -gp[3*noy+3];
 free(gp);

 return res;
}

double get_min(grstr graph,int nograph)
{
 int i,nox,noy;
 double min,*gp=NULL;
 /*
 getgraph3d(graph,Scalar,&gp,&nox,&noy,nograph,1,1,1);  
 */
 min=gp[0];
 for (i=1;i<nox*noy;i++)
   if (min>gp[i]) min=gp[i];
 free(gp); 
 return min;
}

double get_max(grstr graph,int nograph)
{
 int i,nox,noy;
 double max,*gp=NULL;
 /*
 getgraph3d(graph,Scalar,&gp,&nox,&noy,nograph,1,1,1);  
 */
 max=gp[0];
 for (i=1;i<nox*noy;i++)
   if (max<gp[i]) max=gp[i];
 free(gp);
 return max;
}

void get_minmax(double *gp,int anz,double *min, double *max)
{
 int i;
 
 *min=*max=gp[0];
 for (i=1;i<anz;i++){
   if (*max<gp[i]) *max=gp[i];
   if (*min>gp[i]) *min=gp[i];
 }
}

double get_vecmax(grstr graph,int nograph)
{
 int i,nox,noy;
 double max,*gp=NULL;
 register double t;
 /*
 getgraph3d(graph,Vector,&gp,&nox,&noy,nograph,1,1,1);  
 */
 max=0;
 for (i=0;i<2*nox*noy;i+=2){
   t=pow2(gp[i])+pow2(gp[i+1]);
   if (max<t) max=t;
 }
 free(gp);
 return sqrt(max);
}

double get_vecav(grstr graph,int nograph)
{
 int i,nox,noy;
 double av,*gp=NULL;
 /*
 getgraph3d(graph,Vector,&gp,&nox,&noy,nograph,1,1,1);  
 */
 av=0;
 for (i=0;i<2*nox*noy;i+=2){
   av+=sqrt(pow2(gp[i])+pow2(gp[i+1]));
 }
 free(gp);
 return av/nox/noy;
}

double get_av(grstr graph,int nograph)
{
 int i,nox,noy;
 double sum,*gp=NULL;
 /*
 getgraph3d(graph,Scalar,&gp,&nox,&noy,nograph,1,1,1);  
 */
 sum=0;
 for (i=0;i<nox*noy;i++)
   sum+=gp[i];
 free(gp);
 return sum/nox/noy;
}

double get_av2(grstr graph,int nograph)
{
 int i,nox,noy;
 double sum,*gp=NULL;
 /*
 getgraph3d(graph,Scalar,&gp,&nox,&noy,nograph,1,1,1);
 */
 sum=0;
 for (i=0;i<nox*noy;i++)
   sum+=pow2(gp[i]);
 free(gp);
 return sum/nox/noy;
}

double get_areafrac(grstr graph,int nograph,double cut)
{
  long sum;
  int i,nox,noy;
  double *gp=NULL;
  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,nograph,1,1,1);  
  */
  sum=0;
  for (i=0;i<nox*noy;i++)
    if (gp[i]>cut) sum++;
  free(gp);
  return 1.*sum/nox/noy;
}


/* Data analysis part. Calculates different k-modes.  */

double S0(double data[],int nn[])
{
  int i;
  double Ssum=0;

  for  (i=0; i<nn[0]*nn[1];i++)
    Ssum+=data[i]; 
  return Ssum;
}

double S1(double data[],int nn[])
{
  int x,y;
  double Ssum=0;
 
  for  (x=0; x<nn[0]; x++)
    for (y=0; y<nn[1]; y++)
      Ssum+= sqrt(pow2(x-nn[0]/2)+pow2(y-nn[1]/2))*data[x*nn[1]+y]; 
  return Ssum;
}


void Sk(double data[],int nn[],double kav,double vec[2])
{
  int x,y;
  double kk;

  vec[0]=vec[1]=0;
  for  (x=0; x<nn[0]; x++)
    for (y=0; y<nn[1]; y++)
      {
	kk=sqrt(pow2(x-nn[0]/2)+pow2(y-nn[1]/2));
	if (kk!=0){
	  vec[0]+=(1-kav/kk)*fabs(x-nn[0]/2) *data[x*nn[1]+y];
	  vec[1]+=(1-kav/kk)*fabs(y-nn[1]/2) *data[x*nn[1]+y];
	}
      }
}

void Sk2(double data[],int nn[],double kav,double mat[3])
{
  int x,y;
  double kk;

  mat[0]=mat[1]=mat[2]=mat[3]=0;
  for  (x=0; x<nn[0]; x++)
    for (y=0; y<nn[1]; y++)
      {
	kk=sqrt(pow2(x-nn[0]/2)+pow2(y-nn[1]/2));
	if (kk!=0){
	  mat[0]+=pow2((1-kav/kk)   *(x-nn[0]/2)      )*data[x*nn[1]+y];
	  mat[1]+=pow2((1-kav/kk))*(x-nn[0]/2)*(y-nn[1]/2)   *data[x*nn[1]+y];
	  mat[2]+=pow2((1-kav/kk)               *(y-nn[1]/2))*data[x*nn[1]+y];
	}
      }
}

void Sd2(double data[],int nn[],double mat[3])
{
  int x,y;

  mat[0]=mat[1]=mat[2]=0;


  for  (x=0;x<nn[0]; x++)
    for (y=0;y<nn[1]; y++){
      mat[0]+=pow2(2./nn[0]*(x-nn[0]/2))*data[x*nn[1]+y];
      mat[1]+=    2./nn[0]*(x-nn[0]/2)*
	          2./nn[1]*(y-nn[1]/2)   *data[x*nn[1]+y];
      mat[2]+=pow2(2./nn[1]*(y-nn[1]/2))*data[x*nn[1]+y];
    }
}
void Sd2sin(double data[],int nn[],double mat[3])
{
  int x,y;

  mat[0]=mat[1]=mat[2]=0;


  for  (x=0;x<nn[0]; x++)
    for (y=0;y<nn[1]; y++){
/*
      mat[0]+=pow2(sin(2*M_PI/nn[0]*x))*data[x*nn[1]+y];
      mat[1]+=    sin(2*M_PI/nn[0]*x)*
	          sin(2*M_PI/nn[1]*y)   *data[x*nn[1]+y];
      mat[2]+=pow2(sin(2*M_PI/nn[1]*y))*data[x*nn[1]+y];
*/
      mat[0]+=pow2(sin(2*M_PI/nn[0]*(x-nn[0]/2)))*data[x*nn[1]+y];
      mat[1]+=    sin(2*M_PI/nn[0]*(x-nn[0]/2))*
	          sin(2*M_PI/nn[1]*(y-nn[1]/2))   *data[x*nn[1]+y];
      mat[2]+=pow2(sin(2*M_PI/nn[1]*(y-nn[1]/2)))*data[x*nn[1]+y];

    }
}

double Sd0(double data[],int nn[])
{
  int i;
  double sum=0;

  for  (i=0;i<nn[0]*nn[1]; i++) sum+=data[i];
  return sum;
}




int drop_data(grstr graph,double cut,double *a,double *b, double *tet)
{
  int l,*length,nox,noy,boundary=0;
  double *cont,*gp=NULL;
  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,0,1,1,1);  
  */
  contour(gp,nox,noy,
	  cut,&cont,&length,0,NULL,&boundary);
  l=length[0];

  free(gp);
  if (l>0)  
    {
      data_from_contour(cont,length[1],a,b,tet);
      free(cont);
      free(length);
      return 1;
    }
  else 
    {
      free(cont);
      free(length);
      return 0;
    }
}

void get_fftdata(grstr graph,int nogr,double *d0, double *kav, 
		 double d1[2],double d2[3])
{ 
  int x,y,nn[2],m,ct,tvar,nox,noy;
  double *field,*gp=NULL;
  double average;
  
  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,nogr,1,1,1);
  */
  nn[0]=nox;
  nn[1]=noy;

/* Checking that size=2^n ... need to tell that something is wrong...*/
  tvar=nn[0];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_fftdata failed because xdim=%i is not a power of two\n",
	   nn[0]);
    return;
  }
  tvar=nn[1];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_fftdata failed because ydim=%i is not a power of two\n",
	   nn[1]);
    return;
  }

  field=malloc(nn[0]*nn[1]*2*sizeof(double));
  average=0;
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      average+=gp[x*noy+y];
  average/=nn[0]*nn[1];
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      field[2*(x*nn[1]+y)]=gp[x*noy+y]-average;
      field[2*(x*nn[1]+y)+1]=0;
    }
  fourn(field-1,nn-1,2,1);
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      gp[x*noy+y]=
/*	pow2(field[2*(x*nn[1]+y)])
	  +pow2(field[2*(x*nn[1]+y)+1]);
*/
	pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
			  ((y+nn[1]/2)%nn[1]))])
	    +pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
			  ((y+nn[1]/2)%nn[1]))+1]);
  free(field);
  *d0= S0(gp,nn);
  *kav=S1(gp,nn)/ *d0;
  Sk(gp,nn,*kav,d1);
  Sd2(gp,nn,d2);
  free(gp);
}

void get_Struct_data(grstr graph,int nogr,double *S_0, double *S_1, 
		     double *S_2, double *S_3, int *Slen, int *size)
{ 
  int x,y,nn[2],nnm,m,ct,tvar,nox,noy,i;
  int *N;
  double *field,*S,*gp=NULL;
  double average;
  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,nogr,1,1,1);
  */
  nn[0]=nox;
  nn[1]=noy;
  
  
  /* Checking that size=2^n ... need to tell that something is wrong...*/
  tvar=nn[0];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because xdim=%i is not a power of two\n",
	   nn[0]);
    return;
  }
  tvar=nn[1];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because ydim=%i is not a power of two\n",
	   nn[1]);
    return;
  }
  if (nn[0]>nn[1]) nnm=nn[1]; else nnm=nn[0];
  *size=nnm;

  field= (double *) malloc(nn[0]*nn[1]*2*sizeof(double));
  average=0;
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      average+=gp[x*noy+y];
  average/=nn[0]*nn[1];
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      field[2*(x*nn[1]+y)]=gp[x*noy+y]-average;
      field[2*(x*nn[1]+y)+1]=0;
    }
  fourn(field-1,nn-1,2,1);
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      gp[x*noy+y]=
	/*	pow2(field[2*(x*nn[1]+y)])
		+pow2(field[2*(x*nn[1]+y)+1]);
		*/
	pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))])
       +pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))+1]);
  free(field);
  *Slen=nnm/2+1;
  S    = (double *) malloc(*Slen*sizeof(double));
  N    = (int    *) malloc(*Slen*sizeof(int ));
  for (i=0;i<*Slen;i++) {S[i]=0;N[i]=0;}
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      i=ceil(sqrt(pow2((x-nn[0]/2)*nnm/nn[0])+
		   pow2((y-nn[1]/2)*nnm/nn[1]))-0.5);
      if (i<*Slen){
	S[i]+=gp[x*noy+y];
	N[i]++;
      }
    }
  free(gp);
  *S_0=*S_1=*S_2=*S_3=0;
  for (i=0;i<*Slen;i++){
    if (N[i]>0) S[i]/=N[i];
    *S_0+=S[i];
    *S_1+=i*S[i];
    *S_2+=i*i*S[i];
    *S_3+=i*i*i*S[i];
  }
}

double* get_structfact(grstr graph,int nogr,int *Slen)
{ 
  int x,y,nn[2],nnm,m,ct,tvar,nox,noy,i;
  int *N;
  double *field,*S,*gp=NULL;
  double average;
  
  /*
  getgraph3d(graph,Scalar,&gp,&nox,&noy,nogr,1,1,1);
  */
  nn[0]=nox;
  nn[1]=noy;
  
  
  /* Checking that size=2^n ... need to tell that something is wrong...*/
  tvar=nn[0];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because xdim=%i is not a power of two\n",
	   nn[0]);
    return NULL;
  }
  tvar=nn[1];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because ydim=%i is not a power of two\n",
	   nn[1]);
    return NULL;
  }
  if (nn[0]>nn[1]) nnm=nn[1]; else nnm=nn[0];
  
  field= (double *) malloc(nn[0]*nn[1]*2*sizeof(double));
  average=0;
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      average+=gp[x*noy+y];
  average/=nn[0]*nn[1];
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      field[2*(x*nn[1]+y)]=gp[x*noy+y]-average;
      field[2*(x*nn[1]+y)+1]=0;
    }
  fourn(field-1,nn-1,2,1);
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      gp[x*noy+y]=
	/*	pow2(field[2*(x*nn[1]+y)])
		+pow2(field[2*(x*nn[1]+y)+1]);
		*/
	pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))])
       +pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))+1]);
  free(field);
  *Slen=nnm/2+1;
  S    = (double *) malloc(*Slen*sizeof(double));
  N    = (int    *) malloc(*Slen*sizeof(int ));
  for (i=0;i<*Slen;i++) {S[i]=0;N[i]=0;}
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      i=ceil(sqrt(pow2((x-nn[0]/2)*nnm/nn[0])+
		   pow2((y-nn[1]/2)*nnm/nn[1]))-0.5);
      if (i<*Slen){
	S[i]+=gp[x*noy+y];
	N[i]++;
      }
    }
  free(gp);
  for (i=0;i<*Slen;i++){
    if (N[i]>0) S[i]/=N[i];
  }
  free(N);
  return S;
}

double* GetStructFactFromData(double *gp,int nox,int noy,int *Slen)
{
  int x,y,nn[2],nnm,m,ct,tvar,i;
  int *N;
  double *field,*S;
  double average;
  
  nn[0]=nox;
  nn[1]=noy;
  
  
  /* Checking that size=2^n ... need to tell that something is wrong...*/
  tvar=nn[0];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because xdim=%i is not a power of two\n",
	   nn[0]);
    return NULL;
  }
  tvar=nn[1];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) {
    printf("get_Struct_data failed because ydim=%i is not a power of two\n",
	   nn[1]);
    return NULL;
  }
  if (nn[0]>nn[1]) nnm=nn[1]; else nnm=nn[0];
  
  field= (double *) malloc(nn[0]*nn[1]*2*sizeof(double));
  average=0;
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      average+=gp[x*noy+y];
  average/=nn[0]*nn[1];
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      field[2*(x*nn[1]+y)]=gp[x*noy+y]-average;
      field[2*(x*nn[1]+y)+1]=0;
    }
  fourn(field-1,nn-1,2,1);
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      gp[x*noy+y]=
	/*	pow2(field[2*(x*nn[1]+y)])
		+pow2(field[2*(x*nn[1]+y)+1]);
		*/
	pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))])
       +pow2(field[2*(((x+nn[0]/2)%nn[0])*nn[1]+
		     ((y+nn[1]/2)%nn[1]))+1]);
  free(field);
  *Slen=nnm/2+1;
  S    = (double *) malloc(*Slen*sizeof(double));
  N    = (int    *) malloc(*Slen*sizeof(int ));
  for (i=0;i<*Slen;i++) {S[i]=0;N[i]=0;}
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      i=ceil(sqrt(pow2((x-nn[0]/2)*nnm/nn[0])+
		   pow2((y-nn[1]/2)*nnm/nn[1]))-0.5);
      if (i<*Slen){
	S[i]+=gp[x*noy+y];
	N[i]++;
      }
    }
  for (i=0;i<*Slen;i++){
    if (N[i]>0) S[i]/=N[i];
  }
  free(N);
  return S;
}

#ifdef notdef
void get_correldata(int nogr,double *d0,double d2[4])
{ 
  int i,x,y,nn[2],m,ct,tvar;
  double *field,gp=NULL;
  double average;
  
  nn[0]=get_xsize();
  nn[1]=get_ysize();

/* Checking that size=2^n */
  tvar=nn[0];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) return;
  tvar=nn[1];
  for (m=1,ct=0;m<16;ct+=tvar&1,tvar>>=1,m++);
  if (ct!=1) return;

  for (i=1;i<graph.anz; i++) graph.getgraph[i]=0;
  graph.getgraph[nogr]=1;
  graph.gp=malloc(graph.anz*sizeof(double *));
  graph.gp[nogr]=malloc(nn[0]*nn[1]*sizeof(double));
  /*
  getgraph3d(graph,1,1,1,1);
  */
  field=malloc(nn[0]*nn[1]*4*sizeof(double));
  average=0;
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      average+=graph.gp[nogr][x*graph.noy+y];
  average/=nn[0]*nn[1];
  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++){
      field[2*(x*nn[1]*2+y)]=field[2*(x*nn[1]*2+2*nn[1]-1-y)]=
	graph.gp[nogr][x*graph.noy+y]-average;
      field[2*(x*nn[1]*2+y)+1]=field[2*(x*nn[1]*2+2*nn[1]-1-y)+1]=0;
    }
  nn[1]*=2;
  fourn(field-1,nn-1,2,1);
  for (x=0;x<nn[0]*nn[1];x++) {
    field[2*x]=sqrt(pow2(field[2*x])+pow2(field[2*x+1]));
    field[2*x+1]=0;
  }
  fourn(field-1,nn-1,2,-1);
  nn[1]/=2;

  for (x=0;x<nn[0];x++)
    for (y=0;y<nn[1]; y++)
      graph.gp[nogr][x*graph.noy+y]=
	field[2*(((x+nn[0]/2)%nn[0])*nn[1]*2+
		 ((y*2+nn[1])%(nn[1]*2)))]+
	field[2*(((x+nn[0]/2)%nn[0])*nn[1]*2+
		 ((y*2+1+nn[1])%(nn[1]*2)))];
  free(field);
  
  *d0= S0(graph.gp[0],nn);
  Sd2(graph.gp[0],nn,d2);
  d2[0]/=*d0;d2[1]/=*d0;d2[2]/=*d0;d2[3]/=*d0;
  free(graph.getgraph);
  free(graph.gp[nogr]);
  free(graph.gp);
}
#endif

/* Mathematically equivalent to the fourier data with the 
   sin(2 Pi k/L) term but it does not 
   require a power of two size of the lattice with periodic 
   boundary conditions.
*/

void get_derivdata(grstr graph, int nogr,double d2[3])
{ 
  int i,nox,noy;
  double *gp=NULL;
  /*
  getgraph3d(graph,Vector,&gp,&nox,&noy,nogr,1,1,1);
  */
  d2[0]=d2[1]=d2[2]=0;
  for (i=0;i<2*nox*noy;i+=2){
    d2[0]+=pow2(gp[i]);
    d2[1]+=gp[i]*gp[i+1];
    d2[2]+=pow2(gp[i+1]);
  }
  d2[0]/=nox*noy;
  d2[1]/=nox*noy;
  d2[2]/=nox*noy;
  free(gp);
}




/*  This was an attempt to construct data from a correlation function...
int addmat(double data[],int nn[],int xg,int y,double mat[4],int dy,int *no)
{
  int x,xmin,xmax;
  
  if (!(data[xg*nn[1]+y]>0)) return xg+1;
  for  (x=xg; (data[x*nn[1]+y]>0)&&(x<nn[0]); x++){
    *no+=1;
    mat[0]+=pow2((x-nn[0]/2)            )*data[x*nn[1]+y];
    mat[1]+=    (x-nn[0]/2)*(y-nn[1]/2)   *data[x*nn[1]+y];
    mat[3]+=pow2(            (y-nn[1]/2))*data[x*nn[1]+y];
  }
  xmax=x;
  if (data[xg*nn[1]+y]>0)
    for  (x=xg-1;(data[x*nn[1]+y]>0)&&(x>=0); x--){
      *no+=1;
      mat[0]+=pow2((x-nn[0]/2)            )*data[x*nn[1]+y];
      mat[1]+=    (x-nn[0]/2)*(y-nn[1]/2)   *data[x*nn[1]+y];
      mat[3]+=pow2(            (y-nn[1]/2))*data[x*nn[1]+y];
    }
  xmin=x+1;

  if ((y+dy>=0)&&(y+dy<nn[1]))
    for  (x=xmin;(x=addmat(data,nn,x,y+dy,mat,dy,no))<xmax;);
  return xmax;
}

void Sd2(double data[],int nn[],double mat[3])
{
  int x,y,xmin,xmax,no=0;

  mat[0]=mat[1]=mat[2]=0;


  y=nn[1]/2;
  for  (x=nn[0]/2;(data[x*nn[1]+y]>0)&&(x<nn[0]); x++){
    no++;
    mat[0]+=pow2((x-nn[0]/2)            )*data[x*nn[1]+y];
    mat[1]+=    (x-nn[0]/2)*(y-nn[1]/2)   *data[x*nn[1]+y];
    mat[2]+=pow2(            (y-nn[1]/2))*data[x*nn[1]+y];
  }
  xmax=x;
  for  (x=nn[0]/2-1;(data[x*nn[1]+y]>0)&&(x>=0); x--){
    no++;
    mat[0]+=pow2((x-nn[0]/2)            )*data[x*nn[1]+y];
    mat[1]+=    (x-nn[0]/2)*(y-nn[1]/2)   *data[x*nn[1]+y];
    mat[2]+=pow2(            (y-nn[1]/2))*data[x*nn[1]+y];
  }
  xmin=x+1;

  for  (x=xmin;(x=addmat(data,nn,x,y+1,mat, 1,&no))<xmax;);
  for  (x=xmin;(x=addmat(data,nn,x,y-1,mat,-1,&no))<xmax;);
}

int add(double data[],int nn[],int xg,int y,double *sum,int dy,int *no)
{
  int x,xmin,xmax;

  if (!(data[xg*nn[1]+y]>0)) return xg+1;
  for  (x=xg; (data[x*nn[1]+y]>0)&&(x<nn[0]); x++){
    no+=1;
    *sum+=data[x*nn[1]+y];
  }
  xmax=x;
  if (data[xg*nn[1]+y]>0)
    for  (x=xg-1;(data[x*nn[1]+y]>0)&&(x>=0); x--){
      *no+=1;
      *sum+=data[x*nn[1]+y];
    }
  xmin=x+1;

  if ((y+dy>=0)&&(y+dy<nn[1]))
    for  (x=xmin;(x=add(data,nn,x,y+dy,sum,dy,no))<xmax;);
  return xmax;
}

double Sd0(double data[],int nn[])
{
  int x,y,xmin,xmax,no=0;

  double sum=0;


  y=nn[1]/2;
  for  (x=nn[0]/2;(data[x*nn[1]+y]>0)&&(x<nn[0]); x++){
    no++;
    sum+=data[x*nn[1]+y];
  }
  xmax=x;
  for  (x=nn[0]/2-1;(data[x*nn[1]+y]>0)&&(x>=0); x--){
    no++;
    sum+=data[x*nn[1]+y];
  }
  xmin=x+1;

  for  (x=xmin;(x=add(data,nn,x,y+1,&sum, 1,&no))<xmax;);
  for  (x=xmin;(x=add(data,nn,x,y-1,&sum,-1,&no))<xmax;);
  return sum;
}
*/

void contourdata(double *cont,int *length,int *N,int *no1,int *no2,double *len)
{
  int i,j;
  double *contp;

  *N=length[0];
  *no1=0;*no2=0;*len=0;
  if (length[0]>0)
    {
      contp=cont;
      for (j=0;j<length[0];j++)
	{
	  for (i=1,contp+=2; i<length[j*2+2]; i++,contp+=2)
	    *len+=sqrt(pow2(contp[0]-contp[-2])+
		       pow2(contp[1]-contp[-1]));
	  if (length[3+j*2]==4) (*no1)++; 
	  else if (length[3+j*2]==-4) (*no2)++;
	}
    }
}

void dircontourdata(double *cont,int *length,int *N,int *no1,int *no2,
		    double *len, double *len1, double *len2,double *tet)
{
  int i,j;
  double *contp,dl,dtet,nx=0,ny=0,dx,dy,ln;

  *N=length[0];
  *no1=0;*no2=0;*len=0;*len1=0;*len2=0;*tet=0;

  if (length[0]>0)
    {
      contp=cont;
      for (j=0;j<length[0];j++)
	{
	  for (i=1,contp+=2; i<length[j*2+2]; i++,contp+=2){
	    dx=contp[0]-contp[-2];
	    dy=contp[1]-contp[-1];
	    dl=sqrt(pow2(dx)+pow2(dy));
	    *len+=dl;
	    dtet=atan2(dy,dx);
	    /*dtet=acos(dx/dl);*/
	    nx+=dl*cos(2*dtet);
	    ny+=dl*sin(2*dtet);
	  }
	  if (length[3+j*2]==4) (*no1)++; 
	  else if (length[3+j*2]==-4) (*no2)++;
	}
      ln=sqrt(pow2(nx)+pow2(ny));
      *tet=atan2(ny,nx)/2;
      /* *tet=acos(nx/ln)/2;*/
      *len1=*len-ln;
      *len2=*len+ln;
    }
}

static int localxsize,localysize;

long perind(int x,int y){
  x%=localxsize;
  if (x<0) x+=localxsize;
  y%=localysize;
  if (y<0) y+=localysize; 
  return x*localysize+y;
}


void GetContourData(double *data, int nox, int noy, double cut,
		   int *N, int *no1,int *no2, 
		   double *len, double *len1, double *len2, double *tet,
		   double *lenA, double *lenB)
{
  int i,j;
  double *contp,dl,dtet,nx=0,ny=0,dx,dy,ln;
  int *length,boundary=0;
  double *cont,*mylen,dummy;

  localxsize=nox;
  localysize=noy;

  contour(data,nox,noy,
	  cut,&cont,&length,1,&perind,&boundary);

  *N=length[0];
  *no1=0;*no2=0;*len=0;*len1=0;*len2=0;*lenA=0;*lenB=0;*tet=0;dummy=0;

  if (length[0]>0)
    {
      contp=cont;
      for (j=0;j<length[0];j++)
	{
	  if (length[3+j*2]==4) {(*no1)++;mylen=lenA;} 
	  else if (length[3+j*2]==-4) {(*no2)++;mylen=lenB;}
          else mylen=&dummy;
	  for (i=1,contp+=2; i<length[j*2+2]; i++,contp+=2){
	    dx=contp[0]-contp[-2];
	    dy=contp[1]-contp[-1];
	    dl=sqrt(pow2(dx)+pow2(dy));
	    *len+=dl;
            *mylen+=dl;
	    dtet=atan2(dy,dx);
	    /*dtet=acos(dx/dl);*/
	    nx+=dl*cos(2*dtet);
	    ny+=dl*sin(2*dtet);
	  }
	}
      ln=sqrt(pow2(nx)+pow2(ny));
      *tet=atan2(ny,nx)/2;
      /* *tet=acos(nx/ln)/2;*/
      *len1=*len-ln;
      *len2=*len+ln;
    }



  free(cont);
  free(length);
}
