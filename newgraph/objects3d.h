#ifndef _OBJECTS3D_H
#define _OBJECTS3D_H

#include <vector.h>
#include <view3d.h>
#include <mydraw.h>

extern void linie_3d(int c,vecp v1,vecp v2);
extern void text_3d(int c,vecp v1p,vecp v2p,char *str,char *ref,double size);
extern void flaeche_3d(int c,vecp rp, vecp v1p,vecp v2p,int n1, int n2);

#endif
