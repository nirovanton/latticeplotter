# ifndef _VIEW3D_H
# define _VIEW3D_H

# include <vector.h>
# define xaxis 1
# define yaxis 2
# define zaxis 3	
# define Gobjectnum 30000
# define V3DOBJECT 0
# define V3DPOLYHEDRON 1
# define V3DSPRITE 2
# define MESH 4
# define SQUAREMESH 5
# define MESHONESIDED 6
# define SQUAREMESHONESIDED 7
# define MESHTOGGLE 8
# define TEXTOBJECT 9

typedef struct {
    short x, y;
} DPoint;

struct objectstruct{
  matp    mc2wcm,     /* Matrix $M$ zur Transf. von Ortsvektoren */
          mc2wcmn;    /* Matrix zur Transf. von Normalen */
  
  vecp    mc2wcv,     /* Vektor $v$ zur Verschiebung von Ortsvektoren */
                      /* $\vec x_{wc}=M\cdot \vec x_{mc} +\vec v $ */
          posmc,      /* Position des geom. Zentrums in Modellkoordinaten */
          poswc;      /* Position des geom. Zentrums in Weltkoordinaten */
  vec     *va,        /* Array von Meshpunkten fuer V3DPOLYHEDRON */
          *na,        /* Array von Normalenvektoren fuer V3DPOLYHEDRON */
          *tva;       /* Array von transformierten Ortsvektoren s.o.*/
  
  int     type,       /* zB V3DPOLYHEDRON oder V3DSPRITE oder V3DMESH */
          *polygons,  
/* Polygons ist eine integer - Liste mit folgendem
   Aufbau: beginnend mit polygons[0] sind nacheinander
   die Daten $n_o, i_{norm}, color, v_1, .., v_{n_o}, n_1, .. $ abgelegt, mit
   $n_o=$ Anzahl der Ecken, $i_{norm}=$ Index des Normalenvektors, 
   color = Polygonfarbe,
   $v_j=$ Index des Ortsvektors der j-ten Ecke. Das Array wird durch 
   eine 0 beendet. */
           *transfv,   /* Array von Booleans: Vektor n transformiert j/n? */
           van;        /* Anzahl der Ortsvektoren in va[] (bzw tva) */	
  
  double   size;       /* Groesse des Objektes in Weltkoordinaten */
                       /* nuetzlich fuer Sprites in Zus. mit wc2dcfakt() */ 
  
  DPoint  *points;    /* Auf Displaykoordinaten transformierte Punkte */
  void    (*drawstruct)(struct objectstruct *o);
}; 

typedef struct objectstruct Gobject;
    
extern int getview3dhandle();
extern int newframe(int handle);
extern Gobject *initGobjects(int n);
extern void resetGobjects(void);
extern int insertGobject(Gobject *G);
extern void DRAW(void);
extern void setviewglobal (int xsize, int ysize, int f, int zshift);
extern void setscreendimensions(int xsize, int ysize);
extern void rotate(int axis, double angle);
extern void translate(double x, double y, double z);
extern void scale(double sx, double sy, double sz);
extern void pushtrafs(void);
extern void poptrafs(void);
extern void copytrafs(void);
extern vecp mc2wc (vecp v);
extern vecp mc2wcnorm (vecp v);
extern matp getmc2wcm(void);
extern vecp getmc2wcv(void);
extern matp getmc2wcmn(void);

extern double sizeinwc(double sizeinmc);
extern int  wc2dcfakt (short int *x, short int *y, double *f, vecp v);
extern int  wc2dc (short int *x, short int *y, vecp v);
extern void wc2dc_double (double *x, double *y, vecp v);

# endif /* _VIEW3D_H */



