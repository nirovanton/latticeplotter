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

e-mail: Alexander.Wagner@ndsu.edu
\***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
# include	<X11/Xlib.h>
# include	<X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include "mygraph.h"
#include "menu.h"
#include "neuzeichnen.h"

/*static char initload[200];*/

static Display	     *myd;

static mywindow *windows=NULL;
static int nowindows=-1/*,maxwindows=0*/;

static grstr *debug_graph;

/* This index stuff should not be here! */

static int xsize=0,ysize=0;

void DefineDebugGraph(grstr *graph){ debug_graph=graph;}
void DebugGraph(){
  int i;
  printf("DebugGraph: graph=%p\n",debug_graph);
  printf("AnzNxN_R=%i",debug_graph->anzNxN_R);
  for (i=0;i<debug_graph->anzNxN_R;i++) printf("%p,",debug_graph->pNxN_R[i]);
  printf("\n");
}

long ind(int x, int y)
{
  x=x%xsize;
  if (x<0) x+=xsize;
  y=y%ysize;
  if (y<0) y+=ysize;
  return x*ysize+ysize;
}

indfn getind() {return ind;}

void DrawGraphs()
{
  int i;
  
  /*  printf("DrawGraphs reached\n");*/
  for (i=0;(i<nowindows+1);i++){
    if (windows[i].installed&&(windows[i].type&zeichentypemask)&&
	windows[i].neu_zeichnenreq){
      windows[i].neu_zeichnen(myd,&windows[i],0,"",0,0,0,0,
			      *windows[i].menu->graph);
      windows[i].neu_zeichnenreq=0;
      
    }
  }      
}

void EventAction(XEvent	     myevent){
  static int  Shift=0,Ctrl=0;
  KeySym      mykey;
  int	      i,j;
  char	      text[100];
  menudat     *md;
    switch (myevent.type)
      {
      case Expose:
	for (i=0;(i<nowindows+1)
	       &&(myevent.xexpose.window!=windows[i].zeichenw);i++);
	if (i<nowindows+1)
	  {
	    if ((windows[i].newdraw==0)&&(windows[i].doublebuffer)) 
	      {
		setXwindow(myd,windows[i].zeichenw,windows[i].mypixmap,
			   windows[i].zeichengc,windows[i].doublebuffer,
			   windows[i].zeichenmaxx,windows[i].zeichenmaxy);
		myshow();
		XFlush(myd);
	      }
	    else 
	      {
		windows[i].neu_zeichnenreq=1;
		definerequests(windows[i].menu->graph,
			       windows[i].neu_zeichnentype,
			       windows[i].menu->zd);
	      }		
	  } 
	else
	  for (i=0;(i<nowindows+1);i++)
	    if (windows[i].installed)
	      {
		md=windows[i].menu;
		for (j=0;(j<windows[i].nomenu)
		       &&(windows[i].menuw[j]!=myevent.xexpose.window);j++);
		if (j< windows[i].nomenu)
		  {
		    switch (md->mydata[j].md.mytype){
		    case int_:
		      sprintf(windows[i].menutext[j],"%s = %i",
			      md->mydata[j].md.name,
			      *(md->mydata[j].md.choice.myint.i));
		      break;
		    case long_:  
		      sprintf(windows[i].menutext[j],"%s = %li",
			      md->mydata[j].md.name,
			      *md->mydata[j].md.choice.mylong.i);
		      break;
		    case double_:
		      sprintf(windows[i].menutext[j],"%s = %g",
			      md->mydata[j].md.name,
			      *md->mydata[j].md.choice.mydouble.d);
		      break; 
		    case boolean_:
		      if (*md->mydata[j].md.choice.myint.i!=0)
			sprintf(windows[i].menutext[j],"%s on",
				md->mydata[j].md.name);
		      else
			sprintf(windows[i].menutext[j],"%s off",
				md->mydata[j].md.name);
		      break;
		    }

		    zeichne_menutext(myd,windows[i].menuw[j],
				     windows[i].menugc,
				     windows[i].myfont,
				     windows[i].menutext[j]);
		    break;
		  }
	      }
	break;
	
      case ConfigureNotify:
	for (i=0;(i<nowindows+1)
	       &&(myevent.xconfigure.window!=windows[i].myw);i++);
	if (i<nowindows+1)
	  {
	    configurewindow(myd,&windows[i],myevent);
	    windows[i].newdraw=1;
	  }
	break;
	
      case MappingNotify:	  
	XRefreshKeyboardMapping (&myevent.xmapping);
	break;
	
	case ButtonPress:
	  for (i=0;(i<nowindows+1);i++)
	    {
	      for (j=0;(j<windows[i].nomenu)
		     &&(windows[i].menuw[j]!=myevent.xexpose.window);j++);
	      if (j< windows[i].nomenu) 
		windows[i].menuaction(&windows[i],j,
				      2*myevent.xbutton.x/
				      windows[i].menumaxx,
				      myevent.xbutton.button,
				      Shift,Ctrl);
	    }
	  break;
	  
      case KeyPress:
	
	i = XLookupString (&myevent.xkey, text, 10, &mykey, 0);
	switch (mykey){
	case XK_Shift_L:
	case XK_Shift_R:
	  Shift=1;
	  break;
	case XK_Control_L:
	case XK_Control_R:
	  Ctrl=1;
	  break;
	}
	/*	  if (i == 1 && text[0] == 'q')
		  done = 1;*/
	break;
      case KeyRelease:
	XLookupString (&myevent.xkey, text, 10, &mykey, 0);
	switch (mykey){
	case XK_Shift_L:
	case XK_Shift_R:
	  Shift=0;
	  break;
	case XK_Control_L:
	case XK_Control_R:
	  Ctrl=0;
	  break;
	}
	
      }
    
}

void DrawChangedValues(){
  /* Should this routine be migrated into menu.c? 
     The use of the menu types for writing might suggest so,
     but the window information is not available in menu.c.
     So I will leave it here for not.
  */

  /* This routine determines if any of the values have been 
     changed by the program and displays the new values
     accordingly. */

  /* Attention: In this and in the above routine string_ data types
     do not seem to be considered !!!Check this!!!! */

  menudat     *md;
  char text[1000];
  int i,j;

  for (i=0;(i<nowindows+1);i++){
    if (windows[i].installed){
      md=windows[i].menu;
      for (j=0;(j<windows[i].nomenu);j++){
	switch (md->mydata[j].md.mytype){
	case int_:
	  sprintf(text,"%s = %i",
		  md->mydata[j].md.name,
		  *(md->mydata[j].md.choice.myint.i));
	  break;
	case long_:  
	  sprintf(text,"%s = %li",
		  md->mydata[j].md.name,
		  *md->mydata[j].md.choice.mylong.i);
	  break;
	case double_:
	  sprintf(text,"%s = %g",
		  md->mydata[j].md.name,
		  *md->mydata[j].md.choice.mydouble.d);
	  break; 
	case int_arr:
	  sprintf(text,"%s[%i]=%i",md->mydata[j].md.name,
		  md->mydata[j].md.choice.myint_arr.no,
		  md->mydata[j].md.choice.myint_arr.i[md->mydata[j].md.choice.myint_arr.no]);
	  break;
	case int_arrp:
	  sprintf(text,"%s[%i]{%i}=%i",md->mydata[j].md.name,
		  md->mydata[j].md.choice.myint_arrp.no,
		  *md->mydata[j].md.choice.myint_arrp.max,
		  md->mydata[j].md.choice.myint_arrp.i[md->mydata[j].md.choice.myint_arrp.no]);
	  break;
	case double_arr:
	  sprintf(text,"%s[%i]=%10.5g",md->mydata[j].md.name,
		  md->mydata[j].md.choice.mydouble_arr.no,
		  md->mydata[j].md.choice.mydouble_arr.d[md->mydata[j].md.choice.mydouble_arr.no]);
	  break;
	case double_arrp:
	  if (*md->mydata[j].md.choice.mydouble_arrp.max>0)
	  sprintf(text,"%s[%i]{%i}=%10.5g",md->mydata[j].md.name,
		  md->mydata[j].md.choice.mydouble_arrp.no,
		  *md->mydata[j].md.choice.mydouble_arrp.max,
		  (*md->mydata[j].md.choice.mydouble_arrp.d)[md->mydata[j].md.choice.mydouble_arrp.no]);
	  else 
	  sprintf(text,"%s[%i]{%i}=N/A",md->mydata[j].md.name,
		  md->mydata[j].md.choice.mydouble_arrp.no,
		  *md->mydata[j].md.choice.mydouble_arrp.max);
	  break;
	case boolean_:
	  if (*md->mydata[j].md.choice.myint.i!=0)
	    sprintf(text,"%s on",
		    md->mydata[j].md.name);
	  else
	    sprintf(text,"%s off",
		    md->mydata[j].md.name);
	  break;
	default: sprintf(text,"%s",windows[i].menutext[j]);
	}
	if (0!=strcmp(windows[i].menutext[j],text)){
	  sprintf(windows[i].menutext[j],"%s",text);
	  zeichne_menutext(myd,windows[i].menuw[j],
			   windows[i].menugc,
			   windows[i].myfont,
			   windows[i].menutext[j]);
	}
      }
    }
  }
}




void Events(int newdata){
  XEvent      myevent;
  int	      i;

  if (nowindows+1<1) {
    printf("mygraph.c:Events():No windows have been defined. Missing one EndMenu() command?\n");
    exit(1);
  }
  if (newdata){
    for (i=0;i<nowindows+1;i++)
    {
      if ((windows[i].type&zeichentypemask) && (windows[i].installed!=0) ){
	windows[i].neu_zeichnenreq=1;
	definerequests(windows[i].menu->graph,
		       windows[i].neu_zeichnentype,
		       windows[i].menu->zd);
      }
    }
  }	

  
  while (XEventsQueued (myd,QueuedAfterReading)!=0){
    XNextEvent (myd, &myevent);
    if (myevent.type==Expose)
      while (XCheckWindowEvent(myd,myevent.xexpose.window,
			       ExposureMask,&myevent));
    EventAction(myevent);
  }
 
  DrawChangedValues();

}


void InitGraphics()
{
  fprintf(stderr,
	  "Graph library version 0.9, Copyright (C) 1992-2009 Alexander Wagner and Johannes Schlosser\n"
	  "The graph library comes with ABSOLUTELY NO WARRANTY;\n"
	  "it is distributed as free software under the GNU liscence.\n"
	  "For details look at the LICENSE file distributed with the program\n\n"
	  "For upgrades check the homepage: www.ndsu.edu/physics/people/faculty/wagner/graphics_library\n"
	  "You can contact the author at alexander.wagner@ndsu.edu\n");


  initfonts(&myd);
  if (myd!=NULL){
    DefineWindowInformation(&windows,&nowindows);
    setdisplay(myd);
  }
  SetColorRamp(NULL);
}

void CloseGraphics(){  
  int i;
  for (i=0;i<nowindows+1;i++) if(windows[i].installed)
    unmapwindow(myd,&windows[i]);
  XCloseDisplay (myd);
}











