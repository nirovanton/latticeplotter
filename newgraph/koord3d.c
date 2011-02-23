#include <string.h>
#include <koord.h>
#include <koord3d.h>
#include <vector.h>
#include <view3d.h>
#include <mydraw.h>
#include <objects3d.h>


void zeichneachse(int c,tic *tics,int ntic,int beschrifte,double textabstand,
		  char *label, double laenge){
  int i;
  double gr,textsize;
  static char referenz[]="0.001";
  vec
    vnull={0.,0.,0.},
    vx={1.,0.,0.},
    vy={0.,1.,0.},
    vz={0.,0.,1.},
    nimmv,vtic,v1,v2;

    if (ntic==0) {printf("ntic=0 in koord3d:zeichneachse\n");return;}
  vx[0]=laenge;
  veq(v1,mc2wc(vnull));
  textsize=sizeinwc(0.15);
  if (fabs(sp(vdiff(mc2wc(vy),v1),vz))>fabs(sp(vdiff(mc2wc(vz),v1),vz)))
    veq(nimmv,vz);
  else
    veq(nimmv,vy);
  veq(vtic,sm(tics[0].pos,vx));
  linie_3d(c,vnull,vtic);
  for (i=0;i<ntic-1;i++){
    if (tics[i].name!=NULL) {
      gr=0.06;
      if (beschrifte){
	text_3d(c,vsum(vtic,sm(textabstand,nimmv)),
		NULL,tics[i].name,referenz,textsize);
      }
    }
    else gr=0.03;
    linie_3d(c,vsum(vtic,sm(gr,nimmv)),vtic);
/*    veq(v1,vtic);
    veq(v2,sm(tics[i+1].pos,vx));
    linie_3d(c,vtic,v2);*/
    linie_3d(c,vtic,sm(tics[i+1].pos,vx));

    veq(vtic,sm(tics[i+1].pos,vx));
  }
  if (tics[ntic-1].name!=NULL){
    gr=0.06;
    if (beschrifte){
      text_3d(c,vsum(vtic,sm(textabstand,nimmv)),
	      NULL,tics[i].name,referenz,textsize);
    }
  }
  else gr=0.03;
  veq(v2,vsum(vtic,sm(gr,nimmv)));
  linie_3d(c,v2,vtic);
  linie_3d(c,vtic,vx);
  if (beschrifte) text_3d(c,vsum(sm(0.5,vx),sm(3*textabstand,nimmv)),
			  vx,label,referenz,1.3*textsize);
}



void Koordinatensystem3D(int c,double xmin, double xmax,
                    double ymin, double ymax,
                    double zmin, double zmax,
		    double sx, double sy, double sz,	 
                    char *xstr, char *ystr, char *zstr,
                    char *xstru, char *ystru, char *zstru)
{
  tic *tics[3];
  char label[3][30];
  int ntic[3];
  
  char strexp[3][15];
  static vec 
         ecken[8]={{0.0,0.0,0.0},
		     {1.0,0.0,0.0},
                   {1.0,0.0,1.0},
                   {0.0,0.0,1.0},
                   {0.0,1.0,0.0},
                   {1.0,1.0,0.0},
                   {1.0,1.0,1.0},
                   {0.0,1.0,1.0}};
  double
    ea[8],dx,dy,dz;
  vec v;

  int i,j,k,l,
    ba[3]={4,4,4},/* Zu beschriftende Achse */
    sort[3][4]={
      {7,4,0,3},
      {3,0,1,2},
      {4,5,1,0}},
    in[4];
  pushtrafs();
    translate(-0.5,-0.5,-0.5);
    for (i=0;i<8;i++) {
      veq(v,mc2wc(ecken[i]));
      ea[i]=v[2];
    }
  /*    for (i=0;i<7;i++) linie_3d(gruen,ecken[i],ecken[i+1]);*/
  poptrafs();
  
  for (k=0;k<3;k++){
    for (i=0;i<4;i++) in[i]=i;
    j=1;
    while (j>0){ /* j zaehlt Vertauschungen */
      j=0;
      for (i=0;i<3;i++) {
	if (ea[sort[k][in[i]]]<ea[sort[k][in[i+1]]]){
	  j+=1;
	  l=in[i];in[i]=in[i+1];in[i+1]=l;
	}
      }
    }
    ba[k]=in[2]; /* which koordinate axis gets the tic's (was 1) */
  }
  
  Koordinatenachse(xmin,xmax,&(tics[0]),strexp[0],&(ntic[0]),1);
  Koordinatenachse(ymin,ymax,&(tics[1]),strexp[1],&(ntic[1]),1);
  Koordinatenachse(zmin,zmax,&(tics[2]),strexp[2],&(ntic[2]),1);
  if (0==strcmp(xstru,"[arb.u.]")) sprintf(strexp[0],"");
  if (0==strcmp(ystru,"[arb.u.]")) sprintf(strexp[1],"");
  if (0==strcmp(zstru,"[arb.u.]")) sprintf(strexp[2],"");
  if ((0==strcmp(xstru,""))&&(0==strcmp(strexp[0],"")))
    sprintf(label[0],"%s",xstr);
  else
    sprintf(label[0],"%s / %s %s",xstr,strexp[0],xstru);
  if ((0==strcmp(ystru,""))&&(0==strcmp(strexp[1],"")))
    sprintf(label[1],"%s",ystr);
  else
    sprintf(label[1],"%s / %s %s",ystr,strexp[1],ystru);
  if ((0==strcmp(zstru,""))&&(0==strcmp(strexp[2],"")))
    sprintf(label[2],"%s",zstr);
  else
    sprintf(label[2],"%s / %s %s",zstr,strexp[2],zstru);
  
  for (k=0;k<3;k++){
    pushtrafs();
    if (k==0) {dx=-sx/2; dy=sy/2; dz=sz/2;}
    else if (k==1) {
      dx=-sy/2; dy=sx/2; dz=sz/2;
      rotate(zaxis,-90);
    }
    else if (k==2) {
      dx=-sz/2; dy=sy/2; dz=sx/2;
      rotate(yaxis,-90);
    }
    for (j=0;j<4;j++){
      if (j==1) dz*=-1;
      else if (j==2) dy*=-1;
      else if (j==3) dz*=-1;
      pushtrafs();
      translate(dx,dy,dz);
      rotate(xaxis,j*90);
      zeichneachse(c,tics[k],ntic[k],(j==ba[k]),0.1,label[k],-2*dx);
      poptrafs();  
    }
    poptrafs();
  }
  for (i=0;i<3;i++){
    for (j=0;j<ntic[i];j++) 
      if (tics[i][j].name!=NULL) free(tics[i][j].name);
    free (tics[i]);
    
  }
}
