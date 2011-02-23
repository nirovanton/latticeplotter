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
#include <string.h>
#include <stdlib.h>
#include "newgraph/objects3d.h"
#include "newgraph/view3d.h"

#define il(k) {\
 linien[k].type = V3DOBJECT;\
 linien[k].tva=malloc(2*sizeof(vec));\
 linien[k].poswc=vnew();\
 linien[k].drawstruct=draw3Dline;}

#define it(k) {\
 texte[k].type = V3DOBJECT;\
 texte[k].tva=malloc(sizeof(vec));\
 texte[k].poswc=vnew();\
 texte[k].drawstruct=draw3Dtext;}

#define i_f(k) {\
 flaechen[k].type = V3DOBJECT;\
 flaechen[k].tva=malloc(4*sizeof(vec));\
 flaechen[k].poswc=vnew();\
 flaechen[k].drawstruct=drawsimplesurfaceelement;}



void linie_3d(int c,vecp v1p,vecp v2p){
  int i;
  vec v1,v2;
  static int
    handle,
    initialized=0,
    maxnum=2000,
    aktnum=-1;
  static Gobject *linien,*linientmp;
  veq(v1,v1p);veq(v2,v2p);
  if (!initialized) {
    handle=getview3dhandle();
    linien=malloc(maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++) il(i)
    initialized=1;
  }
  if (newframe(handle)) aktnum=-1;
  if (++aktnum>=maxnum){
    linientmp=malloc(2*maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++)linientmp[i]=linien[i];
    free(linien);
    linien=linientmp;
    for (i=maxnum;i<maxnum*2;i++) il(i)
    maxnum*=2;
  }
  
  linien[aktnum].van=c;
  veq(linien[aktnum].tva[0],mc2wc(v1));
  veq(linien[aktnum].tva[1],mc2wc(v2));
  veq(linien[aktnum].poswc,sm(0.5,vsum(linien[aktnum].tva[0],
				       linien[aktnum].tva[1])));
  insertGobject(&(linien[aktnum]));
}



void text_3d(int c,vecp v1p,vecp v2p,char *str,char *ref,double size){
  int i;
  vec v1,v2,vnull={0.,0.,0.};
  static int
    handle,
    initialized=0,
    maxnum=2000,
    aktnum=-1;
  static Gobject *texte,*textetmp;
  veq(v1,v1p); if (v2p!=NULL) veq(v2,v2p);
  if (!initialized) {
    handle=getview3dhandle();
    texte=malloc(maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++) it(i)
    initialized=1;
  }
  if (newframe(handle)) aktnum=-1;
  if (++aktnum>=maxnum){
    textetmp=malloc(2*maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++)textetmp[i]=texte[i];
    free(texte);
    texte=textetmp;
    for (i=maxnum;i<maxnum*2;i++) it(i)
    maxnum*=2;
  }
  texte[aktnum].type=TEXTOBJECT;
  texte[aktnum].van=c;
  texte[aktnum].size=size;
  texte[aktnum].polygons=malloc(strlen(str)*sizeof(char)+1);
  sprintf((char *) texte[aktnum].polygons,"%s",str);
  texte[aktnum].transfv=(int *) ref; /* typecast to reuse pointer */
  veq(texte[aktnum].poswc,mc2wc(v1));
  if (v2p==NULL) veq(texte[aktnum].tva[0],vnull);
  else veq(texte[aktnum].tva[0],mc2wc(vsum(v2,v1)));
  insertGobject(&(texte[aktnum]));
}

void drawsimplesurfaceelement(Gobject *o){
int i;
  XPoint points[4];
  for (i=0;i<4;i++){ 
    wc2dc(&points[i].x,&points[i].y,o->tva[i]);
  }  
  mypolygon(o->van, points, 4);
}

void einfache_flaeche_wc_3d(int c, vecp vr, vecp v0p, vecp v1p,vecp v2p,vecp v3p){
  int i;
  static int
    handle,
    initialized=0,
    maxnum=1000,
    aktnum=-1;
  static Gobject *flaechen,*flaechentmp;

  if (!initialized) {
    handle=getview3dhandle();
    flaechen=malloc(maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++) i_f(i)
    initialized=1;
  }
  if (newframe(handle)) aktnum=-1;
  if (++aktnum>=maxnum){
    flaechentmp=malloc(2*maxnum*sizeof(Gobject));
    for (i=0;i<maxnum;i++)flaechentmp[i]=flaechen[i];
    free(flaechen);
    flaechen=flaechentmp;
    for (i=maxnum;i<maxnum*2;i++) i_f(i)
    maxnum*=2;
  }
  flaechen[aktnum].van=c;
  veq(flaechen[aktnum].poswc,vr);
  veq(flaechen[aktnum].tva[0],v0p);
  veq(flaechen[aktnum].tva[1],v1p);
  veq(flaechen[aktnum].tva[2],v2p);
  veq(flaechen[aktnum].tva[3],v3p);
  insertGobject(&(flaechen[aktnum]));
}





void flaeche_3d(int c,vecp rp, vecp v1p,vecp v2p,int n1, int n2){
  int i,j;
  vec r,r2,v1,v2;
  veq(r,mc2wc(rp));
  veq(v1,sm(1.0/n1,vdiff(mc2wc(vsum(rp,v1p)),r)));
  veq(v2,sm(1.0/n2,vdiff(mc2wc(vsum(rp,v2p)),r)));
/*  veq(v1,mc2wcnorm(v1p)));
  veq(v2,sm(1.0/n2,mc2wcnorm(v2p)));*/
  veq(r2,vsum(r,sm(0.5,vsum(v1,v2))));
  for (i=0;i<n1;i++)
     for (j=0;j<n2;j++)
       einfache_flaeche_wc_3d(c,
			      vsum(r2,vsum(sm(1.0*i,v1),sm(1.0*j,v2))),
			      vsum(r,vsum(sm(1.0*i,v1),sm(1.0*j,v2))),
			      vsum(r,vsum(sm(1.0*(i+1),v1),sm(1.0*j,v2))),
			      vsum(r,vsum(sm(1.0*(i+1),v1),sm(1.0*(j+1),v2))),
			      vsum(r,vsum(sm(1.0*i,v1),sm(1.0*(j+1),v2))));
}





/*#define wuerfelmax 100
void wuerfel(){
  static  mat m[wuerfelmax],mn[wuerfelmax];
  static  Gobject G[wuerfelmax];
  static  vec 
  	  mc2wcv[wuerfelmax],
  	  poswc[wuerfelmax],
	  va[]={{0,0,0},
	  	{0,1,0},
	  	{1,1,0},
	  	{1,0,0},
	  	{0,0,1},
	  	{0,1,1},
	  	{1,1,1},
	  	{1,0,1}},
	  cent= {0.5,0.5,0.5},	
	  tva[8]; 
  static  int 
	  polygons[]=
		{4,0,20,0,1,2,3,
		 4,0,20,0,1,5,4,
		 4,0,20,0,3,7,4,
		 4,0,20,3,2,6,7,
		 4,0,20,1,2,6,5,
		 4,0,20,4,7,6,5,0}, 
          tr[8]=
	        {0,0,0,0,0,0,0,0},
	  aktwuerfel=0,      
	  initialized=0;      
	int i;
	if (!initialized){
	  for (i=0;i<wuerfelmax;i++){          	 
  	    G[i].va=va;G[i].tva=tva;G[i].polygons=polygons;
 	    G[i].mc2wcm=m[i];G[i].mc2wcmn=mn[i];
	    G[i].transfv=tr;G[i].van=8;
	    G[i].type=V3DPOLYHEDRON;
	    G[i].drawstruct=drawpolyhedron;
	    G[i].posmc=cent; 
            G[i].mc2wcv=mc2wcv[i];
            G[i].poswc=poswc[i];
            calcnormals(&G[i],1);
          }
	  initialized=1;
	}  
	if (newframe()) aktwuerfel=0;
	if (aktwuerfel<wuerfelmax){
          pushtrafs();
	    translate(-0.5,-0.5,-0.5);
	    veq(G[aktwuerfel].poswc,mc2wc(cent));
	    insertGobject(&G[aktwuerfel++]);
	  poptrafs();  
	}
}
*/

/*void Koordinatensystem(void){
  int i,j;
  pushtrafs();
    for(i=0;i<4;i++){
      pushtrafs();
        rotate(yaxis,i*90.);
        translate(-.5,-.5,-.5);
        pushtrafs();
          scale(.02,.02,.02);
          wuerfel();
          translate(0.,50.,0.);
          wuerfel();
        poptrafs();
      poptrafs();
    }
  poptrafs();
}*/

