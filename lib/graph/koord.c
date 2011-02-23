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

#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"graph/koord.h"
#include<limits.h>

static void check(int a, int b){
  if (a<=b) return;
  printf("Error in koord c! Exiting...");
  exit(1);
}

static int checkl(double min, double max){
  if ((min<LONG_MIN)||(max>LONG_MAX))
    return 1;
  else
    return 0;
}


void Koordinatenachse(double xmin, double xmax,tic **t,char *expstr, int *anz,
		      int zwischentics)
{
 
  double dx,x,p,tmp;
  long int	ixmin;
  int 	j,no,expx,expcorr;

  if (xmin>xmax){
    tmp=xmax; 
    xmax=xmin; 
    xmin=tmp;
  }

  dx = xmax-xmin;
  expx = log10(fabs(dx));
  if (expx<-14) {
    xmax+=1; xmin-=1; expx=0;
    dx=xmax-xmin;
  }
  if (dx<1) expx--;
  xmin /= pow(10.,1.*expx);
  xmax /= pow(10.,1.*expx); 
  dx   /= pow(10.,1.*expx);

  if (abs(expx)<3) expcorr=expx; else expcorr=0;
  if ((expcorr== 0)&&(expx!=0))
    sprintf(expstr," 10^%d",expx);
  else
    sprintf(expstr," ");
  no=0;

  if (fabs(dx)>=4 )
    {
      if (zwischentics) *anz = (fabs(dx)+2)*2+2;
      else *anz = fabs(dx)+2;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      p = (x-xmin)/(xmax-xmin);
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr){
	case -2: sprintf((*t)[no].name,"%.2f",1.0*ixmin/100); break;
	case -1: sprintf((*t)[no].name,"%.1f",1.0*ixmin/10); break;
	case  0: sprintf((*t)[no].name,"%li",ixmin);    break;
	case  1: sprintf((*t)[no].name,"%li",ixmin*10); break;
	case  2: sprintf((*t)[no].name,"%li",ixmin*100); break;
	}
	no++;
	check(no,*anz);
      }
      p = (x+0.5-xmin)/(xmax-xmin);
      if (p>0) {
	(*t)[no].pos = p;
	(*t)[no].name = NULL;
	no++;
	check(no,*anz);
      }
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  (*t)[no].pos = (x-xmin)/(xmax-xmin);
	  if ((*t)[no].pos>0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.2f",1.0*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.1f",1.0*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%li",ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*10); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*100); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  if (zwischentics){
	    p=(x+0.5-xmin)/(xmax-xmin);
	    if ((p>=0)&&(p<=1)) 
	      { 
		(*t)[no].pos = p; 
		(*t)[no].name= NULL;
		no++;
		check(no,*anz);
	      }
	  }
	}
      *anz=no;
    }
  else if( fabs(dx)>=2 )
    {
      xmin *=2;
      xmax *=2;
      if (zwischentics) *anz = (fabs(dx)+1)*2*5;
      else *anz = (fabs(dx)+1)*2;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      p = (x-xmin)/(xmax-xmin);
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr)
	  {
	  case -2: sprintf((*t)[no].name,"%.3f",0.5*ixmin/100); break;
	  case -1: sprintf((*t)[no].name,"%.2f",0.5*ixmin/10); break;
	  case  0: sprintf((*t)[no].name,"%.1f",0.5*ixmin);    break;
	  case  1: sprintf((*t)[no].name,"%li",ixmin*5); break;
	  case  2: sprintf((*t)[no].name,"%li",ixmin*50); break;
	  }
      	no++;
	check(no,*anz);
      }
      for (j=1; j<=4;j++)
	{
	  p = (x+0.2*j-xmin)/(xmax-xmin);
	  if (p>0) {
	    (*t)[no].pos = p;
	    (*t)[no].name = NULL;
	    no++;
	    check(no,*anz);
	  }
	}
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  (*t)[no].pos = (x-xmin)/(xmax-xmin);
	  if ((*t)[no].pos>=0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.3f",0.5*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.2f",0.5*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%.1f",0.5*ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*5); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*50); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  for (j=1; j<=4;j++)
	    {
	      p = (x+0.2*j-xmin)/(xmax-xmin);
	      if ((p>=0)&&(p<=1)) {
		(*t)[no].pos = p;
		(*t)[no].name = NULL;
		no++;
		check(no,*anz);
	      }
	    }
	}
      *anz=no;
    }
  else
    {
      xmin *=5;
      xmax *=5;
      if (zwischentics) *anz = (fabs(dx)*5+1)*2;
      else *anz = fabs(dx)*5;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      p = (x-xmin)/(xmax-xmin);
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr)
	  {
	  case -2: sprintf((*t)[no].name,"%.3f",0.2*ixmin/100); break;
	  case -1: sprintf((*t)[no].name,"%.2f",0.2*ixmin/10); break;
	  case  0: sprintf((*t)[no].name,"%.1f",0.2*ixmin);    break;
	  case  1: sprintf((*t)[no].name,"%li",ixmin*2); break;
	  case  2: sprintf((*t)[no].name,"%li",ixmin*20); break;
	  }
	no++;
	check(no,*anz);
      }
      p = (x+0.5-xmin)/(xmax-xmin);
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = NULL;
	no++;
	check(no,*anz);
      }
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  (*t)[no].pos = (x-xmin)/(xmax-xmin);
	  if ((*t)[no].pos>=0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.3f",0.2*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.2f",0.2*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%.1f",0.2*ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*2); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*20); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  p = (x+0.5-xmin)/(xmax-xmin);
	  if ((p>=0)&&(p<=1)) {
	    (*t)[no].pos = p;
	    (*t)[no].name = NULL;
	    no++;
	    check(no,*anz);
	  }
	}
      *anz=no;
    }
  return;
}

void KoordinatenachseLog2(double xmin, double xmax,tic **t,char *expstr, 
			 int *anz, int zwischentics)
{
 
  double dx,x,p,tmp,dxmin;
  long int	ixmin;
  int 	no;

  sprintf(expstr," ");
  if (xmin>xmax){
    tmp=xmax; 
    xmax=xmin; 
    xmin=tmp;
  }

  dx = xmax-xmin;

  no=0;

    {
      if (zwischentics) *anz = (log10(dx/xmin)+2)*4+2;
      else *anz = log10(dx/xmin)+2;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      ixmin = log10(xmin)-1;
      dxmin = pow(10,ixmin);
      x = dxmin;
      p = (log(x)-log(xmin))/(log(xmax)-log(xmin));
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	sprintf((*t)[no].name,"%g",dxmin);
	no++;
	check(no,*anz);
      }
      /*      if (x+0.5>xmin)
	p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
      else p=-1;
      if (p>xmin) {
	(*t)[no].pos = p;
	(*t)[no].name = NULL;
	no++;
	check(no,*anz);
	}*/
      for (dxmin*=10,x=dxmin; x<=xmax; dxmin*=10,x=dxmin)
	{
	  if (x>xmin)
	    (*t)[no].pos =(log(x)-log(xmin))/(log(xmax)-log(xmin));
	  else (*t)[no].pos =-1;
       	  if ((*t)[no].pos>0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    sprintf((*t)[no].name,"%g",dxmin);
	    no++;
	    check(no,*anz);
	  }
	  if (zwischentics){
	    if (x+0.5>xmin)
	      p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
	    else p=-1;
	    if ((p>=0)&&(p<=1)) 
	      { 
		(*t)[no].pos = p; 
		(*t)[no].name= NULL;
		no++;
		check(no,*anz);
	      }
	  }
	}
      *anz=no;
    }
  return;
}



void KoordinatenachseLog(double xmin, double xmax,tic **t,char *expstr, 
			 int *anz, int zwischentics)
{
 
  double dx,x,p,tmp,scale;
  long int	ixmin;
  int 	j,no,expx,expcorr;

  if (xmin>xmax){
    tmp=xmax; 
    xmax=xmin; 
    xmin=tmp;
  }

  dx = xmax-xmin;
  scale = dx/xmin;
  if (scale>1000) KoordinatenachseLog2(xmin,xmax,t,expstr,anz,0);
  else{    
  expx = log10(fabs(dx));
  if (expx<-14) {
    xmax+=1; xmin-=1; expx=0;
    dx=xmax-xmin;
  }
  if (dx<1) expx--;
  xmin /= pow(10.,1.*expx);
  xmax /= pow(10.,1.*expx); 
  dx   /= pow(10.,1.*expx);

  if (abs(expx)<3) expcorr=expx; else expcorr=0;
  if ((expcorr== 0)&&(expx!=0))
    sprintf(expstr," 10^%d",expx);
  else
    sprintf(expstr,"");
  no=0;

  if (fabs(dx)>=4 )
    {
      if (zwischentics) *anz = (fabs(dx)+2)*4+2;
      else *anz = fabs(dx)+2;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      p = (x-xmin)/(xmax-xmin);
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr){
	case -2: sprintf((*t)[no].name,"%.2f",1.0*ixmin/100); break;
	case -1: sprintf((*t)[no].name,"%.1f",1.0*ixmin/10); break;
	case  0: sprintf((*t)[no].name,"%li",ixmin);    break;
	case  1: sprintf((*t)[no].name,"%li",ixmin*10); break;
	case  2: sprintf((*t)[no].name,"%li",ixmin*100); break;
	}
	no++;
	check(no,*anz);
      }
      if (x+0.5>xmin)
	p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
      else p=-1;
      if (p>xmin) {
	(*t)[no].pos = p;
	(*t)[no].name = NULL;
	no++;
	check(no,*anz);
      }
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  if (x>xmin)
	    (*t)[no].pos =(log(x)-log(xmin))/(log(xmax)-log(xmin));
	  else (*t)[no].pos =-1;
	  if ((*t)[no].pos>xmin){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.2f",1.0*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.1f",1.0*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%li",ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*10); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*100); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  if (zwischentics){
	    if (x+0.5>xmin)
	      p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
	    else p=-1;
	    if ((p>=0)&&(p<=1)) 
	      { 
		(*t)[no].pos = p; 
		(*t)[no].name= NULL;
		no++;
		check(no,*anz);
	      }
	  }
	}
      *anz=no;
    }
  else if( fabs(dx)>=2 )
    {
      xmin *=2;
      xmax *=2;
      if (zwischentics) *anz = (fabs(dx)+1)*2*5;
      else *anz = (fabs(dx)+1)*2;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      if (x>xmin)
	p = (log(x)-log(xmin))/(log(xmax)-log(xmin));
      else p = -1;
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr)
	  {
	  case -2: sprintf((*t)[no].name,"%.3f",0.5*ixmin/100); break;
	  case -1: sprintf((*t)[no].name,"%.2f",0.5*ixmin/10); break;
	  case  0: sprintf((*t)[no].name,"%.1f",0.5*ixmin);    break;
	  case  1: sprintf((*t)[no].name,"%li",ixmin*5); break;
	  case  2: sprintf((*t)[no].name,"%li",ixmin*50); break;
	  }
      	no++;
	check(no,*anz);
      }
      for (j=1; j<=4;j++)
	{
	  if (x+0.2*j>xmin)
	    p = (log(x+0.2*j)-log(xmin))/(log(xmax)-log(xmin));
	  else p=-1;
	  if (p>0) {
	    (*t)[no].pos = p;
	    (*t)[no].name = NULL;
	    no++;
	    check(no,*anz);
	  }
	}
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  if (x>xmin)
	    (*t)[no].pos =(log(x)-log(xmin))/(log(xmax)-log(xmin));
	  else (*t)[no].pos = -1;
	  if ((*t)[no].pos>=0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.3f",0.5*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.2f",0.5*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%.1f",0.5*ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*5); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*50); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  for (j=1; j<=4;j++)
	    {
	      if (x+0.2*j>xmin)
		p = (log(x+0.2*j)-log(xmin))/(log(xmax)-log(xmin));
	      else p=-1;
	      if ((p>=0)&&(p<=1)) {
		(*t)[no].pos = p;
		(*t)[no].name = NULL;
		no++;
		check(no,*anz);
	      }
	    }
	}
      *anz=no;
    }
  else
    {
      xmin *=5;
      xmax *=5;
      if (zwischentics) *anz = (fabs(dx)*5+1)*2;
      else *anz = fabs(dx)*5;
      *t =(tic*) malloc(sizeof(tic)* *anz);
      if (checkl(xmin-1,xmax)){*anz=0;return;}
      ixmin = xmin-1;
      x = ixmin;
      if (x>xmin)
	p = (log(x)-log(xmin))/(log(xmax)-log(xmin));
      else p=-1;
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = (char *) malloc(sizeof(char)*20);
	switch(expcorr)
	  {
	  case -2: sprintf((*t)[no].name,"%.3f",0.2*ixmin/100); break;
	  case -1: sprintf((*t)[no].name,"%.2f",0.2*ixmin/10); break;
	  case  0: sprintf((*t)[no].name,"%.1f",0.2*ixmin);    break;
	  case  1: sprintf((*t)[no].name,"%li",ixmin*2); break;
	  case  2: sprintf((*t)[no].name,"%li",ixmin*20); break;
	  }
	no++;
	check(no,*anz);
      }
      if (x+0.5>xmin)
	p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
      else p=-1;
      if (p>=0) {
	(*t)[no].pos = p;
	(*t)[no].name = NULL;
	no++;
	check(no,*anz);
      }
      for (x=++ixmin; x<=xmax; x=++ixmin)
	{
	  if (x>xmin)
	    (*t)[no].pos = (log(x)-log(xmin))/(log(xmax)-log(xmin));
	  else (*t)[no].pos=-1;
	  if ((*t)[no].pos>=0){
	    (*t)[no].name = (char *) malloc(sizeof(char) * 20);
	    switch(expcorr)
	      {
	      case -2: sprintf((*t)[no].name,"%.3f",0.2*ixmin/100); break;
	      case -1: sprintf((*t)[no].name,"%.2f",0.2*ixmin/10); break;
	      case  0: sprintf((*t)[no].name,"%.1f",0.2*ixmin);    break;
	      case  1: sprintf((*t)[no].name,"%li",ixmin*2); break;
	      case  2: sprintf((*t)[no].name,"%li",ixmin*20); break;
	      }
	    no++;
	    check(no,*anz);
	  }
	  if (x+0.5>xmin)
	    p = (log(x+0.5)-log(xmin))/(log(xmax)-log(xmin));
	  else p=-1;
	  if ((p>=0)&&(p<=1)) {
	    (*t)[no].pos = p;
	    (*t)[no].name = NULL;
	    no++;
	    check(no,*anz);
	  }
	}
      *anz=no;
    }
  }
  return;
}

  
  
  
  




