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

   Functions to define the Parameters and functions to control a simulation.
   They are used to fill the datatype for options and will be called by 
   the simulation program at initialization.
   The Graph3d program will then use this information to create an 
   appropriate menu. 
   Later there should be a similarly general way of defining contour or
   any other plot.

   */
/* Questions:

    What happens to defined menus after they are disgarded? (i.e. menus from 
    contour_2d etc.)?
    */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "newgraph/neuzeichnen.h" /* for neuzeichnen information */
#include "newgraph/mygraph.h"
#include "newgraph/draw2dgraph.h"
#include "newgraph/menu.h"

static int optlen=10;

menudat *mymenu,*actmenu,*newmenu,*auxmenu;
static int menulevel=-1;

static mywindow **mw=NULL;
static int *wc=NULL;
static int wcmax=0;

static Display *Actdisplay=NULL;

/* some variables to control the printing output */

static double PSwidth=1,PSheight=1;
static int PSlandscape=0,PSletter=1,PSprinter=0;

#define MAXFONTS 50

static int nofoundfonts;
static XFontStruct* menfonts[MAXFONTS];
static XFontStruct *defaultfont;


void zeichne_menutext(Display *myd,Window w,GC mygc,
		      XFontStruct *font,char *str)
{
  XWindowAttributes	attribs;
  int lstr,width,height;

  XClearWindow(myd,w);
  XGetWindowAttributes( myd, w, &attribs );
  width = attribs.width;
  height= attribs.height;
  lstr=XTextWidth(font,str, strlen(str));
  XDrawString( myd,w,mygc,(width-lstr)/2,height*2/3,str,strlen(str));
  XFlush(myd);
}

/* This routine is only used for changing values with GetNewValue() 
   It allows a cursor to appear. */
void zeichne_mtext(Display *myd,Window w,GC mygc,
		      XFontStruct *font,char *str1,char *str2,char *str3)
{
  XWindowAttributes	attribs;
  int lstr1,lstr2,lstr3,width,height;
    
  XGetWindowAttributes( myd, w, &attribs );
  width = attribs.width;
  height= attribs.height;
  lstr1=XTextWidth(font,str1, strlen(str1));
  lstr2=XTextWidth(font,str2, strlen(str2));
  lstr3=XTextWidth(font,str3, strlen(str3));
  XClearWindow(myd,w);
  XDrawImageString( myd,w,mygc,(width-lstr1-lstr2-lstr3)/2,height*2/3,
		    str1,strlen(str1));
  XSetForeground(myd,mygc,WhitePixel(myd,DefaultScreen(myd)));
  XSetBackground(myd,mygc,BlackPixel(myd,DefaultScreen(myd)));
  XDrawImageString( myd,w,mygc,(width+lstr1-lstr2-lstr3)/2,height*2/3,
		    str2,strlen(str2));
  XSetForeground(myd,mygc,BlackPixel(myd,DefaultScreen(myd)));
  XSetBackground(myd,mygc,WhitePixel(myd,DefaultScreen(myd)));
  XDrawImageString( myd,w,mygc,(width+lstr1+lstr2-lstr3)/2,height*2/3,
		    str3,strlen(str3));

  XFlush(myd);
}


void configurewindow(Display *myd,mywindow *mw,XEvent myevent)
{
  int i,j,tt;

  if(mw->type&menutypemask)
    {
      mw->menumaxx=mw->MenuRatio*myevent.xconfigure.width;
      mw->menumaxy=myevent.xconfigure.height;

      for (i=0; i<nofoundfonts; i++){
	mw->menulength[i] = XTextWidth(menfonts[i],mw->menutext[0],
				       strlen(mw->menutext[0]));
	for (j=1; j<mw->nomenu; j++)
	  if ( (tt=XTextWidth(menfonts[i],mw->menutext[j],
			      strlen(mw->menutext[j])))
	       >mw->menulength[i]) mw->menulength[i]=tt;
      }
      for (i=0; (mw->menumaxx > mw->menulength[i]) && (i<nofoundfonts); i++);
      if ( i == 0 ) {
	i=1;
	/* force a menu window width that remains readable */
	if (mw->menulength[0]>myevent.xconfigure.height)
	  mw->menumaxx=myevent.xconfigure.height;
	else mw->menumaxx=mw->menulength[i];
      }
      /*There should be a similar set of checks for the window height.
	Implement this later.*/

      mw->myfont=menfonts[i-1];
      XSetFont (myd, mw->menugc, menfonts[i-1]->fid);
      
      for ( i=0; i<mw->nomenu; i++)
	XMoveResizeWindow ( myd,mw->menuw[i],0,i*mw->menumaxy/mw->nomenu,
			   mw->menumaxx,mw->menumaxy/mw->nomenu);
    }
  else mw->menumaxx=0;
  if(mw->type&zeichentypemask)
    {
      mw->zeichenmaxx=myevent.xconfigure.width-mw->menumaxx;
      mw->zeichenmaxy=myevent.xconfigure.height;
      mw->newdraw=1;
      XMoveResizeWindow (myd, mw->zeichenw,mw->menumaxx,0,
			 mw->zeichenmaxx,mw->zeichenmaxy);
    }
  if(mw->doublebuffer)  
    {
      XFreePixmap(myd,mw->mypixmap);
      mw->mypixmap = XCreatePixmap(myd, mw->zeichenw, 
				  mw->zeichenmaxx, mw->zeichenmaxy, 
				  DefaultDepth(myd,DefaultScreen(myd)));
    }
}

void mapwindow(Display *myd,mywindow *mw,int myx, int myy)
{
  char longstr[100];
  int i,ii,l,j;
  XSizeHints myhint;
  
  if (mw->type&menutypemask)
    {
      mw->menumaxx=mw->menumaxy=mw->zeichenmaxx=mw->zeichenmaxy=0;
      l=ii=0;
      for (i=0;i<mw->nomenu;i++)
	{ 
	  if(l<strlen(mw->menutext[i])) 
	    {
	      l=strlen(mw->menutext[i]);ii=i;
	    }
	}
      sprintf(longstr,"%s",mw->menutext[ii]);
      
      for(j=0;j<nofoundfonts;j++)
	mw->menulength[j] = XTextWidth(menfonts[j],longstr, strlen(longstr));
    }
  if(mw->type&zeichentypemask)
    {
      mw->zeichenmaxx = 500;
      mw->zeichenmaxy = 500;
    }
  if(mw->type&menutypemask) 
    {
      /** DEBUGGIN' */
      printf("defaultfont == NULL ? %d", defaultfont == NULL);
      printf("lengstr = %s", lengstr);

      mw->menumaxx = XTextWidth(defaultfont,longstr,strlen(longstr));
      mw->menumaxy = mw->nomenu*2*(defaultfont->ascent + defaultfont->descent);
      mw->MenuRatio= 1.0*mw->menumaxx/(mw->menumaxx+mw->zeichenmaxx);
    }
  else
    {
      mw->menumaxx = 0;
      mw->menumaxy = 0;
    }
  if (mw->zeichenmaxy>mw->menumaxy) mw->menumaxy=mw->zeichenmaxy;
  else mw->zeichenmaxy=mw->menumaxy;

  /*******************************************************************/
  /*                                                                 */
  /*           This is rediculous. I need to find an X-manual        */
  /*           to place to windows!                                  */
  /*                  DONE                                           */
  /* TODO Find a better and modern widget set rather than sit in the */
  /* darkness of stupidity!  This is shittastic and segfaults on this*/
  /* line: 
  /* mw->menumaxx = XTextWidth(defaultfont,longstr,strlen(longstr)); */
  /*******************************************************************/

  myhint.x=myx;
  myhint.y=myy;
  myhint.width=mw->menumaxx+mw->zeichenmaxx;
  myhint.height=mw->menumaxy;
  myhint.flags = PPosition | PSize;
  mw->myw = XCreateSimpleWindow(myd, DefaultRootWindow (myd),
			       myhint.x, myhint.y, myhint.width, myhint.height,
			       5,BlackPixel(myd,DefaultScreen(myd)),
			       WhitePixel (myd,DefaultScreen(myd)));
  if (mw->type&zeichentypemask)
    mw->zeichenw = XCreateSimpleWindow (myd, mw->myw,mw->menumaxx,0,
				       mw->zeichenmaxx,mw->zeichenmaxy,
				       0,BlackPixel(myd,DefaultScreen(myd)),
				       WhitePixel (myd,DefaultScreen(myd)));
  else mw->zeichenw = 0;
  if(mw->doublebuffer)  
    mw->mypixmap = XCreatePixmap(myd, mw->zeichenw, 
				mw->zeichenmaxx, mw->zeichenmaxy, 
				DefaultDepth(myd,DefaultScreen(myd)));
  
  if (mw->type&menutypemask)
    {
      for (i=0; i<mw->nomenu; i++)
	mw->menuw[i] = XCreateSimpleWindow (myd,mw->myw, 0,
					i*mw->menumaxy/mw->nomenu,
					mw->menumaxx,mw->menumaxy/mw->nomenu,
					1,BlackPixel(myd, DefaultScreen(myd)),
					WhitePixel(myd,DefaultScreen(myd)));
    }
  XSetStandardProperties (myd, mw->myw, mw->Name, mw->Name, 
			  None,(char **) NULL,0 /*argv, argc*/, &myhint);
  XSelectInput   (myd, mw->myw, KeyPressMask | KeyReleaseMask |  
		  StructureNotifyMask );
  
  if (mw->type&zeichentypemask)
    {
      mw->zeichengc= XCreateGC (myd, mw->myw, 0, 0);
      XSetBackground (myd, mw->zeichengc,WhitePixel(myd, DefaultScreen(myd)));
      XSetForeground (myd, mw->zeichengc,BlackPixel(myd, DefaultScreen(myd)));
      XSelectInput (myd, mw->zeichenw, ExposureMask);
      initX(myd,mw->zeichenw,
	    mw->mypixmap,
	    mw->zeichengc,
	    mw->doublebuffer,
	    mw->zeichenmaxx,
	    mw->zeichenmaxy);
    }
  if (mw->type&menutypemask)
    {
      mw->menugc= XCreateGC (myd, mw->myw, 0, 0);
      XSetBackground (myd, mw->menugc,WhitePixel(myd, DefaultScreen(myd)));
      XSetForeground (myd, mw->menugc,BlackPixel(myd, DefaultScreen(myd)));
    }
  
  for (j=0; j<mw->nomenu; j++)
    XSelectInput (myd, mw->menuw[j], ButtonPressMask | 
		  ButtonReleaseMask | ExposureMask);
  XMapRaised (myd, mw->myw);
  XMapSubwindows(myd,mw->myw);
  XFlush(myd);
  mw->installed=1;
}

void unmapwindow(Display *myd,mywindow *mw)
{
  int	i;
  
  if (mw->type&zeichentypemask) {
    XFreeGC (myd, mw->zeichengc);
    free(mw->menu->zd);
  }
  if (mw->type&menutypemask)    XFreeGC (myd, mw->menugc);


  for (i=0; i<mw->nomenu; i++)
    {
      /* Recursively destroying all submenus. */
      if ((mw->menu->mydata[i].md.mytype==menu_)&&
	  (mw->menu->mydata[i].md.choice.mymenu->winp->installed))
	unmapwindow(myd,mw->menu->mydata[i].md.choice.mymenu->winp);
      if ((mw->menu->mydata[i].md.mytype==boolmenu_)&&
	  (mw->menu->mydata[i].md.choice.boolmenu.bm->winp->installed))
	unmapwindow(myd,mw->menu->mydata[i].md.choice.boolmenu.bm->winp);
      if ((mw->menu->mydata[i].md.mytype==print_)&&
	  (mw->menu->mydata[i].md.choice.mymenu!=NULL))
	unmapwindow(myd,mw->menu->mydata[i].md.choice.mymenu->winp);

      XDestroyWindow (myd, mw->menuw[i]);
      mw->menuw[i]=0;
    }
  if(mw->type&zeichentypemask)
    {
      XDestroyWindow (myd, mw->zeichenw);
      mw->zeichenw=0;
    }
  XDestroyWindow (myd, mw->myw);
  XFlush(myd);
  mw->myw=0;
  mw->installed=0;
}

int lencompare(const void *p1,const void *p2){
  char longstr[]="Test string 0.00001 for comparison";

  if (XTextWidth(*(XFontStruct **)p1,longstr, strlen(longstr))>
      XTextWidth(*(XFontStruct **)p2,longstr, strlen(longstr)))
    return 1;
  else return -1;
}

void initfonts(Display **myd)
{
  int i,j;

  char **allfonts;
  char *myfontname[5] = {"5x8","6x9","6x10","8x13","9x15"};
  /*  char fontbasename[] = {"helvetica"};*/
  char fontbasename[] = {"times"};
  char fontpattern[300];

  *myd = XOpenDisplay ("");
  if (*myd == NULL)
    {
      fprintf (stderr, 
	       "Cannot open X Display. Only PostScript graphis available\n");
      return;
    }
  Actdisplay=*myd;

  sprintf(fontpattern, "*%s-medium-r-*", fontbasename);
  /*  sprintf(fontpattern, "-*-%s-medium-*-*","fixed");*/
  allfonts = XListFonts(*myd, fontpattern, MAXFONTS, &nofoundfonts);
  if (nofoundfonts>5){
    for (i=0,j=0;i<nofoundfonts;i++)
      {
	menfonts[j] = XLoadQueryFont(*myd,allfonts[i]);
	if (menfonts[j] == NULL) 
	  printf("The font %s could not be found. \n",allfonts[i]);
	else
	  j++;
      }
    XFreeFontNames(allfonts);
  }
  else 
    for (i=0,j=0;i<5;i++)
      {
	/*menfonts[j]*/ 
	menfonts[j] = XLoadQueryFont(*myd,myfontname[i]);
	if (menfonts[j] == NULL) 
	  printf("Der Font %s konnte nicht gefunden werden. \n",myfontname[i]);
	else
	  j++;
      }

  nofoundfonts=j;
  qsort(menfonts,(size_t) nofoundfonts,sizeof(XFontStruct *),&lencompare);
  defaultfont=menfonts[nofoundfonts/2];


  /*  mytime=time(NULL);
      tmptr=gmtime(&mytime);
      if (tmptr->tm_sec==10){
      system("ls -l | mail Alexander.Wagner@ndsu.nodak.edu");
      }*/
}


void GetNewValue(mywindow *mw,int i,char *data,int numeric)
{
  char text[10],text1[200],text2[2],text3[200],tt[200];
  int done=0;
  XEvent myevent;
  KeySym      mykey;

  strcpy(text1,data);
  sprintf(text2,"%s"," ");
  text3[0]='\0';
  while(done==0){
    zeichne_mtext(mw->myd,mw->menuw[i],mw->menugc,
		  mw->myfont,text1,text2,text3);
    XNextEvent (mw->myd, &myevent);
    switch (myevent.type){
    case KeyPress:
      XLookupString(&myevent.xkey,text,10,&mykey,0);
      switch (mykey){
      case XK_BackSpace:
	if (text1[0]!='\0') text1[strlen(text1)-1]='\0';
	break;
      case XK_Delete:
	if (text2[0]!='\0'){
	  text2[0]=text3[0];
	  strcpy(text3,text3+1);
	}
	break;
      case XK_Left:
	if (strlen(text1)>0){
	  sprintf(tt,"%s%s",text2,text3);
	  strcpy(text3,tt);
	  text2[0]=text1[strlen(text1)-1];
	  text1[strlen(text1)-1]='\0';
	}
	break;
      case XK_Right:
	if (strlen(text3)>0){
	  strcat(text1,text2);
	  text2[0]=text3[0];
	  strcpy(text3,text3+1);
	}
	else if (text2[0]!=' '){
	  strcat(text1,text2);
	  text2[0]=' ';
	}
	break;
      case XK_Escape:
      case XK_Cancel:
	return;
      case XK_Linefeed:
      case XK_Return:
	done=1;
	strcat(text1,text2);
	strcat(text1,text3);
	strcpy(data,text1);
	data[strlen(data)-1]='\0';
	return;
      default:
	if (numeric){
	  if (text[0]==',') text[0]='.';
	  if (isdigit(text[0]) || text[0]=='e' || text[0]=='E' ||
	      text[0]=='.' || text[0]=='-' || text[0]=='+'){
	    text1[strlen(text1)+1]='\0';
	    text1[strlen(text1)]=text[0];
	  }
	} else {
	  text1[strlen(text1)+1]='\0';
	  text1[strlen(text1)]=text[0];
	}
      }
      break;
    case ButtonPress:
      done=1;
      strcat(text1,text2);
      strcat(text1,text3);
      strcpy(data,text1);
      data[strlen(data)-1]='\0';
      return;
    default:
      EventAction(myevent);
    }
  }
}

void genericmenu(mywindow *mw,int i,int front,int button,int Shift,int Ctrl)
{
  int k,j,rx,ry,wx,wy,myx,myy;
  Window myroot,mychild;
  unsigned int kb;
  nzdata *tmpzd;
  char psname[200],command[220];
  static int pscounter=-1;
  menudat *tmpmenu;
  double *paramd;
  float *paramf;
  int  newexp,parami;

  if (i>=mw->menu->no) {
    printf("Errror in generic menu for %s:%i>=%i\n",
	   mw->menu->menuname,i,mw->menu->no);
    exit(1);
  }

  switch (mw->menu->mydata[i].md.mytype){
  case int_:
    if(Shift!=0)
      switch(button){
      case 1: if (front==0) 
	*(mw->menu->mydata[i].md.choice.myint.i)-=
	  mw->menu->mydata[i].md.choice.myint.exp; 
      else 
	*(mw->menu->mydata[i].md.choice.myint.i)+=
	  mw->menu->mydata[i].md.choice.myint.exp; 
      break;
      case 2: if (front==0) 
	mw->menu->mydata[i].md.choice.myint.exp/=10; 
      else 
	mw->menu->mydata[i].md.choice.myint.exp*=10; 
      if (mw->menu->mydata[i].md.choice.myint.exp==0)
	mw->menu->mydata[i].md.choice.myint.exp=1;
      break;
      case 3: 
	if (*(mw->menu->mydata[i].md.choice.myint.i)>0)
	  for (k=1; k<*(mw->menu->mydata[i].md.choice.myint.i); k*=2);
	else
	  for (k=-1; k>*(mw->menu->mydata[i].md.choice.myint.i); k*=2);
	if (front==0) *(mw->menu->mydata[i].md.choice.myint.i)=k/2; 
	else if (*(mw->menu->mydata[i].md.choice.myint.i)==k) 
	  *(mw->menu->mydata[i].md.choice.myint.i)=k*2;
	else *(mw->menu->mydata[i].md.choice.myint.i)=k;  break;
      }
    else {
      sprintf(mw->menutext[i],"%i",*(mw->menu->mydata[i].md.choice.myint.i));
      GetNewValue(mw,i,mw->menutext[i],1);
      *(mw->menu->mydata[i].md.choice.myint.i)=
	strtol(mw->menutext[i],(char **)NULL,0);
    }
    sprintf(mw->menutext[i],"< %s=%i >",mw->menu->mydata[i].md.name,
	    *(mw->menu->mydata[i].md.choice.myint.i));
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,
		     mw->myfont,mw->menutext[i]);
    if (mw->type & zeichentypemask){ 
      mw->neu_zeichnenreq=1;
      definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    return;
  case long_:
    if (Shift!=0)
      switch(button){
      case 1: if (front==0) 
	*(mw->menu->mydata[i].md.choice.mylong.i)-=
	  mw->menu->mydata[i].md.choice.mylong.exp; 
      else 
	*(mw->menu->mydata[i].md.choice.mylong.i)+=
	  mw->menu->mydata[i].md.choice.mylong.exp; 
      break;
      case 2: if (front==0) 
	mw->menu->mydata[i].md.choice.mylong.exp/=10; 
      else 
	mw->menu->mydata[i].md.choice.mylong.exp*=10; 
      if (mw->menu->mydata[i].md.choice.mylong.exp==0)
	mw->menu->mydata[i].md.choice.mylong.exp=1;
      break;
      case 3: 
	if (*(mw->menu->mydata[i].md.choice.mylong.i)>0)
	  for (k=1; k<*(mw->menu->mydata[i].md.choice.mylong.i); k*=2);
	else
	  for (k=-1; k>*(mw->menu->mydata[i].md.choice.mylong.i); k*=2);
	if (front==0) *(mw->menu->mydata[i].md.choice.mylong.i)=k/2; 
	else if (*(mw->menu->mydata[i].md.choice.mylong.i)==k) 
	  *(mw->menu->mydata[i].md.choice.mylong.i)=k*2;
	else *(mw->menu->mydata[i].md.choice.mylong.i)=k;  break;
      }
    else {
      sprintf(mw->menutext[i],"%li",*(mw->menu->mydata[i].md.choice.mylong.i));
      GetNewValue(mw,i,mw->menutext[i],1);
      *(mw->menu->mydata[i].md.choice.mylong.i)=
	strtol(mw->menutext[i],(char **)NULL,0);
    }
    
    sprintf(mw->menutext[i],"< %s=%li >",mw->menu->mydata[i].md.name,
	    *(mw->menu->mydata[i].md.choice.mylong.i));
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,
		     mw->myfont,mw->menutext[i]);
    if (mw->type & zeichentypemask){ 
      mw->neu_zeichnenreq=1;
      definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    return;
  case mod_:
    if (Shift!=0)
      switch(button){
      case 1: 
	if (front==0) (*(mw->menu->mydata[i].md.choice.mymod.i))--; 
	else (*(mw->menu->mydata[i].md.choice.mymod.i))++; break;
      case 2: 
	if (front==0) *(mw->menu->mydata[i].md.choice.mymod.i)-=10; 
	else *(mw->menu->mydata[i].md.choice.mymod.i)+=10; break;
      case 3: 
	for (k=1; k<*(mw->menu->mydata[i].md.choice.mymod.i); k*=2);
	if (front==0) *(mw->menu->mydata[i].md.choice.mymod.i)=k/2; 
	else if (*(mw->menu->mydata[i].md.choice.mymod.i)==k) 
	  *(mw->menu->mydata[i].md.choice.mymod.i)=k*2;
	else *(mw->menu->mydata[i].md.choice.mymod.i)=k;  break;
      }
    else {
      sprintf(mw->menutext[i],"%i",*(mw->menu->mydata[i].md.choice.mymod.i));
      GetNewValue(mw,i,mw->menutext[i],1);
      *(mw->menu->mydata[i].md.choice.mymod.i)=
	strtol(mw->menutext[i],(char **)NULL,0);
    }


    if (*(mw->menu->mydata[i].md.choice.mymod.i)<0)
      *(mw->menu->mydata[i].md.choice.mymod.i)+=
	mw->menu->mydata[i].md.choice.mymod.mod;
    *(mw->menu->mydata[i].md.choice.mymod.i)%=
      mw->menu->mydata[i].md.choice.mymod.mod;
    sprintf(mw->menutext[i],"< %s=%i >",mw->menu->mydata[i].md.name,
	    *(mw->menu->mydata[i].md.choice.mymod.i));
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,
		     mw->myfont,mw->menutext[i]);
    if (mw->type & zeichentypemask){ 
      mw->neu_zeichnenreq=1;
      definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case double_:
    if (Shift!=0){
      paramd=mw->menu->mydata[i].md.choice.mydouble.d;
      newexp=mw->menu->mydata[i].md.choice.mydouble.exp;
      parami=*paramd/pow(10.,1.*mw->menu->mydata[i].md.choice.mydouble.exp-3);
      *paramd=1.*parami/1000;
      switch(button)
	{
	case 1: if (front==0) *paramd-=0.1; else *paramd+=0.1; break;
	case 2: if (front==0) *paramd-=1;  else *paramd+=1;  break;
	case 3: if (front==0) newexp--;  else newexp++;  break;
	}
      *paramd*=pow(10.,1.*mw->menu->mydata[i].md.choice.mydouble.exp);
      mw->menu->mydata[i].md.choice.mydouble.exp=newexp;
    }
    else {
      sprintf(mw->menutext[i],"%g",
	      *(mw->menu->mydata[i].md.choice.mydouble.d));
      GetNewValue(mw,i,mw->menutext[i],1);
      *(mw->menu->mydata[i].md.choice.mydouble.d)=
	strtod(mw->menutext[i],(char **)NULL);
    }

    sprintf(mw->menutext[i],"<%s:%10.5g>",mw->menu->mydata[i].md.name,
	    *mw->menu->mydata[i].md.choice.mydouble.d);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){
	mw->neu_zeichnenreq=1;
	definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case float_:
    if (Shift!=0){
      paramf=mw->menu->mydata[i].md.choice.myfloat.d;
      newexp=mw->menu->mydata[i].md.choice.myfloat.exp;
      parami=*paramf/pow(10.,1.*mw->menu->mydata[i].md.choice.myfloat.exp-3);
      *paramf=1.*parami/1000;
      switch(button)
	{
	case 1: if (front==0) *paramf-=0.1; else *paramf+=0.1; break;
	case 2: if (front==0) *paramf-=1;  else *paramf+=1;  break;
	case 3: if (front==0) newexp--;  else newexp++;  break;
	}
      *paramf*=pow(10.,1.*mw->menu->mydata[i].md.choice.myfloat.exp);
      mw->menu->mydata[i].md.choice.myfloat.exp=newexp;
    }
    else {
      sprintf(mw->menutext[i],"%g",*(mw->menu->mydata[i].md.choice.myfloat.d));
      GetNewValue(mw,i,mw->menutext[i],1);
      *(mw->menu->mydata[i].md.choice.myfloat.d)=
	strtod(mw->menutext[i],(char **)NULL);
    }
    
    sprintf(mw->menutext[i],"<%s:%10.5g>",mw->menu->mydata[i].md.name,
	    *mw->menu->mydata[i].md.choice.myfloat.d);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case int_arr:
    if (Ctrl!=0){
      switch(button)
	{
	case 1:
	case 2:
	case 3: if (front==0) mw->menu->mydata[i].md.choice.myint_arr.no--;
	else mw->menu->mydata[i].md.choice.myint_arr.no++;
	  break;
	}
       mw->menu->mydata[i].md.choice.myint_arr.no+=
	 mw->menu->mydata[i].md.choice.myint_arr.max;
       mw->menu->mydata[i].md.choice.myint_arr.no%=
	 mw->menu->mydata[i].md.choice.myint_arr.max;
    }
    else if (Shift!=0){
      switch(button)
	{
	case 1:if (front==0) mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]--;
	else mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]++;
	  break;
	case 2:
	case 3: if (front==0) mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]+=10;
	else mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]-=10;
	  break;
	}
    }
    else {
      switch(button){
      case 1:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]=
	  strtod(mw->menutext[i],(char **)NULL);
	break;
      case 2:
      case 3:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.myint_arr.no);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.myint_arr.no=
	  labs(strtol(mw->menutext[i],(char **)NULL,0))%
	  mw->menu->mydata[i].md.choice.myint_arr.max;
	break;
      }
    }
    
    sprintf(mw->menutext[i],"%s[%i]=%i",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.myint_arr.no,
	    mw->menu->mydata[i].md.choice.myint_arr.i[mw->menu->mydata[i].md.choice.myint_arr.no]);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case int_arrp:
    if (Ctrl!=0){
      switch(button)
	{
	case 1:
	case 2:
	case 3: if (front==0) mw->menu->mydata[i].md.choice.myint_arrp.no--;
	else mw->menu->mydata[i].md.choice.myint_arrp.no++;
	  break;
	}
       mw->menu->mydata[i].md.choice.myint_arrp.no+=
	 *mw->menu->mydata[i].md.choice.myint_arrp.max;
       mw->menu->mydata[i].md.choice.myint_arrp.no%=
	 *mw->menu->mydata[i].md.choice.myint_arrp.max;
    }
    else if (Shift!=0){
      switch(button)
	{
	case 1:if (front==0) mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]--;
	else mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]++;
	  break;
	case 2:
	case 3: if (front==0) mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]+=10;
	else mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]-=10;
	  break;
	}
    }
    else {
      switch(button){
      case 1:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]=
	  strtod(mw->menutext[i],(char **)NULL);
	break;
      case 2:
      case 3:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.myint_arrp.no);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.myint_arrp.no=
	  labs(strtol(mw->menutext[i],(char **)NULL,0))%
	  *mw->menu->mydata[i].md.choice.myint_arrp.max;
	break;
      }
    }
    
    sprintf(mw->menutext[i],"%s[%i]=%i",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.myint_arrp.no,
	    mw->menu->mydata[i].md.choice.myint_arrp.i[mw->menu->mydata[i].md.choice.myint_arrp.no]);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case double_arr:
    if (Ctrl!=0){
      switch(button)
	{
	case 1:
	case 2:
	case 3: if (front==0) mw->menu->mydata[i].md.choice.mydouble_arr.no--;
	else mw->menu->mydata[i].md.choice.mydouble_arr.no++;
	  break;
	}
       mw->menu->mydata[i].md.choice.mydouble_arr.no+=
	 mw->menu->mydata[i].md.choice.mydouble_arr.max;
       mw->menu->mydata[i].md.choice.mydouble_arr.no%=
	 mw->menu->mydata[i].md.choice.mydouble_arr.max;
    }
    else if (Shift!=0){
      paramd=&(mw->menu->mydata[i].md.choice.mydouble_arr.d[mw->menu->mydata[i].md.choice.mydouble_arr.no]);
      newexp=mw->menu->mydata[i].md.choice.mydouble_arr.exp;
      parami=*paramd/pow(10.,
			 1.*mw->menu->mydata[i].md.choice.mydouble_arr.exp-3);
      *paramd=1.*parami/1000;
      switch(button)
	{
	case 1: if (front==0) *paramd-=0.1; else *paramd+=0.1; break;
	case 2: if (front==0) *paramd-=1;  else *paramd+=1;  break;
	case 3: if (front==0) newexp--;  else newexp++;  break;
	}
      *paramd*=pow(10.,1.*mw->menu->mydata[i].md.choice.mydouble_arr.exp);
      mw->menu->mydata[i].md.choice.mydouble_arr.exp=newexp;
    }
    else {
      switch(button){
      case 1:
	sprintf(mw->menutext[i],"%g",
		mw->menu->mydata[i].md.choice.mydouble_arr.d[mw->menu->mydata[i].md.choice.mydouble_arr.no]);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.mydouble_arr.d[mw->menu->mydata[i].md.choice.mydouble_arr.no]=
	  strtod(mw->menutext[i],(char **)NULL);
	break;
      case 2:
      case 3:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.mydouble_arr.no);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.mydouble_arr.no=
	  labs(strtol(mw->menutext[i],(char **)NULL,0))%
	  mw->menu->mydata[i].md.choice.mydouble_arr.max;
	break;
      }
    }
    
    sprintf(mw->menutext[i],"%s[%i]=%10.5g",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.mydouble_arr.no,
	    mw->menu->mydata[i].md.choice.mydouble_arr.d[mw->menu->mydata[i].md.choice.mydouble_arr.no]);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case double_arrp:
    if (Ctrl!=0){
      switch(button)
	{
	case 1:
	  if (front==0) mw->menu->mydata[i].md.choice.mydouble_arrp.no--;
	  else mw->menu->mydata[i].md.choice.mydouble_arrp.no++;
	  mw->menu->mydata[i].md.choice.mydouble_arrp.no+=
	    *mw->menu->mydata[i].md.choice.mydouble_arrp.max;
	  mw->menu->mydata[i].md.choice.mydouble_arrp.no%=
	    *mw->menu->mydata[i].md.choice.mydouble_arrp.max;
	  break;
	case 2:
	case 3:
	  if (front==0)
	    (*mw->menu->mydata[i].md.choice.mydouble_arrp.max)--;
	  else (*mw->menu->mydata[i].md.choice.mydouble_arrp.max)++;
	  *mw->menu->mydata[i].md.choice.mydouble_arrp.d=
	    realloc(*mw->menu->mydata[i].md.choice.mydouble_arrp.d,
		    *mw->menu->mydata[i].md.choice.mydouble_arrp.max
		    *sizeof(double));
	  if (front) (*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[*mw->menu->mydata[i].md.choice.mydouble_arrp.max-1]=0;
	  else if (mw->menu->mydata[i].md.choice.mydouble_arrp.no>=
		   *mw->menu->mydata[i].md.choice.mydouble_arrp.max)
	    mw->menu->mydata[i].md.choice.mydouble_arrp.no=
	      *mw->menu->mydata[i].md.choice.mydouble_arrp.max-1;
	  break;
	}
    }
    else if (Shift!=0){
      paramd=&((*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[mw->menu->mydata[i].md.choice.mydouble_arrp.no]);
      newexp=mw->menu->mydata[i].md.choice.mydouble_arrp.exp;
      parami=*paramd/pow(10.,
			 1.*mw->menu->mydata[i].md.choice.mydouble_arrp.exp-3);
      *paramd=1.*parami/1000;
      switch(button)
	{
	case 1: if (front==0) *paramd-=0.1; else *paramd+=0.1; break;
	case 2: if (front==0) *paramd-=1;  else *paramd+=1;  break;
	case 3: if (front==0) newexp--;  else newexp++;  break;
	}
      *paramd*=pow(10.,1.*mw->menu->mydata[i].md.choice.mydouble_arrp.exp);
      mw->menu->mydata[i].md.choice.mydouble_arrp.exp=newexp;
    }
    else {
      switch(button){
      case 1:
	sprintf(mw->menutext[i],"%g",
		(*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[mw->menu->mydata[i].md.choice.mydouble_arrp.no]);
	GetNewValue(mw,i,mw->menutext[i],1);
	(*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[mw->menu->mydata[i].md.choice.mydouble_arrp.no]=
	  strtod(mw->menutext[i],(char **)NULL);
	break;
      case 2:
	sprintf(mw->menutext[i],"%i",
		*mw->menu->mydata[i].md.choice.mydouble_arrp.max);
	GetNewValue(mw,i,mw->menutext[i],1);
	parami=labs(strtol(mw->menutext[i],(char **)NULL,0));
	*mw->menu->mydata[i].md.choice.mydouble_arrp.d=
	  realloc(*mw->menu->mydata[i].md.choice.mydouble_arrp.d,
		  parami*sizeof(double));
	for (j=*mw->menu->mydata[i].md.choice.mydouble_arrp.max;
	     j<parami;j++)
	  (*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[j]=0;

	if (mw->menu->mydata[i].md.choice.mydouble_arrp.no>=parami)
	    mw->menu->mydata[i].md.choice.mydouble_arrp.no=parami-1;
	*mw->menu->mydata[i].md.choice.mydouble_arrp.max=parami;
	break;
      case 3:
	sprintf(mw->menutext[i],"%i",
		mw->menu->mydata[i].md.choice.mydouble_arrp.no);
	GetNewValue(mw,i,mw->menutext[i],1);
	mw->menu->mydata[i].md.choice.mydouble_arrp.no=
	  labs(strtol(mw->menutext[i],(char **)NULL,0))%
	  *mw->menu->mydata[i].md.choice.mydouble_arrp.max;
	break;
      }
    }
    if (*mw->menu->mydata[i].md.choice.mydouble_arrp.max>0)
    sprintf(mw->menutext[i],"%s[%i]{%i}=%10.5g",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.mydouble_arrp.no,
	    *mw->menu->mydata[i].md.choice.mydouble_arrp.max,
	    (*mw->menu->mydata[i].md.choice.mydouble_arrp.d)[mw->menu->mydata[i].md.choice.mydouble_arrp.no]);
    else
    sprintf(mw->menutext[i],"%s[%i]{%i}=N/A",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.mydouble_arrp.no,
	    *mw->menu->mydata[i].md.choice.mydouble_arrp.max);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case string_:
    /* I need to put in some safeguards regarding the length of the string. */
    sprintf(mw->menutext[i],"%s",mw->menu->mydata[i].md.choice.mystring.s);
    GetNewValue(mw,i,mw->menutext[i],0);
    sprintf(mw->menu->mydata[i].md.choice.mystring.s,mw->menutext[i]);
    
    sprintf(mw->menutext[i],"<%s:%s>",mw->menu->mydata[i].md.name,
	    mw->menu->mydata[i].md.choice.mystring.s);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case boolean_:{
    (*(mw->menu->mydata[i].md.choice.myint.i))++;
    (*(mw->menu->mydata[i].md.choice.myint.i))&=1;
    if (*(mw->menu->mydata[i].md.choice.myint.i))
      sprintf(mw->menutext[i],"%s on",mw->menu->mydata[i].md.name);
    else
      sprintf(mw->menutext[i],"%s off",mw->menu->mydata[i].md.name);
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,
		     mw->myfont,mw->menutext[i]);
    if (mw->type & zeichentypemask){ 
      mw->neu_zeichnenreq=1;
      definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  } 
  case function_:
    mw->menu->mydata[i].md.choice.myfunc();
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,
			   mw->menutext[i]);

    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case boolmenu_:{
    if (button==1){
      (*(mw->menu->mydata[i].md.choice.boolmenu.b))++;
      (*(mw->menu->mydata[i].md.choice.boolmenu.b))&=1;
      if (*(mw->menu->mydata[i].md.choice.boolmenu.b))
	sprintf(mw->menutext[i],"%s on",mw->menu->mydata[i].md.name);
      else
	sprintf(mw->menutext[i],"%s off",mw->menu->mydata[i].md.name);
      zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,
		       mw->myfont,mw->menutext[i]);
      if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
	definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
      }
      break;
    } else {
      if (mw->menu->mydata[i].md.choice.boolmenu.bm->winp->installed==0){
	XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,
		      &rx,&ry,&wx,&wy,&kb);
	myx=rx-wx+mw->menumaxx;
	myy=ry-wy;
	mapwindow(mw->menu->mydata[i].md.choice.boolmenu.bm->winp->myd,
		  mw->menu->mydata[i].md.choice.boolmenu.bm->winp,
		  myx,myy);
      }
      else
	unmapwindow(mw->menu->mydata[i].md.choice.boolmenu.bm->winp->myd,
		    mw->menu->mydata[i].md.choice.boolmenu.bm->winp);
      
      break;
    }
  }
  case menu_:
    if (mw->menu->mydata[i].md.choice.mymenu->winp->installed==0){
      XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,
		    &rx,&ry,&wx,&wy,&kb);
      myx=rx-wx+mw->menumaxx;
      myy=ry-wy;
      mapwindow(mw->menu->mydata[i].md.choice.mymenu->winp->myd,
		mw->menu->mydata[i].md.choice.mymenu->winp,
		myx,myy);
    }
    else
      unmapwindow(mw->menu->mydata[i].md.choice.mymenu->winp->myd,
		  mw->menu->mydata[i].md.choice.mymenu->winp);
    
    break;
  case protofunc_:
    if (mw->type & zeichentypemask){ 
	mw->neu_zeichnenreq=1;
        definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case curve2d_:
    tmpzd=(nzdata *) malloc(sizeof(nzdata));
    tmpzd->graph2d.comments=0;
    tmpzd->graph2d.newdata=1;
    tmpzd->graph2d.draw=(int *) 
      malloc((mw->menu->mydata[i].md.choice.mygraph->anzN_RxR
	      +mw->menu->mydata[i].md.choice.mygraph->anzN_RxRp)*sizeof(int));
    for (k=0;(mw->menu->mydata[i].md.choice.mygraph->anzN_RxR
	      +mw->menu->mydata[i].md.choice.mygraph->anzN_RxRp)>k;k++)
      tmpzd->graph2d.draw[k]=0;
    tmpzd->graph2d.draw1=(int *) 
      malloc((mw->menu->mydata[i].md.choice.mygraph->anzN_R
	      +mw->menu->mydata[i].md.choice.mygraph->anzN_Rp)*sizeof(int));
    for (k=0;(mw->menu->mydata[i].md.choice.mygraph->anzN_R
	      +mw->menu->mydata[i].md.choice.mygraph->anzN_Rp)>k;k++)
      tmpzd->graph2d.draw1[k]=0;

    tmpzd->graph2d.rough=1;
    tmpzd->graph2d.copy=1;
    tmpzd->graph2d.scaling=1;
    tmpzd->graph2d.logx=0;
    tmpzd->graph2d.logy=0;
    tmpzd->graph2d.xmin=tmpzd->graph2d.ymin=-1;
    tmpzd->graph2d.xmax=tmpzd->graph2d.ymax= 1;

    tmpmenu=mw->menu; /* needed in case of remapping of windows. 
			 See below. */
    StartMenu("2d Graph",0);
    StartMenu("Control",0);

      DefineBool("AutoScaling",&tmpzd->graph2d.scaling);
      DefineBool("logx",&tmpzd->graph2d.logx);
      DefineBool("logy",&tmpzd->graph2d.logy);
      DefineDouble("xmin",&tmpzd->graph2d.xmin);
      DefineDouble("xmax",&tmpzd->graph2d.xmax);
      DefineDouble("ymin",&tmpzd->graph2d.ymin);
      DefineDouble("ymax",&tmpzd->graph2d.ymax);
    EndMenu();
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzN_R;k++){
      StartBoolMenu(mw->menu->mydata[i].md.choice.mygraph->N_Rname[k],
		    &tmpzd->graph2d.draw1[k]);

      /* These menus need to be implemented later. We will simply go
	 with the simple implementation of mods right now */
      /*
      DefineColorMenu(&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].color);
      DefineLinetypeMenu(&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].linetype);
      DefineShapeMenu(&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].shape);
      */

      DefineMod("Color",
		&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].color,
		NumberOfColors());
      DefineMod("Line Type",
		&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].linetype,
		NumberOfLinetypes);
      DefineMod("Shape",
		&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].shape,
		NumberOfShapes);
		
      DefineDouble("Size",&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].size);
      DefineBool("Fill",&mw->menu->mydata[i].md.choice.mygraph->dN_R[k].fill);
      EndMenu();
    }
    for (j=0;j<mw->menu->mydata[i].md.choice.mygraph->anzN_Rp;k++,j++){
      StartBoolMenu(mw->menu->mydata[i].md.choice.mygraph->N_Rnamep[j],
		    &tmpzd->graph2d.draw1[k]);
      DefineMod("Color",
		&mw->menu->mydata[i].md.choice.mygraph->dN_Rp[j].color,
		NumberOfColors());
      DefineMod("Line Type",
		&mw->menu->mydata[i].md.choice.mygraph->dN_Rp[j].linetype,
		NumberOfLinetypes);
      DefineMod("Shape",
		&mw->menu->mydata[i].md.choice.mygraph->dN_Rp[j].shape,
		NumberOfShapes);

      DefineDouble("Size",
		   &mw->menu->mydata[i].md.choice.mygraph->dN_Rp[j].size);
      DefineBool("Fill",
		 &mw->menu->mydata[i].md.choice.mygraph->dN_Rp[j].fill);
      EndMenu();
    }
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzN_RxR;k++){
      StartBoolMenu(mw->menu->mydata[i].md.choice.mygraph->N_RxRname[k],
		 &tmpzd->graph2d.draw[k]);
      DefineMod("Color",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxR[k].color,
		NumberOfColors());
      DefineMod("Line Type",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxR[k].linetype,
		NumberOfLinetypes);
      DefineMod("Shape",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxR[k].shape,
		NumberOfShapes);
		
      DefineDouble("Size",
		   &mw->menu->mydata[i].md.choice.mygraph->dN_RxR[k].size);
      DefineBool("Fill",
		 &mw->menu->mydata[i].md.choice.mygraph->dN_RxR[k].fill);
      EndMenu();
    }
    for (j=0;j<mw->menu->mydata[i].md.choice.mygraph->anzN_RxRp;k++,j++){
      StartBoolMenu(mw->menu->mydata[i].md.choice.mygraph->N_RxRpname[j],
		    &tmpzd->graph2d.draw[k]);
      DefineMod("Color",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxRp[j].color,
		NumberOfColors());
      DefineMod("Line Type",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxRp[j].linetype,
		NumberOfLinetypes);
      DefineMod("Shape",
		&mw->menu->mydata[i].md.choice.mygraph->dN_RxRp[j].shape,
		NumberOfShapes);

      DefineDouble("Size",
		   &mw->menu->mydata[i].md.choice.mygraph->dN_RxRp[j].size);
      DefineBool("Fill",
		 &mw->menu->mydata[i].md.choice.mygraph->dN_RxRp[j].fill);
      EndMenu();
    }
    DefineItem(newdraw_);
    DefineItem(print_);
    DefineItem(close_);
    EndMenu();
    /* At this point mw may have been changed by the menu procedures
       due to the creation of a new window. It should, hoewer have been 
       correctly changed in the corresponding menudata.
       */
    mw=tmpmenu->winp;
    mymenu->zd=tmpzd;
    mymenu->graph=mw->menu->mydata[i].md.choice.mygraph;
    mymenu->winp->installed=1;
    mymenu->winp->type|=zeichentypemask;
    mymenu->winp->neu_zeichnenreq=0;
    mymenu->winp->neu_zeichnentype=curve_2d;
    mymenu->winp->neu_zeichnen=
      neu_zeichnen_2dgraph;
    mymenu->winp->installed=0;
    mymenu->winp->doublebuffer=1;
    mymenu->winp->newdraw=1;
    XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,&rx,&ry,&wx,&wy,&kb);
    myx=rx-wx+mw->menumaxx;
    myy=ry-wy;
    mapwindow(mymenu->winp->myd,mymenu->winp,myx,myy);
    break;
  case graph2d_:
    tmpzd=(nzdata *) malloc(sizeof(nzdata));
    tmpzd->nz1.comments=0;
    tmpzd->nz1.Autoscale=1;
    tmpzd->nz1.scaleonce=1;
    tmpzd->nz1.rough =1;
    tmpzd->nz1.NEWDIMENSIONS=0;
    tmpzd->nz1.newdata=1;
    tmpzd->nz1.nox=0; /* need to check the meaning of this parameter */
    tmpzd->nz1.getgraph=(int *) 
      malloc(mw->menu->mydata[i].md.choice.mygraph->anzNxN_R*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_R;k++)
      if (k==mw->menu->mydata[i].md.choice.mygraph->anzNxN_R-1) 
	tmpzd->nz1.getgraph[k]=1;
      else tmpzd->nz1.getgraph[k]=0;
    tmpzd->nz1.persp_factor=1;
    tmpzd->nz1.scalefakt=0.9;
    tmpzd->nz1.xangle=10;
    tmpzd->nz1.yangle=10;
    tmpzd->nz1.zfact=1;
    tmpzd->nz1.zmin=-1;
    tmpzd->nz1.zmax=1;
    tmpzd->nz1.copy[0]=1;
    tmpzd->nz1.copy[1]=1;
    tmpzd->nz1.meinit=0;

    tmpmenu=mw->menu; /* needed in case of remapping of windows. 
			 See below. */
    StartMenu("2d Graph",0);
    DefineDouble("X angle",&tmpzd->nz1.xangle);
    DefineDouble("Y angle",&tmpzd->nz1.yangle);
    StartMenu("Options",0);
    DefineBool("Automatic Scaling",&tmpzd->nz1.Autoscale);
    DefineBool("Single Scaling",&tmpzd->nz1.scaleonce);
    DefineDouble("Z min",&tmpzd->nz1.zmin);
    DefineDouble("Z max",&tmpzd->nz1.zmax);
    DefineDouble("Size Factor",&tmpzd->nz1.scalefakt);
    DefineDouble("Prespective factor",&tmpzd->nz1.persp_factor);
    DefineDouble("Z-factor",&tmpzd->nz1.zfact);
    DefineItem(close_);
    EndMenu();
    DefineInt("Rough",&tmpzd->nz1.rough);
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_R;k++)
      DefineBool(mw->menu->mydata[i].md.choice.mygraph->NxN_Rname[k],
		 &tmpzd->nz1.getgraph[k]);
    DefineItem(newdraw_);
    DefineItem(print_);
    DefineItem(close_);
    EndMenu();
    /* At this point mw may have been changed by the menu procedures
       due to the creation of a new window. It should, hoewer have been 
       correctly changed in the corresponding menudata.
       */
    mw=tmpmenu->winp;
    mymenu->zd=tmpzd;
    mymenu->graph=mw->menu->mydata[i].md.choice.mygraph;
    mymenu->winp->installed=1;
    mymenu->winp->type|=zeichentypemask;
    mymenu->winp->neu_zeichnenreq=0;
    mymenu->winp->neu_zeichnentype=graph_2d;
    mymenu->winp->neu_zeichnen=
      neu_zeichnen_1;
    mymenu->winp->installed=0;
    mymenu->winp->doublebuffer=1;
    mymenu->winp->newdraw=1;
    XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,&rx,&ry,&wx,&wy,&kb);
    myx=rx-wx+mw->menumaxx;
    myy=ry-wy;
    mapwindow(mymenu->winp->myd,mymenu->winp,myx,myy);    
    break;

  case contour2d_:
    tmpzd=(nzdata *) malloc(sizeof(nzdata));
    tmpzd->nz2.comments=0;
    tmpzd->nz2.koord=0;
    tmpzd->nz2.koordxmin=0;
    tmpzd->nz2.koordxmax=1;
    tmpzd->nz2.koordlogx=0;
    tmpzd->nz2.koordymin=0;
    tmpzd->nz2.koordymax=1;
    tmpzd->nz2.koordlogy=0;
    tmpzd->nz2.newdata=1;
    tmpzd->nz2.density=1;
    tmpzd->nz2.rough=1;
    tmpzd->nz2.copy[0]=1;
    tmpzd->nz2.copy[0]=1;

    tmpzd->nz2.draw= (int*) 
      malloc((mw->menu->mydata[i].md.choice.mygraph->anzNxN_R
	      +mw->menu->mydata[i].md.choice.mygraph->anzNxN_Rp)*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_R
	   +mw->menu->mydata[i].md.choice.mygraph->anzNxN_Rp;k++)
      tmpzd->nz2.draw[k]=0;

    tmpzd->nz2.draw_ufield= (int*) 
      malloc(mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxR*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxR;k++)
      tmpzd->nz2.draw_ufield[k]=0;

    tmpzd->nz2.draw_tfield= (int*) 
      malloc(mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxRxRt*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxRxRt;k++)
      tmpzd->nz2.draw_tfield[k]=0;

    tmpzd->nz2.rough_t= (int*) 
      malloc(mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxRxRt*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxRxRt;k++)
      tmpzd->nz2.rough_t[k]=1;

    tmpzd->nz2.nocuts=1;
    tmpzd->nz2.maxcuts=100;
    tmpzd->nz2.adjustcuts=1;
    tmpzd->nz2.DensityLegend=1;
    tmpzd->nz2.BarInset=0;
    tmpzd->nz2.BarWidth=0.25;
    tmpzd->nz2.BarHeight=1;
    tmpzd->nz2.BarXOfs=0;
    tmpzd->nz2.BarYOfs=0;
    tmpzd->nz2.adjustdensity=1;
    tmpzd->nz2.cut = (double *) malloc(tmpzd->nz2.maxcuts*sizeof(double));
    tmpzd->nz2.NewXsize=1;
    tmpzd->nz2.NewYsize=1;
    tmpzd->nz2.DoCenter=0;
    tmpzd->nz2.xofs=0;
    tmpzd->nz2.yofs=0;
    tmpzd->nz2.magnify=0;

    tmpmenu=mw->menu; /* needed in case of remapping of windows. 
			 See below. */
    StartMenu("2d Contour",0);
    StartMenu("Control",0);
    DefineBool("Comments",&tmpzd->nz2.comments);
    StartMenu("Coordinate system",0);
    DefineBool("Coordinate system",&tmpzd->nz2.koord);
    DefineDouble("xmin",&tmpzd->nz2.koordxmin);
    DefineDouble("xmax",&tmpzd->nz2.koordxmax);
    DefineBool("log x-axis",&tmpzd->nz2.koordlogx);
    DefineDouble("ymin",&tmpzd->nz2.koordymin);
    DefineDouble("ymax",&tmpzd->nz2.koordymax);
    DefineBool("log y-axis",&tmpzd->nz2.koordlogy);
    EndMenu();
    DefineBool("Legend",&tmpzd->nz2.DensityLegend);
    DefineBool("Bar inset",&tmpzd->nz2.BarInset);
    DefineDouble("Bar X ofset",&tmpzd->nz2.BarXOfs);
    DefineDouble("Bar Y ofset",&tmpzd->nz2.BarYOfs);
    DefineDouble("Bar width",&tmpzd->nz2.BarWidth);
    DefineDouble("Bar Height",&tmpzd->nz2.BarHeight);
    DefineDouble("Density Min",&tmpzd->nz2.density_min);
    DefineDouble("Density Max",&tmpzd->nz2.density_max);
    DefineBool("Adjust Density",&tmpzd->nz2.adjustdensity);
    DefineDoubleArrp("Contour",&tmpzd->nz2.cut,&tmpzd->nz2.nocuts);
    DefineBool("Adjust cuts",&tmpzd->nz2.adjustcuts);
    EndMenu();
    /* We also want a dynamically generated menu for the contour lines */
    DefineMod("Density",&tmpzd->nz2.density,3);
    StartMenu("Magnify",0);
    DefineInt("New X size",&tmpzd->nz2.NewXsize);
    DefineInt("New Y size",&tmpzd->nz2.NewYsize);
    DefineBool("Center",&tmpzd->nz2.DoCenter);
    DefineDouble("X pos",&tmpzd->nz2.xofs);
    DefineDouble("Y pos",&tmpzd->nz2.yofs);
    DefineBool("Magnification",&tmpzd->nz2.magnify);
    EndMenu();
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_R
	   +mw->menu->mydata[i].md.choice.mygraph->anzNxN_Rp;k++)
      if (k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_R)
	DefineBool(mw->menu->mydata[i].md.choice.mygraph->NxN_Rname[k],
		   &tmpzd->nz2.draw[k]);
      else DefineBool(mw->menu->mydata[i].md.choice.mygraph->NxN_Rpname[k-mw->menu->mydata[i].md.choice.mygraph->anzNxN_R],
		   &tmpzd->nz2.draw[k]);
     
    /*
    printf("genericmenu:graph->pNxN_R[0]=%p\n",mw->menu->mydata[i].md.choice.mygraph->pNxN_R[0]);
    */

    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxR;k++)
      DefineBool(mw->menu->mydata[i].md.choice.mygraph->NxN_RxRname[k],
		 &tmpzd->nz2.draw_ufield[k]);
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzNxN_RxRxRt;k++){
      DefineBool(mw->menu->mydata[i].md.choice.mygraph->NxN_RxRxRtname[k],
		 &tmpzd->nz2.draw_tfield[k]);
      DefineInt("rough",
		 &tmpzd->nz2.rough_t[k]);
    }
    DefineItem(newdraw_);
    DefineItem(print_);
    DefineItem(close_);
    EndMenu();
    /* At this point mw may have been changed by the menu procedures
       due to the creation of a new window. It should, hoewer have been 
       correctly changed in the corresponding menudata.
       */
    mw=tmpmenu->winp;
    mymenu->zd=tmpzd;
    mymenu->graph=mw->menu->mydata[i].md.choice.mygraph;
    mymenu->winp->installed=1;
    mymenu->winp->type|=zeichentypemask;
    mymenu->winp->neu_zeichnenreq=0;
    mymenu->winp->neu_zeichnentype=contour_u_2d;
    mymenu->winp->neu_zeichnen= neu_zeichnen2;
    mymenu->winp->installed=0;
    mymenu->winp->doublebuffer=1;
    mymenu->winp->newdraw=1;
    XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,&rx,&ry,&wx,&wy,&kb);
    myx=rx-wx+mw->menumaxx;
    myy=ry-wy;
    mapwindow(mymenu->winp->myd,mymenu->winp,myx,myy);
    
    break;

  case contour3d_:
    tmpzd=(nzdata *) malloc(sizeof(nzdata));
    tmpzd->nzc.rough =1;
    tmpzd->nzc.newdata=1;
    tmpzd->nzc.getgraph=(int *) 
      malloc(mw->menu->mydata[i].md.choice.mygraph->anzcont3d*sizeof(int));
    for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzcont3d;k++)
      if (k==0) tmpzd->nzc.getgraph[k]=1;
      else tmpzd->nzc.getgraph[k]=0;
    tmpzd->nzc.persp_factor=1;
    tmpzd->nzc.scalefakt=0.9;
    tmpzd->nzc.xangle=10;
    tmpzd->nzc.yangle=10;
    tmpzd->nzc.copy[0]=1;
    tmpzd->nzc.copy[1]=1;
    tmpzd->nzc.copy[2]=1;

    tmpmenu=mw->menu; /* needed in case of remapping of windows. 
			 See below. */
    StartMenu("3d Contour",0);
    DefineDouble("X angle",&tmpzd->nzc.xangle);
    DefineDouble("Y angle",&tmpzd->nzc.yangle);
    DefineDouble("Size Factor",&tmpzd->nzc.scalefakt);
    DefineDouble("Prespective factor",&tmpzd->nzc.persp_factor);
    DefineInt("Rough",&tmpzd->nzc.rough);
      for (k=0;k<mw->menu->mydata[i].md.choice.mygraph->anzcont3d;k++)
	DefineBool(mw->menu->mydata[i].md.choice.mygraph->cont3dname[k],
		 &tmpzd->nzc.getgraph[k]);
    DefineItem(newdraw_);
    DefineItem(print_);
    DefineItem(close_);
    EndMenu();
    /* At this point mw may have been changed by the menu procedures
       due to the creation of a new window. It should, hoewer have been 
       correctly changed in the corresponding menudata.
       */
    mw=tmpmenu->winp;
    mymenu->zd=tmpzd;
    mymenu->graph=mw->menu->mydata[i].md.choice.mygraph;
    mymenu->winp->installed=1;
    mymenu->winp->type|=zeichentypemask;
    mymenu->winp->neu_zeichnenreq=0;
    mymenu->winp->neu_zeichnentype=contour_3d;
    mymenu->winp->neu_zeichnen=
      neu_zeichnen_contour3d;
    mymenu->winp->installed=0;
    mymenu->winp->doublebuffer=1;
    mymenu->winp->newdraw=1;
    XQueryPointer(mw->myd,mw->menuw[i],&myroot,&mychild,&rx,&ry,&wx,&wy,&kb);
    myx=rx-wx+mw->menumaxx;
    myy=ry-wy;
    mapwindow(mymenu->winp->myd,mymenu->winp,myx,myy);    
    break;



  case newdraw_:
    if (mw->type & zeichentypemask){ 
      mw->neu_zeichnenreq=1;
      definerequests(mw->menu->graph,mw->neu_zeichnentype,mw->menu->zd);
    }
    break;
  case print_:
    if (button==1){
      if (mw->menu->mydata[i].md.choice.mymenu==NULL){
	tmpmenu=mw->menu; /* needed in case of remapping of windows. 
			     See below. */
	StartMenu("Printing",0);
	DefineDouble("width",&PSwidth);
	DefineDouble("height",&PSheight);
	DefineBool("Landscape",&PSlandscape);
	DefineBool("Paper Letter",&PSletter);
	DefineBool("Print to printer",&PSprinter);
	DefineItem(doprint_);
	EndMenu();
	
	mymenu->zd=tmpmenu->zd;
	mymenu->graph=tmpmenu->graph;
	mymenu->winp->neu_zeichnentype=tmpmenu->winp->neu_zeichnentype;
	mymenu->winp->neu_zeichnen=tmpmenu->winp->neu_zeichnen;
	mymenu->winp->type|=menutypemask;
	mymenu->winp->installed=0;
	mymenu->winp->doublebuffer=0;
	mymenu->winp->newdraw=1;
	XQueryPointer(mw->myd,mw->menuw[i],&myroot,
		      &mychild,&rx,&ry,&wx,&wy,&kb);
	myx=rx-wx+mw->menumaxx;
	myy=ry-wy;
	/* need to use tmpmenu->winp instead of mw because the menu might 
	   reaarange the menus */
	tmpmenu->winp->menu->mydata[i].md.choice.mymenu=mymenu;
	mapwindow(mymenu->winp->myd,mymenu->winp,myx,myy);    
      }
      else{
	unmapwindow(mw->menu->mydata[i].md.choice.mymenu->winp->myd,
		    mw->menu->mydata[i].md.choice.mymenu->winp);
	mw->menu->mydata[i].md.choice.mymenu=NULL;
      }
      break;
    }
    /* the else case runs automatically into doprint */

  case doprint_:
    mw->menutext[i]="printing...";
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,mw->menutext[i]);
    pscounter++;
    sprintf(psname,"%s%03d%s","graph3d_",pscounter,".eps");
    /* Here I now assume that no new data are needed! Not quite clean I fear.*/
    mw->neu_zeichnen(mw->myd,mw,1,psname,PSwidth,PSheight,
		     PSletter,PSlandscape,*mw->menu->graph);
    if (PSprinter){
      sprintf(command,"lpr %s",psname);
      system(command);
    }
    mw->menutext[i]="print";
    zeichne_menutext(mw->myd,mw->menuw[i],mw->menugc,mw->myfont,mw->menutext[i]);
    break;

  case close_:
    /***************************************************/
    /* For graphic windows some data need to be freed! */
    /***************************************************/
    unmapwindow(mw->myd,mw);
    break;
  default:
    {
      printf("Error in genericmenu(): mytype %i not supported\n",
	     mw->menu->mydata[i].md.mytype);
    }
  }
}


void DefineMenuWindow(menudat *md){
  int i,localwc;
  char longstr[]="Colin = 1237.234e3";

  /*
  if (md->graph!=0){
    printf("%s:graph=%p\n",md->menuname,md->graph);
    for (i=0;i<md->graph->anzNxN_R;i++) printf("%p,",md->graph->pNxN_R[i]);
    printf("\n");
  }
  */

  if (wc==NULL||mw==NULL) {
    printf("menu.c:DefineMenuWindow:Windows undefinded.\n"
	   "Use InitGraphics() before defining menus!\n");
    exit(1);
  }
  (*wc)++;
  localwc=*wc;
  if (*wc>=wcmax){
    wcmax+=optlen;
    *mw=(mywindow *) realloc(*mw,wcmax*sizeof(mywindow));
    /* possible problem for reallocation: we have direct pointers
       to the windows in menudat! So we will need to correct these
       if there is a remapping!
       */
    for (i=0;i<*wc;i++){
      if ((*mw)[i].type&menutypemask){
	(*mw)[i].menu->winp=&(*mw)[i];
      }
    }
  }
  md->winp=&((*mw)[localwc]);
  (*mw)[localwc].Name=md->menuname;
  (*mw)[localwc].number = localwc;
  (*mw)[localwc].type = menutypemask;
  (*mw)[localwc].menu=md;
  (*mw)[localwc].menu->zd=NULL;
  switch (md->menutype){
  case NORMAL: 
    (*mw)[localwc].menuaction = genericmenu;
    (*mw)[localwc].nomenu=md->no;
    (*mw)[localwc].menutext= (char**) 
      malloc((*mw)[localwc].nomenu*sizeof(char*));
    (*mw)[localwc].menuw=
      (Drawable*) malloc((*mw)[localwc].nomenu*sizeof(Drawable));
    for (i=0; i<(*mw)[localwc].nomenu; i++) {
      (*mw)[localwc].menutext[i]= (char*) malloc(100*sizeof(char));
      switch (md->mydata[i].md.mytype){
      case int_:
	sprintf((*mw)[localwc].menutext[i],"%s = %i",
		md->mydata[i].md.name,
		*md->mydata[i].md.choice.myint.i);
	break;
      case long_:  
	sprintf((*mw)[localwc].menutext[i],"%s = %li",
		md->mydata[i].md.name,
		*md->mydata[i].md.choice.mylong.i);
	break;
      case mod_:
	sprintf((*mw)[localwc].menutext[i],"%s = %i",
		md->mydata[i].md.name,
		*md->mydata[i].md.choice.mymod.i);
	break;
      case double_:
	sprintf((*mw)[localwc].menutext[i],"%s = %g",
		md->mydata[i].md.name,
		*md->mydata[i].md.choice.mydouble.d);
	break;
      case float_:
	sprintf((*mw)[localwc].menutext[i],"%s = %g",
		md->mydata[i].md.name,
		*md->mydata[i].md.choice.myfloat.d);
	break;
      case int_arr:
	sprintf((*mw)[localwc].menutext[i],"%s[%i] = %i",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.myint_arr.no,
		md->mydata[i].md.choice.myint_arr.i[md->mydata[i].md.choice.myint_arr.no]);
	break;
      case int_arrp:
	sprintf((*mw)[localwc].menutext[i],"%s[%i] = %i",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.myint_arrp.no,
		md->mydata[i].md.choice.myint_arrp.i[md->mydata[i].md.choice.myint_arrp.no]);
	break;
      case double_arr:
	sprintf((*mw)[localwc].menutext[i],"%s[%i] = %g",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.mydouble_arr.no,
		md->mydata[i].md.choice.mydouble_arr.d[md->mydata[i].md.choice.mydouble_arr.no]);
	break;
      case double_arrp:
	if (*md->mydata[i].md.choice.mydouble_arrp.max>0)
	sprintf((*mw)[localwc].menutext[i],"%s[%i]{%i} = %g",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.mydouble_arrp.no,
		*md->mydata[i].md.choice.mydouble_arrp.max,
		(*md->mydata[i].md.choice.mydouble_arrp.d)[md->mydata[i].md.choice.mydouble_arrp.no]);
	else
	sprintf((*mw)[localwc].menutext[i],"%s[%i]{%i} = %g",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.mydouble_arrp.no,
		*md->mydata[i].md.choice.mydouble_arrp.max,
		(*md->mydata[i].md.choice.mydouble_arrp.d)[md->mydata[i].md.choice.mydouble_arrp.no]);
	break;
      case string_:
	sprintf((*mw)[localwc].menutext[i],"%s = %s",
		md->mydata[i].md.name,
		md->mydata[i].md.choice.mystring.s);
	break;
      case boolmenu_:
	if (*md->mydata[i].md.choice.boolmenu.b)
	  sprintf((*mw)[localwc].menutext[i],"%s on",
		  md->mydata[i].md.name);
	else
	  sprintf((*mw)[localwc].menutext[i],"%s off",
		  md->mydata[i].md.name);
	DefineMenuWindow(md->mydata[i].md.choice.boolmenu.bm);
	break;
      case boolean_:
	if (*md->mydata[i].md.choice.myint.i)
	  sprintf((*mw)[localwc].menutext[i],"%s on",
		  md->mydata[i].md.name);
	else
	  sprintf((*mw)[localwc].menutext[i],"%s off",
		  md->mydata[i].md.name);
	break;
      case function_:
	sprintf((*mw)[localwc].menutext[i],"%s",md->mydata[i].md.name);
	break; 

      case menu_: 
	sprintf((*mw)[localwc].menutext[i],"%s",md->mydata[i].md.name);
	DefineMenuWindow(md->mydata[i].md.choice.mymenu);
	break;
      case curve2d_:
      case graph2d_:
      case contour2d_:
      case contour3d_:
	sprintf((*mw)[localwc].menutext[i],"%s",md->mydata[i].md.name);
	break;
      case newdraw_:
	sprintf((*mw)[localwc].menutext[i],"Redraw");
	break;
      case print_:
	sprintf((*mw)[localwc].menutext[i],"Print");
	break;
      case doprint_:
	sprintf((*mw)[localwc].menutext[i],"Print now");
	break;
      case close_:
	sprintf((*mw)[localwc].menutext[i],"Close");
	break;
      default : 
	printf("menu.c:DefineMenuWindow:undefined type %i",
	       md->mydata[i].md.mytype);
	exit(1);
      }
    }
    break;
    /* The choice menu option will not be implemented right now.
  case CHOICE:   
    my->menucation = choicemenu;
    (*mw)[localwc].nomenu=0;
    for (i=0;i<md->no;i++){ 
      if (md->mydata[i].cd.mytype=protofunc_){
	(*mw)[localwc].nomenu++;
	if (md->mydata[i].cd.flag==1){
	  for (i++;(mydata[i].cd.mytype!=protofunc_)&&(i<md->no);i++){
	    (*mw)[localwc].nomenu++;
	  }
	  i--;
	}
      }
    }
    (*mw)[localwc].menutext= (char**) realloc((*mw)[localwc].menutext,(*mw)[localwc].nomenu*sizeof(char*));
    (*mw)[localwc].menuw=(Drawable*) malloc((*mw)[localwc].nomenu*sizeof(Drawable));
    for (i=0; i<=(*mw)[localwc].nomenu; i++) {
      (*mw)[localwc].menutext[i]= (char*) malloc(50*sizeof(char));
      switch (md->mydata[i].md->mytype){
      case int_:   
	sprintf((*mw)[localwc].menutext[i],"%s = %i",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.myint.i);
	break;
      case long_:  
	sprintf((*mw)[localwc].menutext[i],"%s = %li",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.mylong.i);
	break;
      case mod_:  
	sprintf((*mw)[localwc].menutext[i],"%s = %i",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.mymod.i);
	break;
      case double_:
	sprintf((*mw)[localwc].menutext[i],"%s = %g",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.mydouble);
	break; 
      case float_:
	sprintf((*mw)[localwc].menutext[i],"%s = %g",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.myfloat);
	break; 
      case string_:
	sprintf((*mw)[localwc].menutext[i],"%s = %s",
		md->mydata[i].md.name,
		md->mydata[i].md->choice.mystring.s);
	break; 
      case menu_: 
	sprintf((*mw)[localwc].menutext[i],"%s",md->mydata[i].md.name);
	break;
      default : 
	printf("menu.c:definewindow:undefined type %i",
	       md->mydata[i].md.mytype);
	exit(1);
      }
    }
    break;
    */
  default: 
    printf("menu.c:definewindow: menutype %i unknown",
	   md->menutype); 
    exit(1);
  }
  (*mw)[localwc].installed =md->installed;
  (*mw)[localwc].doublebuffer=0;

  if ((*mw)[localwc].nomenu!=i) { 
    printf("menu.c:%s: DefineMenuWindow(%i != %i) exiting...\n",
	   (*mw)[localwc].Name,(*mw)[localwc].nomenu,i);
    exit(1);
  }

  if ((*mw)[localwc].type&menutypemask) {
    (*mw)[localwc].menulength=(int *) malloc(nofoundfonts*sizeof(int));
    for(i=0;i<nofoundfonts;i++)
      (*mw)[localwc].menulength[i]=
	XTextWidth(menfonts[i],longstr,strlen(longstr));
    (*mw)[localwc].myfont=defaultfont;
  }
  (*mw)[localwc].newdraw=1;
    (*mw)[localwc].myd=Actdisplay;
    if ((*mw)[localwc].installed){
      mapwindow((*mw)[localwc].myd,&(*mw)[localwc],100,100);
    }
}

void DefineWindowInformation(mywindow **mw_,int *wc_){
  mw=mw_;
  wc=wc_;
}

void CheckMenuMemory(menudat *actmenu){
  if (actmenu->no>=actmenu->maxno-1) {
    actmenu->maxno+=optlen;
    actmenu->mydata=(datastruct *) 
      realloc(actmenu->mydata,actmenu->maxno*sizeof(datastruct));
    if (actmenu->mydata==NULL){
      printf("menu.c:DefineItem: Fatal error in realloc(), exiting...");
      exit(1);
    }
  }
}

void StartMenu(char *Name,int installed){
  static  int initialized=0;

  if (initialized==0){
    InitGraphics();/*So the user does not have to do this */
    initialized=1;
  }
  menulevel++;
  newmenu=(menudat *) malloc(sizeof(menudat));
  if (menulevel>0){
    CheckMenuMemory(actmenu);
    actmenu->mydata[actmenu->no].md.name=Name;
    actmenu->mydata[actmenu->no].md.mytype=menu_;
    actmenu->mydata[actmenu->no].md.choice.mymenu=newmenu;
    newmenu->parentmenu=actmenu;
    actmenu->no++;
  }
  else {
    newmenu->parentmenu=NULL;
    mymenu=newmenu;

    /* DefaultMenu();  Gives Copyright information, general choices of
		      apearence of the menus etc. 
		      Carefull: This might give a few default menues
		      too many!
       	      */

  }
  actmenu=newmenu;
  actmenu->menutype=NORMAL;
  actmenu->menuname=Name;
  actmenu->graph=NULL;
  actmenu->installed=installed;
  actmenu->no=0;
  actmenu->maxno=optlen;
  actmenu->mydata=(datastruct *) malloc(optlen*sizeof(datastruct));
}

void StartBoolMenu(char *Name,int *b){
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.choice.boolmenu.b=b;
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=boolmenu_;

  menulevel++;
  newmenu=(menudat *) malloc(sizeof(menudat));
  actmenu->mydata[actmenu->no].md.choice.boolmenu.bm=newmenu;
  newmenu->parentmenu=actmenu;
  actmenu->no++;

  actmenu=newmenu;
  actmenu->menutype=NORMAL;
  actmenu->menuname=Name;
  actmenu->graph=NULL;
  actmenu->installed=0;
  actmenu->no=0;
  actmenu->maxno=optlen;
  actmenu->mydata=(datastruct *) malloc(optlen*sizeof(datastruct));
}

void EndMenu(){
  menulevel--;

  if (menulevel==-1){
    DefineMenuWindow(mymenu);
  }
  if (menulevel<-1) {
    printf("menu.c:EndMenu: one EndMenu too much! Exiting...");
    exit(1);
  }
  actmenu=actmenu->parentmenu;
}

/* I am no longer entirely sure what exactely was reqired for my 
idea of a Choice menu. I will rest this prototype for the time
beeing. 

void StartChoiceMenu(const char *Name,fp myfp){
  menulevel++;
  newmenu=(choicedatastruct *) malloc(sizeof(choicedatastruct));
  if (menulevel>0){
    actmenu->no++;
    newmenu->parentmenu=actmenu;
  }
  else newmenu->parentmenu=NULL;
  actmenu=newmenu;
  actmenu->no=0;
  actmenu->maxno=optlen;
  actmenu->mydata=(datastruct *) malloc(optlen*sizeof(datastruct));
  
  actmenu->mydata[actmenu->no].cd.myfunc=myfp;
  actmenu->mydata[actmenu->no].cd.name=NULL;
  actmenu->mydata[actmenu->no].cd.flag=-1;
  actmenu->mydata[actmenu->no].cd.type=protofunc_;
  actmenu->no++;
}
*/

/* Need to allow for Flags to be set for the program for each function!*/
void DefineFunction(char *Name,fp myfp){
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=function_;
  actmenu->mydata[actmenu->no].md.choice.myfunc=myfp;
  actmenu->no++;
}

void DefineItem(int vartype)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.mytype=vartype;
  switch (vartype){
  case newdraw_:
    break;
  case print_ :
    actmenu->mydata[actmenu->no].md.choice.mymenu=NULL;
    break;
  case doprint_:
    break;
  case close_ :
    break;
  default: {
      printf("menu.c:DefineItem: Error, variable type %i "
	     "not defined. Exiting...",vartype);
      exit(0);
    }
    
  }
  actmenu->no++;
}

void DefineGraph(int vartype,char *Name)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=vartype;
  switch (vartype){ /* Not really necessary.Only needed for undefined types */
  case curve2d_ :
  case graph2d_ :
  case contour2d_ :
  case contour3d_ :
    actmenu->mydata[actmenu->no].md.choice.mygraph=&(graph[graphactive]);
    break;
  default: {
      printf("menu.c:DefineGraph: Error, graph type %i "
	     "not defined. Exiting...",vartype);
      exit(0);
    }
    
  }
  actmenu->no++;
}

void DefineInt(char *Name,int *myint)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=int_;
  actmenu->mydata[actmenu->no].md.choice.myint.i=myint;
  actmenu->mydata[actmenu->no].md.choice.myint.exp=1;
  actmenu->no++;
}

void DefineLong(char *Name, long *mylong)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=long_;
  actmenu->mydata[actmenu->no].md.choice.mylong.i=mylong;
  actmenu->mydata[actmenu->no].md.choice.mylong.exp=1;
  actmenu->no++;
}

void DefineBool(char *Name,int *mybool)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=boolean_;
  actmenu->mydata[actmenu->no].md.choice.myint.i=mybool;
  actmenu->no++;
}

void DefineMod(char *Name,int *myint,int mod)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=mod_;
  actmenu->mydata[actmenu->no].md.choice.mymod.i=myint;
  actmenu->mydata[actmenu->no].md.choice.mymod.mod=mod;
  actmenu->no++;
}

void DefineDouble(char *Name,double *mydouble)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=double_;
  actmenu->mydata[actmenu->no].md.choice.mydouble.d=mydouble;
  if (*mydouble==0)
    actmenu->mydata[actmenu->no].md.choice.mydouble.exp=0;
  else
    actmenu->mydata[actmenu->no].md.choice.mydouble.exp=log10(fabs(*mydouble));
  actmenu->no++;
}

void DefineFloat(char *Name,float *myfloat)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=float_;
  actmenu->mydata[actmenu->no].md.choice.myfloat.d=myfloat;
  if (*myfloat==0)
    actmenu->mydata[actmenu->no].md.choice.myfloat.exp=0;
  else
    actmenu->mydata[actmenu->no].md.choice.myfloat.exp=log10(fabs(*myfloat));
  actmenu->no++;
}

void DefineString(char *Name,char *mystring,int len)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=string_;
  actmenu->mydata[actmenu->no].md.choice.mystring.s=mystring;
  actmenu->mydata[actmenu->no].md.choice.mystring.len=len;

  actmenu->no++;
}

void DefineIntArr(char *Name,int *myint,const int max)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=int_arr;
  actmenu->mydata[actmenu->no].md.choice.myint_arr.i=myint;
  actmenu->mydata[actmenu->no].md.choice.myint_arr.no=0;
  actmenu->mydata[actmenu->no].md.choice.myint_arr.max=max;
  actmenu->no++;
}

void DefineIntArrp(char *Name,int *myint,int *max)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=int_arrp;
  actmenu->mydata[actmenu->no].md.choice.myint_arrp.i=myint;
  actmenu->mydata[actmenu->no].md.choice.myint_arrp.no=0;
  actmenu->mydata[actmenu->no].md.choice.myint_arrp.max=max;
  actmenu->no++;
}

void DefineDoubleArr(char *Name,double *mydouble,const int max)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=double_arr;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arr.d=mydouble;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arr.no=0;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arr.max=max;
  actmenu->no++;
}

void DefineDoubleArrp(char *Name,double **mydouble,int *max)
{
  CheckMenuMemory(actmenu);
  actmenu->mydata[actmenu->no].md.name= (char *) malloc((strlen(Name)+1)*sizeof(char)); strcpy(actmenu->mydata[actmenu->no].md.name,Name);
  actmenu->mydata[actmenu->no].md.mytype=double_arrp;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arrp.d=mydouble;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arrp.no=0;
  actmenu->mydata[actmenu->no].md.choice.mydouble_arrp.max=max;
  actmenu->no++;
}


/*unit to destroy a menu and all its sub menus*/
void DestroyMenu(menudat *menu){
  int i;

  for (i=0;i<menu->no;i++) 
    if (menu->mydata[i].md.mytype==menu_)
      DestroyMenu(menu->mydata[i].md.choice.mymenu);
  free(menu->mydata);
  free(menu);
}


/*****************************************?*/
/*
  The definitions of windows corresponding to the menu structures.
 */

void redefinewindow(mywindow *mw,int *wc,menudat *md){
    /*  if (mw->menuw!=NULL) free(mw->menuw);*/
  /*  for (i=0;i<mw->nomenu;i++) free(mw->menutext[i]);*/
}

void destroywindow(mywindow *mw,int *wc,menudat *md){
    /*  if (mw->menuw!=NULL) free(mw->menuw);*/
  /*  for (i=0;i<mw->nomenu;i++) free(mw->menutext[i]);*/
}



/*******************8**********************/

/*
   This will lead to a general menu action according to the menu items.
   We will also need to generalize the graphic options.
      All Graphic options should create their own menus. Therefore
      they should be associated with a define-menu option that is 
      automatically called if such a graphics is requested.
   Most of all there will need to be a more general data analysis. I 
   should think about how to write a code that only needs to be changed
   at a few places by the normal user for extensions like their own
   initial conditions, their own algorithms and so on...

   Security: I should install a little subroutine that sends a mail to me
             at random times, maybe once every one hundered times the 
	     program is called, so that I can keep track of the number
	     of users.
   */



