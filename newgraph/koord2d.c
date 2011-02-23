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

#include <string.h>
#include <koord.h>
#include <koord2d.h>
#include <mydraw_2d.h>


void zeichneachsex(int c,tic *tics,int ntic,int beschrifte,int textsize,
		  char *label, int xmin, int xmax, int y,int dir)
{
  int i,len,posx,posy;
  double gr;
  
  len=xmax-xmin;
  mydrawline(c,xmin,y,xmax,y);
  for (i=0;i<ntic;i++)
    {
      posx=xmin+tics[i].pos*len;
      if (tics[i].name!=NULL) 
	{
	  gr=1*len/50;
	  if (beschrifte)
	    {
	      posy=y+3*dir*gr;
	      mytext(c,posx,posy,tics[i].name,1);
	    }
	}
      else gr=0.5*len/50;
      posy=y-dir*gr;
      mydrawline(c,posx,y,posx,posy);
  }
  if (beschrifte) mytext(c,xmin+len/2,y+dir*4*len/50+2*len/50,label,1);
}

void zeichneachsey(int c,tic *tics,int ntic,int beschrifte,int textsize,
		  char *label, int ymin, int ymax, int x,int dir)
{
  int i,len,posx,posy;
  double gr;
  
  len=ymax-ymin;
  mydrawline(c,x,ymin,x,ymax);
  for (i=0;i<ntic;i++)
    {
      posy=ymin+tics[i].pos*len;
      if (tics[i].name!=NULL) 
	{
	  gr=1*len/50;
	  if (beschrifte)
	    {
	      posx=x+dir*gr;
	      if (dir<0) mytext(c,posx,posy,tics[i].name,0);
	      else mytext(c,posx,posy,tics[i].name,2);
	    }
	}
      else gr=0.5*len/50;
      posx=x-dir*gr;
      mydrawline(c,x,posy,posx,posy);
  }
  if (beschrifte) mytext(c,x+dir*6*len/50,ymin+9*len/10,label,1);
}


void Koordinatensystem2D(int c,
			 double xmin, double xmax,int logx, 
			 double ymin, double ymax,int logy,
			 int *xofs, int *yofs, int *xsize,int *ysize,
			 char *xstr, char *ystr,char *xstru, char *ystru)
{
  static tic *tics[2];
  static char label[2][30];
  char strexp[2][15];
  static int ntic[2],
         initialisiert=0;

  int i,j,textsize,strl;
  
  if (initialisiert){
    for (i=0;i<2;i++){
      for (j=0;j<ntic[i];j++) 
	if (tics[i][j].name!=NULL) free(tics[i][j].name);
      free (tics[i]);
    }
  }
  initialisiert=0;
  
  if (!initialisiert){
    if (logx)
      KoordinatenachseLog(xmin,xmax,&(tics[0]),strexp[0],&(ntic[0]),1);
    else
      Koordinatenachse(xmin,xmax,&(tics[0]),strexp[0],&(ntic[0]),1);

    if (logy)
      KoordinatenachseLog(ymin,ymax,&(tics[1]),strexp[1],&(ntic[1]),1);
    else
      Koordinatenachse(ymin,ymax,&(tics[1]),strexp[1],&(ntic[1]),1);

    if (0==strcmp(xstru,"[arb.u.]")) strexp[0][0]='\0';
    if (0==strcmp(ystru,"[arb.u.]")) strexp[1][0]='\0';
    if ((0==strcmp(xstru,""))&&(0==strcmp(strexp[0],"")))
	sprintf(label[0],"%s",xstr);
    else
      sprintf(label[0],"%s / %s %s",xstr,strexp[0],xstru);
    if ((0==strcmp(ystru,""))&&(0==strcmp(strexp[1],"")))
	sprintf(label[1],"%s",ystr);
    else
      sprintf(label[1],"%s / %s %s",ystr,strexp[1],ystru);
    initialisiert=1;
  }
  mysetfontdirection(0);
  textsize=*xsize/50;
  myselectfont(0,"0",textsize);
  for (strl=0,i=0;i<ntic[0];i++) 
    if (NULL!=tics[0][i].name)
      if (strl<strlen(tics[0][i].name)) strl=strlen(tics[0][i].name);
  strl+=4;
  strl*=textsize;
  /*if (drawmask&labelyl)*/ *xofs = strl;/* else *xofs=0;*/
  /*if (drawmask&labelyr)*/ *xsize-=2**xofs;/* else *xsize-=*xofs;*/
  /*if (drawmask&labelxu)*/ *yofs = 4*textsize;/* else *yofs=0;*/
  /*if (drawmask&labelxd)*/ *ysize-=2**yofs;/* else *ysize-=*yofs;*/
  /*if (drawmask&drawxu)*/
    zeichneachsex(c,tics[0],ntic[0],/*drawmask&labelxu*/0,textsize,label[0],
		  *xofs,*xofs+*xsize,*yofs,-1);
  /*if (drawmask&drawxd)*/
    zeichneachsex(c,tics[0],ntic[0],/*drawmask&labelxd*/1,textsize,label[0],
		  *xofs,*xofs+*xsize,*yofs+*ysize,1);
  /*if (drawmask&drawyl)*/
    zeichneachsey(c,tics[1],ntic[1],/*drawmask&labelyl*/1,textsize,label[1],
		  *yofs+*ysize,*yofs,*xofs,1);
  /*if (drawmask&drawyr)*/
    zeichneachsey(c,tics[1],ntic[1],/*drawmask&labelyr*/0,textsize,label[1],
		  *yofs+*ysize,*yofs,*xofs+*xsize,-1);
}


void CoordLineY(int c,double min, double max,int log,
		int xofs, int yofs, int xsize,int ysize)
{
  static tic *tics;
  static int ntic,initialisiert=0;
  char strexp[15];
  int i,j,textsize,strl;
  
  strexp[0]='\0';
  if (initialisiert){
    for (j=0;j<ntic;j++)
      if (tics[j].name!=NULL) free(tics[j].name);
    free (tics);
  }
  initialisiert=0;
  
  if (!initialisiert){
    if (log)
      KoordinatenachseLog(min,max,&tics,strexp,&ntic,1);
    else
      Koordinatenachse(min,max,&tics,strexp,&ntic,1);

    initialisiert=1;
  }
  mysetfontdirection(0);
  textsize=xsize;
  myselectfont(0,"0.0014",textsize);
  for (strl=0,i=0;i<ntic;i++) 
    if (NULL!=tics[i].name)
      if (strl<strlen(tics[i].name)) strl=strlen(tics[i].name);
  strl+=4;
  strl*=textsize;
  zeichneachsey(c,tics,ntic,1,textsize,strexp,
		yofs+ysize,yofs,xofs,-1);
}













