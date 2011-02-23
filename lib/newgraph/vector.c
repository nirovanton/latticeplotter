/***************************************************************************\
graph library
Copyright (C) 1992-2003  Johannes Schlosser and Alexander Wagner

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
#include "newgraph/vector.h"


/* typedef double *vecp; */

#define v_max 200
#define m_max 20

static vec vb[v_max];
static long vindex=0;
static mat mb[m_max];
static long mindex=0;
/* static long i; */

vecp vnext(void){
  vecp erg;
   if (vindex==v_max) vindex=0; erg=vb[vindex++];
   return erg;
} 

vecp vsum(vecp v1, vecp v2)
{
  vecp erg;
  register int i;
  if (vindex==v_max) vindex=0; erg=vb[vindex++];
  for (i=0;i<vecdim;i++)
    erg[i] = v1[i] + v2[i];
  return erg;
}

vecp vdiff(vecp v1, vecp v2)
{
  vecp erg;
  register int i;
  if (vindex==v_max) vindex=0; erg=vb[vindex++];
  for (i=0;i<vecdim;i++)
    erg[i] = v1[i] - v2[i];
  return erg;
}

double sp(vecp v1, vecp v2)
{
  register int i;
  register double sum=0;
  for (i=0;i<vecdim;i++)sum+=v1[i]*v2[i];
  return sum;
}

double vsqr(vecp v)
{
  register int i;
  register double sum=0;
  for (i=0;i<vecdim;i++)sum+=v[i]*v[i];
  return sum;
}

double vabs(vecp v)
{
  register int i;
  register double sum=0;
  for (i=0;i<vecdim;i++)sum+=v[i]*v[i];
  return sqrt(sum);
}


vecp vnorm(vecp v)
{
  vecp erg;
  register int i;
  register double sum=0;
  if (vindex==v_max) vindex=0; erg=vb[vindex++];
  for (i=0;i<vecdim;i++)sum+=v[i]*v[i];
  sum=1/sqrt(sum);
  for (i=0;i<vecdim;i++)erg[i]=v[i]*sum;
  return erg;
}




vecp vp(vecp v1, vecp v2)
{
  vecp erg;
  if (vindex==v_max) vindex=0; erg=vb[vindex++];
  erg[0]=v1[1]*v2[2]-v1[2]*v2[1];
  erg[1]=-v1[0]*v2[2]+v1[2]*v2[0];
  erg[2]=v1[0]*v2[1]-v1[1]*v2[0];
  return erg;
}


vecp sm(double s,vecp v)
{
  vecp erg;
  register int i;
  if (vindex==v_max) vindex=0; erg=vb[vindex++];
  for (i=0;i<vecdim;i++)
    erg[i]=s*v[i];
  return erg;
}

vecp vnew()
{
  return  (double *)malloc(sizeof(double)*vecdim);
}

void veq(vecp v1, vecp v2)
{
  memcpy(v1, v2, sizeof(double)*vecdim);
/*  vindex=0;*/
}

void vreset()
{
/*  vindex=0;*/
}

void pv(vecp v)
{printf("x: %f y: %f z: %f\n",v[0],v[1],v[2]);}

void pve(vecp v)
{printf("x: %e y: %e z: %e\n",v[0],v[1],v[2]);}


vecp matvecmul(matp m, vecp v)
{
	vecp erg;
    	register int i,j;
	if (vindex==v_max) vindex=0;
      	erg	= vb[vindex++];
        for (i=0;i<vecdim;i++){
        	erg[i]	= 0;
        	for (j=0;j<vecdim;j++)
        		erg[i] += *(m+j) * *(v+j);
        	m+=vecdim;	
        }		
        return erg;
}

matp matmatmul(matp m, matp m2)
{
	matp erg;
	register matp dum;
	register int i,j,k;
	if (mindex==m_max) mindex=0;
	erg	= mb[mindex++];
 	dum	= erg;
  	for (i=0;i<vecdim;i++){
  		for (j=0;j<vecdim;j++){
  			*(dum) = 0;
  			for (k=0;k<vecdim;k++)
  				*(dum) += *(m+k) * *(m2 + k*vecdim +j);
  			dum ++;		 
  		}
  		m += vecdim;
  	}		
  	return erg;
}

vecp mnew()
{
  return  (double *)malloc(sizeof(double)*vecdim*vecdim);
}

void meq(matp v1, matp v2)
{
  	memcpy(v1, v2, sizeof(double)*vecdim*vecdim);
/*    	mindex=0;*/
}

void mreset()
{
/*	mindex=0;*/
}
   
void pm(matp v)
{
	int i,j;
	
	for (i=0; i<vecdim; i++){
		printf("| ");
		for (j=0; j<vecdim; j++)
			printf("%f ",*(v++));
		printf("|\n");
	}		
}



/* End. */

