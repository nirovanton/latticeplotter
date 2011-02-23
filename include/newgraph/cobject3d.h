#ifndef _COBJECT3D_H
#define _COBJECT3D_H

/* cobject3d realisiert komplexe 3D Objekte, wie Meshes und Contourflaechen,
   bei denen zwischen Initialisierung des Objektes und der Darstellung 
   unterschieden werden soll. Diese Objekte sind jeweils nur einfach in einer
   Szenerie vorgesehen, im Gegensatz zu den 3D Primitiven aus der objects3d
   Bibliothek. */


#include <view3d.h>
#include <mydraw_2d.h>
#include <mydraw.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


#define doublebufferX 1
#define TRIANGLES 1
#define SQUARES 2
/*
#define MESH           (1<<0)
#define SQUARES        (1<<1)
#define TRIANGLES      (0<<1)
#define ONESIDED       (1<<2)
#define TWOSIDED       (0<<2)

#define SINGLEOBJECT   (0<<0)
#define SPRITE         (1<<1)
#define                (1<<3)
#define                (1<<4)
#define                (1<<5)
#define                (1<<6)
#define FILLED         (1<<7)
*/

/* Datenstrukturen und Header Mesh-bezogener Funktionen */


struct meshstruct{
    mat mc2wcm;
    mat mc2wcmn;
    vec mc2wcv;
    vec *va;
    vec *na;
    vec *tva;
    DPoint *points;
    Gobject *Garray;
    int xdim;
    int ydim;
    int mode;
    int *pol;
    int *transf2dc;
    int normalsinitialized;
};

typedef struct meshstruct Mesh;    
typedef Mesh* Meshp;


extern void coloronmesh(Mesh* m,int lci,int cn);
extern void coloronmeshlim(Mesh* m,int lci,int cn,
		    double minl,double maxl,int col,
		    double zmin, double zmax);
extern Mesh *initmesh(int xdim, int ydim, int mode);
extern void destroymesh(Mesh *m);
extern void putinmesh(Mesh* mymesh, int xind,int yind, vecp v);
extern void putinmeshz(Mesh* mymesh, int xind,int yind, double z);
extern double getmeshz(Mesh* m,int xind, int yind);
extern void mesh(Mesh* mymesh);
extern void togglemesh(void);

/* Datenstrukturen und Header Contourflaechen-bezogener Funktionen */

struct contourstruct{
    mat mc2wcm;
    mat mc2wcmn;
    vec mc2wcv;
    vec *va;
    vec *na;
    vec *tva;
    vec *sp;
    DPoint *points;
    Gobject *Garray;
    int vaakt;
    int vamax;
    int polakt;
    int polmax;
    int *pol;
/*    int *transf2dc; */
    int initialized;
};

typedef struct contourstruct Contour;    
typedef Contour* Contourp;

extern Contour *initcontour(int dummy);
extern void destroycontour(Contour *c);
extern int putpointintocontour(Contour* mycontour, vecp v);
extern void puttriangleincontour(Contour* mycontour, 
                                 int ind1, int ind2, int ind3, int basecolor);
extern void contour3d(Contour* mycontour);

#endif /* _COBJECT3D_H */






