#ifndef _graph_3D_H
#define _graph_3D_H

#include <X11/Xlib.h>
#include "basicdef.h"
#include "graph.h"
#include "menu.h"
#include "color.h"
#include "analyse.h"

extern void Events(int newdata);
extern void EventAction(XEvent myevent);
extern void DrawGraphs();
extern void InitGraphics();
extern void CloseGraphics();

#endif









