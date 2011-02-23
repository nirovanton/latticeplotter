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

# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include "newgraph/view3d.h"

# define STACKSIZE 100

static int
        handles[100],
        akthandle       = 0,
	xscreensize	= 0,
	yscreensize	= 0,
	xscreensizehalf	= 0,
	yscreensizehalf	= 0,
	focusdistance	= 0,	/*  Abstand Auge-Schirm in Pixeln  */
	matstackptr	= 0,
	gobjektptr	= -1,
	_newframe	= 1;
/*	maxgobject;
*/	
static Gobject
	*Gobjects[Gobjectnum];

	
static mat
	matstack[STACKSIZE],          /* Transformationen der Ortsvektoren */
	matstackinvscale[STACKSIZE],  /* Transformationen der Normalen */ 
	mateins         = {1,0,0,0,1,0,0,0,1},
	aktmat          = {1,0,0,0,1,0,0,0,1},
	aktmatinvscale  = {1,0,0,0,1,0,0,0,1};
	
static vec
	vecstack[STACKSIZE],
	vecnull         = {0,0,0},
	aktvec          = {0,0,0};	

int getview3dhandle(){handles[akthandle]=1;akthandle++;return akthandle-1;}
int newframe(int handle){
  int erg;
  erg=handles[handle];
  handles[handle]=0;
  return erg;
}

Gobject *initGobjects(int n)
/* Muss angepasst werden: Es wird derzeit kein Speicher alokiert fuer Daten */
{
  return malloc(sizeof(Gobject)*n);
}	

void resetGobjects(void)
{
	int i;
	for (i=0;i<=gobjektptr;i++)
	  {
	    /*	  if (Gobjects[i]->type==V3DMESH)
		  for (j=0;j<Gobjects[i]->van;j++)
		  Gobjects[i]->transfv[j]=0;*/
	    if (Gobjects[i]->type==TEXTOBJECT)
	      free(Gobjects[i]->polygons);
	  }    
	gobjektptr	= -1;
}
	

int insertGobject(Gobject *G)
{
	int r 			= 0;
/*	_newframe=0;*/
	if (G->type==V3DPOLYHEDRON){
		meq(G->mc2wcm,aktmat);
		meq(G->mc2wcmn,aktmatinvscale);
		veq(G->mc2wcv,aktvec);
	}
	if (++gobjektptr<Gobjectnum){ 
		Gobjects[gobjektptr]	= G;
		r		= 1;
	}
	return r;	
}

void setviewglobal (int xsize, int ysize, int f, int zshift)
{
        double scal;

	scal		= xsize/2;
	if (scal>ysize/2)
		scal            = ysize/2;
        xscreensize     = xsize;
        yscreensize     = ysize;
        xscreensizehalf = xsize/2;
        yscreensizehalf	= ysize/2;
	focusdistance	= f*scal;
	meq (aktmat, mateins);
	meq (aktmatinvscale, mateins);
	veq (aktvec, vecnull);
	translate(0.0, 0.0, 1.0 * zshift*scal);
	scale (scal, scal, scal);
}                

void setscreendimensions(int xsize, int ysize)
{
        xscreensize     = xsize;
        yscreensize     = ysize;
        xscreensizehalf = xsize/2;
	yscreensizehalf = ysize/2;
}        
                
void rotate(int axis, double angle)
{
	mat 
	  R	= {1,0,0,0,1,0,0,0,1};
	int 
	  i1, i2, i3, i4;
	double
	  co, si;  
	  
	if (axis == xaxis)
		{i1 = 4; i2 = 5; i3 = 7; i4 = 8;}
	else if (axis == yaxis)
		{i1 = 0; i2 = 2; i3 = 6; i4 = 8;}
	else if (axis == zaxis)
		{i1 = 0; i2 = 1; i3 = 3; i4 = 4;}
	if ((axis >= xaxis) && (axis <= zaxis)){
		angle  *= 3.1415927/180;
		co 	= cos(angle);
		si 	= sin(angle);
		R[i1] 	= co;
		R[i2]	= si;
		R[i3]	= -si;
		R[i4]	= co;
		meq (aktmat, matmatmul (aktmat, R));
		meq (aktmatinvscale, matmatmul (aktmatinvscale, R));
	}
}	

	
void translate(double x, double y, double z)
{
	vec v;
	v[0]	= x;
	v[1]	= y;
	v[2]	= z;
	veq (aktvec, vsum (aktvec, matvecmul(aktmat, v)));
}

	
void scale(double sx, double sy, double sz)
{
	mat
	  S	= {0,0,0,0,0,0,0,0,0};
	 
	S[0]	= sx;
	S[4]	= sy;
	S[8]	= sz;
	meq (aktmat, matmatmul (aktmat, S));
	S[0]	= 1/sx;
	S[4]	= 1/sy;
	S[8]	= 1/sz;
	meq (aktmatinvscale, matmatmul (aktmatinvscale, S));
}	  


void pushtrafs(void)
{
	meq(matstack[++matstackptr],aktmat);
	meq(matstackinvscale[matstackptr],aktmatinvscale);
	veq(vecstack[matstackptr],aktvec);
}

void poptrafs(void)
{
        meq(aktmat,matstack[matstackptr]);
        meq(aktmatinvscale,matstackinvscale[matstackptr]);
        veq(aktvec,vecstack[matstackptr--]);
}
                
void copytrafs(void)
{
        meq(aktmat,matstack[matstackptr]);
        meq(aktmatinvscale,matstackinvscale[matstackptr]);
        veq(aktvec,vecstack[matstackptr]);
}
                
                	
vecp mc2wc (vecp v)
{
	return vsum (matvecmul (aktmat, v), vsum (aktvec, v));
}

vecp mc2wcnorm (vecp v)
{
	return matvecmul (aktmatinvscale, v);
}

matp getmc2wcm(void){
  return aktmat;
}
  
vecp getmc2wcv(void){
  return aktvec;
}
  
matp getmc2wcmn(void) {
  return aktmatinvscale;
}  

double sizeinwc(double sizeinmc){
  vec
    v1={0.,0.,0.},
    v2={1.,0.,0.},	    
    v3={0.,1.,0.},
    v4={0.,0.,1.},v;
  veq(v,mc2wc(v1));
  v2[0]=sizeinmc;
  return sizeinmc/3*(vabs(vdiff(v,mc2wc(v2)))
		     +vabs(vdiff(v,mc2wc(v3)))
		     +vabs(vdiff(v,mc2wc(v4))));
}	  


int wc2dcfakt (short int *x, short int *y, double *f, vecp v)
{
/*	if (v[2] > focusdistance){*/
		*f	= focusdistance / v[2];
		*x	= v[0] * *f+xscreensizehalf;
		*y	= v[1] * *f+yscreensizehalf;	
		return 1;
/*	}
	else return 0;	*/
}


int wc2dc (short int *x, short int *y, vecp v)
{
	double
	  f;
/*	if (v[2] > focusdistance){*/
		f	= focusdistance / v[2];
		*x	= v[0] * f+xscreensizehalf;
		*y	= v[1] * f+yscreensizehalf;	
		return 1;
/*	}
	else return 0;	*/
}

void wc2dc_double (double *x, double *y, vecp v)
{
	double
	  f;
	f	= focusdistance / v[2];
	*x	= v[0] * f+1.0*xscreensizehalf;
	*y	= v[1] * f+1.0*yscreensizehalf;	
}



/**
 **	Hier erstmal ein Index - Heapsort
 **/


void indexx(int n, double arrin[], int indx[])
{
  int l,j,ir,indxt,i;
  float q;
  
  for (j=1;j<=n;j++) indx[j]=j;
  if (n == 1) return;
  l=(n >> 1) + 1;
  ir=n;
  for (;;) {
    if (l > 1)
      q=arrin[(indxt=indx[--l])];
    else {
      q=arrin[(indxt=indx[ir])];
      indx[ir]=indx[1];
      if (--ir == 1) {
	indx[1]=indxt;
	return;
      }
    }
    i=l;
    j=l << 1;
    while (j <= ir) {
      if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
      if (q < arrin[indx[j]]) {
	indx[i]=indx[j];
	j += (i=j);
      }
      else j=ir+1;
    }
    indx[i]=indxt;
  }
}




void DRAW(void)
{
  static int 
    initialized = 0,
    *ind;
  
  static double
    *dist;
  
  int i;
  
  if (!initialized) {
    printf("initialized:%i\n",initialized);
    if ((dist       = malloc(sizeof(double)*Gobjectnum))==NULL){
      printf("Error allocating memory for >dist< in view3d.DRAW() ");
      exit(0);
    }	
    if ((ind       = malloc(sizeof(ind)*Gobjectnum))==NULL){
      printf("Error allocating memory for >ind< in view3d.DRAW() ");
      exit(0);
    }	
    initialized	= 1;
  }	                                                                                                 
  ind--;		/* Da die einkommenden arrays von 0 - n-1 	*/
  dist--;		/* und nicht von 1 - n laufen			*/
  for (i=0; i<=gobjektptr; i++)
    dist[i+1]	= Gobjects[i] -> poswc[2];
  indexx (gobjektptr+1, dist, ind);
  for (i=gobjektptr+1; i>0; i--){
    /*printf("Gobject[%i] hat den Betrachterabstand %f, in der Struktur %f\n",
      ind[i]-1, dist[ind[i]], Gobjects[ind[i]-1] -> poswc[2]);  */
    Gobjects[ind[i]-1] -> drawstruct(Gobjects[ind[i]-1]);
  }			
  ind++;
  dist++;
  resetGobjects();
  for (i=0;i<akthandle;i++)handles[i]=1;
  _newframe=1;
}







