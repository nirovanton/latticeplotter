#ifndef _MYDRAW_H
#define _MYDRAW_H

#include "view3d.h"
#include "mydraw_2d.h"
#include "cobject3d.h"
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "basicdef.h"


#define doublebufferX 1
#define TRIANGLES 1
#define SQUARES 2

extern void draw3Dline(Gobject *o);
extern void draw3Dtext(Gobject *o);
extern void drawtrianglelight(Gobject *o);
extern void drawpolyhedron(Gobject *o);
extern void drawflatmesh(Gobject *o);
extern void drawmeshsegment(Gobject *o);
extern void drawsquaremeshsegment(Gobject *o);
extern void calcnormals(Gobject *o, int mallocnormals);
extern void coloronmesh(Mesh* m,int lci,int cn);
extern void coloronmeshlim(Mesh* m,int lci,int cn,
		    double minl,double maxl,int col,
		    double zmin, double zmax);

#endif /* _MYDRAW_H */






