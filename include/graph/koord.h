#ifndef KOORD_H
#define KOORD_H

#include<stdlib.h>
#include<math.h>


struct _tic{
  double pos;
  char *name;
};

typedef struct _tic tic;

extern void Koordinatenachse(double xmin, double xmax,tic **t,
			     char *expstr, int *anz, int zwischentics);
extern void KoordinatenachseLog(double xmin, double xmax,tic **t,
				char *expstr, int *anz, int zwischentics);


#endif
