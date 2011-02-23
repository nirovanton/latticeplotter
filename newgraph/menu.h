#ifndef MENU_H
#define MENU_H

/* This makes the menu part of the graphics dependent on the X system.
It would be nice if this could be capsuled away in some device
independent fashion for generalization to Mac or Microsoft
environments. */


#include <X11/Xlib.h> 
#include <X11/Xutil.h>
/*#include <stdarg.h>*/
#include "basicdef.h"
#include "mydraw.h"
#include "graph.h"


typedef void (*fp)(void);

/* Masks for Window types. */
#define zeichentypemask 1
#define menutypemask 2


/* Definition of menu types */
#define NORMAL 0
#define CHOICE 1

/* Definition of parameter types */
#define int_         0
#define long_        1
#define mod_         2
#define double_      3
#define boolean_     4
#define switch_      5
#define menu_        6
#define protofunc_   7
#define graph2d_     8
#define contour2d_   9
#define contour3d_  10
#define curve2d_    11
#define newdraw_    12
#define print_      13
#define doprint_    14
#define close_      15
#define float_      16
#define function_   17
#define string_     18
#define boolmenu_   19
#define int_arr     20
#define int_arrp    21
#define double_arr  22
#define double_arrp 23

/* Should be shared with the ZPL program to avoid wrong referencing!*/

struct menudat_;
struct boolmenudat_;

typedef void (*myfuncp)(void);

typedef struct intstr_{
  int *i;
  int exp;
} intstr;

typedef struct longstr_{
  long *i;
  long exp;
} longstr;

typedef struct doublestr_{
  double *d;
  int    exp;
} doublestr;

typedef struct floatstr_{
  float *d;
  int    exp;
} floatstr;

typedef struct stringstr_{
  char *s;
  int    len;
} stringstr;

typedef struct boolmenudat_ {
  int *b; 
  struct menudat_ *bm;
} boolmenudat;

typedef struct modstr_{
  int    *i;
  int    mod;
} modstr;

typedef struct int_arr_str_{
  int *i;
  int    no;
  int    max;
} int_arr_str;

typedef struct int_arrp_str_{
  int *i;
  int    no;
  int    *max;
} int_arrp_str;

typedef struct double_arr_str_{
  double *d;
  int exp;
  int    no;
  int    max;
} double_arr_str;

typedef struct double_arrp_str_{
  double **d;
  int exp;
  int    no;
  int    *max;
} double_arrp_str;

typedef union mchoice_ {
  intstr myint;
  longstr mylong;
  modstr mymod;
  doublestr mydouble;
  floatstr myfloat;
  stringstr mystring;
  int_arr_str myint_arr;
  int_arrp_str myint_arrp;
  double_arr_str mydouble_arr;
  double_arrp_str mydouble_arrp;
  myfuncp myfunc;
  struct boolmenudat_ boolmenu;
  struct menudat_ *mymenu;
  grstr *mygraph;
} mchoice;

typedef struct menudatastruct_ {
  char *name;
  int mytype;
  mchoice choice;
} menudatastruct;

typedef struct choicedatastruct_{
  char *name;
  int *flag;
  void (*func)(void);
} choicedatastruct;

typedef union datastruct_{
  menudatastruct md;
  choicedatastruct cd;
} datastruct;


typedef struct menudat_ {
  char *menuname; 
  struct menudat_ *parentmenu;
  struct mywindowstruct *winp;
  nzdata  *zd;
  grstr *graph;
  int installed;
  int menutype;
  int no; 
  int maxno;
  int actno;
  datastruct *mydata;
} menudat;



typedef struct mywindowstruct{
  int number;
  int type; 
  menudat *menu;
  int installed;
  int newdraw;
  int doublebuffer;
  int nomenu;
  double MenuRatio;
  int menumaxx;
  int menumaxy;
  int zeichenmaxx;
  int zeichenmaxy;
  char **menutext;
  Display *myd;
  Drawable myw;
  Drawable zeichenw;
  Drawable mypixmap;
  Drawable *menuw;
  GC menugc;
  GC zeichengc;
  XFontStruct *myfont;
  int *menulength;
  char *Name;
  int neu_zeichnenreq; /* requests data to be made available in prog. */
  int neu_zeichnentype;/* specifies type of neu_zeichnen data so that 
                          get request can be processed. */
  void (*neu_zeichnen)(Display*,struct mywindowstruct*,int,char*,
		       double, double, int, int,grstr);
  void (*menuaction)(struct mywindowstruct*,int,int,int,int,int);
} mywindow;
/*
typedef struct mywindowstruct_ {
  int nowindows;
  int maxnowindows;
  mywindow *mwp;
} mywindowstruct;
*/

extern void zeichne_menutext(Display *myd,Window w,GC mygc,
			     XFontStruct *font,char *str);
extern void unmapwindow(Display *myd,mywindow *mw);


extern void StartMenu(char *Name,int installed);
extern void StartBoolMenu(char *Name,int *b);
extern void EndMenu();

void DefineFunction(char *Name,fp myfp);

extern void DefineItem(int vartype);
extern void DefineInt(char *Name,int *myint);
extern void DefineLong(char *Name,long *myint);
extern void DefineMod(char *Name,int *myint,int mod);
extern void DefineBool(char *Name,int *myint);
extern void DefineDouble(char *Name,double *mydouble);
extern void DefineFloat(char *Name,float *myfloat);
extern void DefineString(char *Name,char *mystring,int len);
extern void DefineIntArr(char *Name,int *myint,int max);
extern void DefineIntArrp(char *Name,int *myint,int *max);
extern void DefineDoubleArr(char *Name,double *mydouble,int max);
extern void DefineDoubleArrp(char *Name,double **mydouble,int *max);
extern void DefineGraph(int vartype, char *Name);

extern void configurewindow(Display *myd,mywindow *mw,XEvent myevent);
extern void initfonts(Display **myd);

extern void DefineWindowInformation(mywindow **mw_,int *wc_);


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

#endif


