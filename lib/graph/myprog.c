/***************************************************************************\
graph library
Copyright (C) 1992-2009  Alexander Wagner

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

/* This is a somewhat excessive example program that makes use of about every 
   feature of the GUI. If you are looking for an example of how to implement
   something, this might be a good place to look.                            */

#include <math.h>
#include <unistd.h>
#include "graph/mygraph.h"
#include "graph/color.h"

#define onedim 200
#define onedim2 250
#define cdim 500
#define Xdim 30
#define Ydim 30

static int sinsinreq,sinexpreq,sinreq,cosreq,creq,vecreq,treq;
static int continuous=1,next=0,newdata=1;
static int sinsinxdim=Xdim,sinsinydim=Ydim,sindim=onedim,cosdim=onedim2;
static int vecxdim=Xdim,vecydim=Ydim,txdim=Xdim,tydim=Ydim;
static int mycdim=cdim;
static  double sinsindata[Xdim*Ydim],*sinsindatap;
static  double sinexpdata[Xdim*Ydim];
static  double vecdata[Xdim*Ydim*2];
static  double sindata[onedim],*sindatap,cosdata[onedim2*2],cdata[cdim*2];
static  double tdata[Xdim*Ydim*3];
static double Point[2]={0,0};
static int One=1;
static int Cont1req;
static int Contxdim=1,Contydim=1,Contzdim=1;
static Contour *Cont1;
static double mydouble=1.002;
static long xtime=0;
static int myint=0;
static char mystring[50]="This is some text";
static char OurComment[200];
static double darray[5]={0.1,0.2,0.3,0.4,0.5};

void myvec(int x, int y, double *res){
  res[0]=sin(1.0*(x*y+xtime)/100);
  res[1]=cos(1.0*(x*y+xtime)/120);
}

void mytensor(int x, int y, double *res){
  res[0]= sin(1.0*(x*y+xtime)/100);
  res[1]= sin(1.0*(x*y+1.01*xtime)/99);
  res[2]= -sin(1.0*(x*y+xtime)/100)/*cos(1.0*(x*y+xtime)/200)*/;
}
void mysin(int x, double *res){
  *res=sin(1.0*(x+xtime)/100);
}
void mycos(int x, double *res){
  *res=cos(1.0*(x+xtime)/100);
}
void myc(int x, double *res){
  res[0]=sin(1.0*(x+xtime)/20);
  res[1]=sin(mydouble*(x+xtime+myint)/30);
}
void sinsin(int x, int y, double *res){
  *res= sin(1.*x/10+1.0*xtime/1000)*sin(1.*y/10+1.0*xtime/1000);
}

void sinexp(int x, int y, double *res){
  *res= sin(1.*x/10-1.0*xtime/1000)*exp(-1.*y*y/100);
}

void sincos2(int x,double *res){
  res[0]= sin(1.*x/10);
  res[1]= cos(1.*x/10);
}

void DefineContour(){
  static int continitialized=0; 
  vec v;
  int ind[8],dummy;

  if (continitialized)
    destroycontour(Cont1);
  else continitialized=1;
  Cont1=initcontour(dummy);
  v[0]=0.25+0.2*sin(xtime/100.); v[1]=0.25+0.2*cos(xtime/100.); v[2]=0.25;
  ind[0]=putpointintocontour(Cont1,v);
  v[0]=-0.25; v[1]=0.25; v[2]=0.25;
  ind[1]=putpointintocontour(Cont1,v);
  v[0]=-0.25; v[1]=-0.25; v[2]=0.25;
  ind[2]=putpointintocontour(Cont1,v);
  v[0]=0.25; v[1]=-0.25; v[2]=0.25;
  ind[3]=putpointintocontour(Cont1,v);
  v[0]=0.25; v[1]=0.25; v[2]=-0.25;
  ind[4]=putpointintocontour(Cont1,v);
  v[0]=-0.25; v[1]=0.25; v[2]=-0.25;
  ind[5]=putpointintocontour(Cont1,v);
  v[0]=-0.25; v[1]=-0.25; v[2]=-0.25;
  ind[6]=putpointintocontour(Cont1,v);
  v[0]=0.25; v[1]=-0.25; v[2]=-0.25;
  ind[7]=putpointintocontour(Cont1,v);

  puttriangleincontour(Cont1,ind[0],ind[1],ind[4],rot());
  puttriangleincontour(Cont1,ind[1],ind[5],ind[4],gruen());
  /*  puttriangleincontour(Cont1,ind[1],ind[6],ind[5],rot());*/
  /*  puttriangleincontour(Cont1,ind[1],ind[2],ind[6],gruen());*/
  puttriangleincontour(Cont1,ind[2],ind[7],ind[6],rot());
  puttriangleincontour(Cont1,ind[2],ind[3],ind[7],gruen());
  puttriangleincontour(Cont1,ind[3],ind[4],ind[7],rot());
  /*  puttriangleincontour(Cont1,ind[3],ind[0],ind[4],gruen());*/
  puttriangleincontour(Cont1,ind[5],ind[6],ind[7],rot());
  puttriangleincontour(Cont1,ind[5],ind[7],ind[4],gruen());
  /*  puttriangleincontour(Cont1,ind[3],ind[0],ind[1],rot());*/
  puttriangleincontour(Cont1,ind[3],ind[1],ind[2],gruen());
}

void providedata() { 
  int x,y;
  
  sinsinxdim=Xdim;
  sinsinydim=Ydim;
  if (sinreq){
    for (x=0;x<onedim;x++){
      mysin(x,&sindata[x]);
    }
  }
  if (cosreq){
    for (x=0;x<onedim2;x++){
      cosdata[2*x]=x+xtime;
      mycos(x,&cosdata[2*x+1]);
    }
  }
  if (creq){
    for (x=0;x<cdim;x++){
      myc(x,&cdata[2*x]);
    }
  }
  if (sinsinreq){ 
    for (x=0;x<Xdim;x++)
      for (y=0;y<Ydim;y++)
	sinsin(x,y,&sinsindata[x*Ydim+y]);
    sinsinreq=0;
  }
  if (sinexpreq){ 
    for (x=0;x<Xdim;x++)
      for (y=0;y<Ydim;y++)
	sinexp(x,y,&sinexpdata[x*Ydim+y]);
    sinexpreq=0;
  }
  if (vecreq){ 
    for (x=0;x<Xdim;x++)
      for (y=0;y<Ydim;y++)
	myvec(x,y,&vecdata[(x*Ydim+y)*2]);
    vecreq=0;
  }
  if (treq){ 
    for (x=0;x<Xdim;x++)
      for (y=0;y<Ydim;y++)
	mytensor(x,y,&tdata[(x*Ydim+y)*3]);
    treq=0;
  }
  if (Cont1req){
    DefineContour();
  }
}

void NewColorRamp(double frac,double *red, double *green,double *blue){
  *red=frac;
  *green=0.1;
  *blue=1-frac;
}

void SetNewRamp(){
  SetColorRamp(&NewColorRamp);
}

void SetOldRamp(){
  SetColorRamp(NULL);
}

void SetMyComment(){
  SetComment(OurComment);
}
void SetSystemComment(){
  SetComment(NULL);
}

int main(int argc,char **argv){
  int	      i,done=0,repeat=1,dummy;

  long        testlong=1123;


  sindatap=&sindata[0];
  SetDefaultColor(1);
  SetDefaultShape(3);
  SetDefaultFill(1);
  SetDefaultSize(0.7);
  SetDefaultLineType(0);
  DefineGraphN_R("Sin[x+xtime]",&sindata[0],&sindim,&sinreq);
  SetDefaultLineType(1);
  DefineGraphN_RxRp("SinSinp",&sindatap,&sindim,&sinreq);
  SetDefaultShape(2);
  DefineGraphN_RxR("Cos[x+xtime]",&cosdata[0],&cosdim,&cosreq);
  SetDefaultShape(3);
  SetDefaultColor(3);
  SetDefaultLineType(0);
  DefineGraphN_RxR("Point",&Point[0],&One,NULL);
  DefineGraphN_RxR("(Sin,Sin)",&cdata[0],&mycdim,&creq);
  DefineGraphNxN_R("(Sin[x]*Sin[y])",&sinsindata[0],
                   &sinsinxdim,&sinsinydim,&sinsinreq);
  sinsindatap=&sinsindata;
  DefineGraphNxN_Rp("(Sin[x]*Sin[y])p",&sinsindatap,
                   &sinsinxdim,&sinsinydim,&sinsinreq);
  DefineGraphNxN_R("(Sin[x]*exp[-y^2])",&sinexpdata[0],
                   &sinsinxdim,&sinsinydim,&sinexpreq);
  DefineGraphNxN_RxR("(Sin[x],Cos[Y])",&vecdata[0],
                   &vecxdim,&vecydim,&vecreq);
  DefineGraphNxN_RxRxRt("(Sin[x],Sin[x],Cos[Y])",&tdata[0],
                   &txdim,&tydim,&treq);
  DefineGraphContour3d("(wuerfel)",&Cont1,
                   &Contxdim,&Contydim,&Contzdim,&Cont1req);

  StartMenu("Test menu",1);
    DefineBool("Continuous",&continuous);
    DefineLong("Xtime",&xtime);
    DefineBool("Next",&next);
    DefineInt("Repeat",&repeat);
    DefineLong("Longint",&testlong);
    DefineMod("Mod int (5)",&myint,5);
    DefineDouble("Double",&mydouble);
    DefineDoubleArr("Double_arr",darray,5);
    DefineString("Name",mystring,50);
    StartMenu("Change Colors",0);
    DefineFunction("Try my colorramp",&SetNewRamp);
    DefineFunction("Restore system colorramp",&SetOldRamp);
    EndMenu();
    StartMenu("Comment selection",0);
    DefineFunction("Set Our Comment",&SetMyComment);
    DefineFunction("Set System Comment",&SetSystemComment);
    EndMenu();
    DefineLong("Longint",&testlong);
    DefineGraph(curve2d_,"2-d Curve");
    DefineGraph(graph2d_,"2-d Graph");
    DefineGraph(contour2d_,"2-d Contour");
    DefineGraph(contour3d_,"3-d Contour");
    DefineBool("Done",&done);
  EndMenu();
  
  /*
    We would like to be able to call:

    StartGraph(curve2d_,"name1","name2",Scaling);
    StartGraph(curve2d_,"name1","name2",NoScaling,xmin,xmax,ymin,ymax);
  */

  /* This has to be done after the graphics has been initialized !
     It would be good if we could to this at any time and the call would 
     just wait for the graphics to be initialized if it is called to early.
  */
  SetColorRamp(GreenRedColRamp);

  /* The Event loop. */

  while ( done ==0 )
    {
      sprintf(OurComment,"Time = %li",xtime);
      Events(newdata);
      providedata(); newdata=0;
      DrawGraphs(dummy);
      if (continuous||next){
	for (i=0;(i<repeat);i++){
	  xtime++;
	  next=0;
	  newdata=1;
	}
      } else {
	sleep(1);
      }
    }

  CloseGraphics(dummy);
  return 0;
}

