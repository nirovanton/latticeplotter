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

e-mail: Alexander.Wagner@ndsu.nodak.edu
\***************************************************************************/
#include "mydraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

# define MAXPOINTS 100

static int
    toggle= 1;


#define TV(o,n) if (!o->transfv[n]){veq (o->tva[n],vsum(matvecmul(o->mc2wcm,o->va[n]),o->mc2wcv));o->transfv[n]=1;}


void draw3Dline(Gobject *o){
  short int x1,y1,x2,y2;
  wc2dc(&x1,&y1,o->tva[0]);
  wc2dc(&x2,&y2,o->tva[1]);
  mydrawline(o->van,x1,y1,x2,y2);
}

void draw3Dtext(Gobject *o){
  short int d1,d2;
  double x,y,x1,y1,x2,y2;
  double f,a;
  wc2dcfakt(&d1,&d2,&f,o->poswc);
  wc2dc_double(&x,&y,o->poswc);
  if ((o->tva[0][0]==0.)&&(o->tva[0][1]==0.)&&(o->tva[0][2]==0.))
    a=0.;
  else{
    wc2dc_double(&x1,&y1,o->tva[0]);
    if (x1>=x){
      x2=x1;x1=x;y2=y1;y1=y;
    } else {x2=x;y2=y;}
    if ((x2-x1)==0) a=-90.;
    else a=-atan((y2-y1)/(x2-x1))*180/M_PI;
  }
  mysetfontdirection(a);
  myselectfont(0,(char *) o->transfv,o->size*f);
  mytext(o->van,x,y,(char *) o->polygons,1);
}


void drawpolyhedron(Gobject *o){

  int    edges,i,
         *p; /* Zum schnellen Zugriff auf o->Polygons */
         
  unsigned long c;       
       
  XPoint points[MAXPOINTS];     
  
  p = o->transfv;
  for (i=0;i<o->van;i++) *(p+i)=0;   /* $\vec x_i$ transfomiert = FALSE */
  p = o->polygons;
  while ((edges = *(p++))!=0){
    TV(o,*(p+2))
    if (sp(o->tva[*(p+2)], matvecmul(o->mc2wcmn,o->na[*(p++)]))<0){
      c=*(p++);
      for (i=0;i<edges;i++){
        TV(o,*p)
        wc2dc(&points[i].x,&points[i].y,o->tva[*(p++)]);
      }
      mypolygon_line(c, 0, points, edges);
    } else p += 1 + edges;
  }
}

void drawflatmesh(Gobject *o){

  int    edges,i,
         *p; /* Zum schnellen Zugriff auf o->Polygons */
         
  unsigned long c;       
       
  XPoint points[MAXPOINTS];     
  
  for (i=0;i<o->van;i++){ 
    veq (o->tva[i],vsum(matvecmul(o->mc2wcm,o->va[i]),o->mc2wcv));
    wc2dc(&o->points[i].x,&o->points[i].y,o->tva[i]);
  }  
  p = o->polygons;
  while ((edges = *(p++))!=0){
    p++;          /* Normalenindex wird uebersprungen */
    c=*(p++);
    for (i=0;i<edges;i++){
      points[i].x=o->points[*p].x;
      points[i].y=o->points[*(p++)].y;
    }  
    mypolygon_line(c, 0, points, edges);
  }  
}

/* 
   Die function drawmeshsegment zeichnet stets zwei Dreiecke. Es ist zu 
   unterscheiden zwischen, mit Oberflaechennormalen ausgestatteten, einseitigen
   Meshes und beidseitigen Meshes. Einseitige Meshes enthalten in ihrem 
   Polygonarray explizit die Daten:
   $$ 3,i_{norm},color,i_{x_1},i_{x_2},i_{x_4},3,i_{norm},color,i_{x_2},i_{x_3},i_{x_4}$$
   Es werden also, jeweils in Abh. von $i_{norm}$ nacheinander beide Dreiecke
   gemalt. Durch den Normalentest sind Fehler in der Darstellung kaum moeglich.
   Zweiseitige Meshes stellen Vor- und Rueckseite dar. Es entfaellt deshalb der
   Normalentest, wodurch erhebliche Rechenzeit gespart werden kann.
   Es sind wieder zwei Unterfaelle zu unterscheiden: 
   Fall 1: Die Daten sind abgelegt wie oben. Die ermoeglicht verschiedene Farbwerte
           fuer die beiden Dreiecke. Dieser Fall ist zu erkennen an der 3 am Anfang
           des polygon - Arrays. 
   Fall 2: Die Daten sind abgelegt in der Form:
   $$ 4,i_{norm},color,i_{x_1},i_{x_2},i_{x_3},i_{x_4}$$
           aus diesen vier Koordinaten koennen auf zwei verschiedene Arten Dreiecke
           extrahiert werden. Sie haben jedoch in jedem Fall den gleichen Farbwert.
   Um die richtige Verdeckung der beiden Dreiecke zu gewaehrleisten, werden die
   z-Komponenten der Weltkoordinaten der jeweils nur zu einem der Dreiecke gehoerenden
   Ecken miteinander verglichen.
   Prinzipiell sind Fehler in der Darstellung beim beidseitigen Modus moeglich. Um hier 
   einfache Abhilfe im interaktiven Modus zu schaffen, ist das Toggeln zwischen 
   den beiden in Fall 2 erwaehnten Auswahlmoeglichkeiten der Dreiecke vorgesehen. 
*/       
   




void drawmeshsegment(Gobject *o){
register int i,*p;
int c,j;
XPoint points[3];
DPoint *op;
  p=o->polygons;
  op=o->points;
  if (o->type==MESHONESIDED){
    for(j=0;j<2;j++){
      if (sp(o->tva[*(p+3)], matvecmul(o->mc2wcmn,o->na[*(p+1)]))<0){
        p+=2;
        c=*(p++);
        for (i=0;i<3;i++){
          points[i].x=op[*(p)].x;
          points[i].y=op[*(p++)].y;
        }      
        mypolygon_line(c, 0, points, 3);
      }  
    }
  } else if (o->type==MESH){
    if(*p==3)
      {
	if (o->tva[*(p+3)][2]>o->tva[*(p+9)][2]){
	  for(j=0;j<2;j++){
	    p+=2;
	    c=*(p++);
	    for (i=0;i<3;i++){
	      points[i].x=op[*(p)].x;
	      points[i].y=op[*(p++)].y;
	    }      
	    mytriangle_2(c, 1, points);
	  }  
	}
	else 
	  {
	    p+=6;
	    for(j=0;j<2;j++){
	      p+=2;
	      c=*(p++);
	      for (i=0;i<3;i++){
		points[i].x=op[*(p)].x;
		points[i].y=op[*(p++)].y;
	      }      
	      mytriangle_2(c, 1, points);
	      p-=12;
	    }  
	  }  
    } else if (*p==4){
      int i1=0,i2=1,i3=2,i4=3;
      if (toggle){i1=1;i2=2;i3=3;i4=0;}
      p+=2;
      c=*(p++);
      if (o->tva[*(p+i1)][2]>o->tva[*(p+i3)][2]){
        int dum;
        dum=i1;i1=i3;i3=dum;
        dum=i2;i2=i4;i4=dum;
      }  
      points[0].x=op[*(p+i1)].x;
      points[0].y=op[*(p+i1)].y;
      points[1].x=op[*(p+i2)].x;
      points[1].y=op[*(p+i2)].y;
      points[2].x=op[*(p+i4)].x;
      points[2].y=op[*(p+i4)].y;
      mypolygon_line(c, 0, points, 3);
      points[0].x=op[*(p+i2)].x;
      points[0].y=op[*(p+i2)].y;
      points[1].x=op[*(p+i3)].x;
      points[1].y=op[*(p+i3)].y;
      points[2].x=op[*(p+i4)].x;
      points[2].y=op[*(p+i4)].y;
      mypolygon_line(c, 0, points, 3);
    }
  }     
}

void drawsquaremeshsegment(Gobject *o){
int draw=1,c;
register int*p,i;
XPoint points[4];
  p=o->polygons;
  if (o->type == SQUAREMESHONESIDED)
    if (sp(o->tva[*(p+3)], matvecmul(o->mc2wcmn,o->na[*(p+1)]))>=0)
      draw=0;
  if (draw){
    p+=2;
    c=*(p++);
    for (i=0;i<4;i++){
      points[i].x=o->points[*(p)].x;
      points[i].y=o->points[*(p++)].y;
    }      
    mypolygon_line(c, 0, points, 4);
  }
}

void drawtrianglelight(Gobject *o){
int c;
register int*p,i;
XPoint points[3];
  p=o->polygons;
  p+=2;
  c=*(p++); /* Hier normalenabhängigen Farboffset berechnen */
  for (i=0;i<3;i++){
    points[i].x=o->points[*(p)].x;
    points[i].y=o->points[*(p++)].y;
  }      
  mypolygon_line(c, 0, points, 3);
}





