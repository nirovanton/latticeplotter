/***************************************************************************\
graph library
Copyright (C) 1992-2003  Alexander Wagner

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "mygraph.h"

grstr *graph=NULL;
int graphactive;

static int maxgraph=5,graphno;
static int DefaultColor=1,DefaultLineType=1,DefaultShape=0,DefaultFill=0;
static double DefaultSize=1;

void SetDefaultColor(int c){DefaultColor=c;}
void SetDefaultLineType(int s){DefaultLineType=s;}
void SetDefaultShape(int s){DefaultShape=s;}
void SetDefaultSize(double s){DefaultSize=s;}
void SetDefaultFill(int f){DefaultFill=f;}

void StartGraph(grstr *graph){
  
  graph->anzN_R    = 0;
  graph->maxanzN_R = 0;
  graph->N_Rname   = NULL;
  graph->pN_R      = NULL;
  graph->dimN_R    = NULL;
  graph->dN_R      = NULL;
  graph->reqN_R    = NULL;
  

  graph->anzN_Rp    = 0;
  graph->maxanzN_Rp = 0;
  graph->N_Rnamep   = NULL;
  graph->pN_Rp      = NULL;
  graph->dimN_Rp    = NULL;
  graph->dN_Rp      = NULL;
  graph->reqN_Rp    = NULL;
  

  graph->anzN_RxR    = 0;
  graph->maxanzN_RxR = 0;
  graph->N_RxRname   = NULL;
  graph->pN_RxR      = NULL;
  graph->dimN_RxR    = NULL;
  graph->dN_RxR      = NULL;
  graph->reqN_RxR    = NULL;

  graph->anzN_RxRp    = 0;
  graph->maxanzN_RxRp = 0;
  graph->N_RxRpname   = NULL;
  graph->pN_RxRp      = NULL;
  graph->dimN_RxRp    = NULL;
  graph->dN_RxRp      = NULL;
  graph->reqN_RxRp    = NULL;

  graph->anzNxN_R    = 0;
  graph->maxanzNxN_R = 0;
  graph->NxN_Rname   = NULL;
  graph->pNxN_R      = NULL;
  graph->dimNxN_R    = NULL;
  graph->reqNxN_R    = NULL;

  graph->anzNxN_Rp    = 0;
  graph->maxanzNxN_Rp = 0;
  graph->NxN_Rpname   = NULL;
  graph->pNxN_Rp      = NULL;
  graph->dimNxN_Rp    = NULL;
  graph->reqNxN_Rp    = NULL;

  graph->anzNxN_RxR    = 0;
  graph->maxanzNxN_RxR = 0;
  graph->NxN_RxRname   = NULL;
  graph->pNxN_RxR      = NULL;
  graph->dimNxN_RxR    = NULL;
  graph->reqNxN_RxR    = NULL;

  graph->anzNxN_RxRp    = 0;
  graph->maxanzNxN_RxRp = 0;
  graph->NxN_RxRpname   = NULL;
  graph->pNxN_RxRp      = NULL;
  graph->dimNxN_RxRp    = NULL;
  graph->reqNxN_RxRp    = NULL;

  graph->anzNxN_RxRxRt    = 0;
  graph->maxanzNxN_RxRxRt = 0;
  graph->NxN_RxRxRtname   = NULL;
  graph->pNxN_RxRxRt      = NULL;
  graph->dimNxN_RxRxRt    = NULL;
  graph->reqNxN_RxRxRt    = NULL;

  graph->anzNxNxN_R    = 0;
  graph->maxanzNxNxN_R = 0;
  graph->NxNxN_Rname   = NULL;
  graph->pNxNxN_R      = NULL;
  graph->dimNxNxN_R    = NULL;
  graph->reqNxNxN_R    = NULL;

  graph->anzNxNxN_Rp    = 0;
  graph->maxanzNxNxN_Rp = 0;
  graph->NxNxN_Rnamep   = NULL;
  graph->pNxNxN_Rp      = NULL;
  graph->dimNxNxN_Rp    = NULL;
  graph->reqNxNxN_Rp    = NULL;

  graph->anzcont3d    = 0;
  graph->maxanzcont3d = 0;
  graph->cont3dname   = NULL;
  graph->pcont3d      = NULL;
  graph->dimcont3d    = NULL;
  graph->reqcont3d    = NULL;

}

void InitGraph(){
  if (graph!=NULL){
    printf("Error in graph.c:InitGraph: graph already initialized."
	   "Init ignored...\n");
    return;
  }
  graph = (grstr *) malloc(maxgraph*sizeof(grstr));
  graphno=0;
  graphactive=0;
  StartGraph(&(graph[graphno]));
}

void NewGraph(){
  graphno++;
  if (graphno>=maxgraph){
    maxgraph*=2;
    graph=(grstr *) realloc(graph,maxgraph*sizeof(grstr));
  }
  StartGraph(&(graph[graphno]));
}

void SetActiveGraph(int no){
  if (no>=graphno) {
    printf("Error in graph:SetActiveGraph: %i >= %i "
	   "(number of available graphs)\n"
	   "Ignored...\n",no,graphno);
    return;
  }
  graphactive=no;
}

void DestroyGraph(grstr *graph){
  /* still needs some work ... */
  free(graph->N_Rname);
  free(graph->reqN_R);
  free(graph->dimN_R);

  free(graph->N_Rnamep);
  free(graph->reqN_Rp);
  free(graph->dimN_Rp);

  free(graph->N_RxRname);
  free(graph->reqN_RxR);
  free(graph->dimN_RxR);

  free(graph->N_RxRpname);
  free(graph->reqN_RxRp);
  free(graph->dimN_RxRp);

  free(graph->NxN_Rname);
  free(graph->reqNxN_R);
  free(graph->dimNxN_R);

  free(graph->NxN_Rpname);
  free(graph->reqNxN_Rp);
  free(graph->dimNxN_Rp);

  free(graph->NxN_RxRname);
  free(graph->reqNxN_RxR);
  free(graph->dimNxN_RxR);

  free(graph->NxN_RxRpname);
  free(graph->reqNxN_RxRp);
  free(graph->dimNxN_RxRp);

  free(graph->NxN_RxRxRtname);
  free(graph->reqNxN_RxRxRt);
  free(graph->dimNxN_RxRxRt);
}

void DefineGraphN_R(char *Name,double *data,int *dim,int *req){
  const static int optlen =10;

  if (graph==NULL) InitGraph();
  graph[graphno].anzN_R++;
  if (graph[graphno].anzN_R>=graph[graphno].maxanzN_R){
    graph[graphno].maxanzN_R+=optlen;
    graph[graphno].N_Rname = (char **) 
      realloc(graph[graphno].N_Rname,
	      graph[graphno].maxanzN_R*sizeof(char *));
    graph[graphno].dimN_R = (dim1 *) 
      realloc(graph[graphno].dimN_R,graph[graphno].maxanzN_R*sizeof(dim1));
    graph[graphno].pN_R = (double **) 
      realloc(graph[graphno].pN_R,
	      graph[graphno].maxanzN_R*sizeof(double *));
    graph[graphno].dN_R = (grdat *)
      realloc(graph[graphno].dN_R,graph[graphno].maxanzN_R*sizeof(grdat));
    graph[graphno].reqN_R = (int **)
      realloc(graph[graphno].reqN_R,graph[graphno].maxanzN_R*sizeof(int*));
  }
  graph[graphno].N_Rname[graph[graphno].anzN_R-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].N_Rname[graph[graphno].anzN_R-1],"%s",Name);
  graph[graphno].dimN_R[graph[graphno].anzN_R-1][0]=dim;
  graph[graphno].pN_R[graph[graphno].anzN_R-1]=data;
  graph[graphno].reqN_R[graph[graphno].anzN_R-1]=req;
  graph[graphno].dN_R[graph[graphno].anzN_R-1].color=DefaultColor;
  graph[graphno].dN_R[graph[graphno].anzN_R-1].linetype=DefaultLineType;
  graph[graphno].dN_R[graph[graphno].anzN_R-1].shape=DefaultShape;
  graph[graphno].dN_R[graph[graphno].anzN_R-1].fill=DefaultFill;
  graph[graphno].dN_R[graph[graphno].anzN_R-1].size=DefaultSize;
}

void DefineGraphN_Rp(char *Name,double **data,int *dim,int *req){
  const static int optlen =10;

  if (graph==NULL) InitGraph();
  graph[graphno].anzN_Rp++;
  if (graph[graphno].anzN_Rp>=graph[graphno].maxanzN_Rp){
    graph[graphno].maxanzN_Rp+=optlen;
    graph[graphno].N_Rnamep = (char **) 
      realloc(graph[graphno].N_Rnamep,
	      graph[graphno].maxanzN_Rp*sizeof(char *));
    graph[graphno].dimN_Rp = (dim1 *) 
      realloc(graph[graphno].dimN_Rp,graph[graphno].maxanzN_Rp*sizeof(dim1));
    graph[graphno].pN_Rp = (double ***) 
      realloc(graph[graphno].pN_Rp,
	      graph[graphno].maxanzN_Rp*sizeof(double **));
    graph[graphno].dN_Rp = (grdat *)
      realloc(graph[graphno].dN_Rp,graph[graphno].maxanzN_Rp*sizeof(grdat));
    graph[graphno].reqN_Rp = (int **)
      realloc(graph[graphno].reqN_Rp,graph[graphno].maxanzN_Rp*sizeof(int*));
  }
  graph[graphno].N_Rnamep[graph[graphno].anzN_Rp-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].N_Rnamep[graph[graphno].anzN_Rp-1],"%s",Name);
  graph[graphno].dimN_Rp[graph[graphno].anzN_Rp-1][0]=dim;
  graph[graphno].pN_Rp[graph[graphno].anzN_Rp-1]=data;
  graph[graphno].reqN_Rp[graph[graphno].anzN_Rp-1]=req;
  graph[graphno].dN_Rp[graph[graphno].anzN_Rp-1].color=DefaultColor;
  graph[graphno].dN_Rp[graph[graphno].anzN_Rp-1].linetype=DefaultLineType;
  graph[graphno].dN_Rp[graph[graphno].anzN_Rp-1].shape=DefaultShape;
  graph[graphno].dN_Rp[graph[graphno].anzN_Rp-1].fill=DefaultFill;
  graph[graphno].dN_Rp[graph[graphno].anzN_Rp-1].size=DefaultSize;
}

void DefineGraphN_RxR(char *Name,double *data,int *dim,int *req){
  const static int optlen =10;

  if (graph==NULL) InitGraph();
  graph[graphno].anzN_RxR++;
  if (graph[graphno].anzN_RxR>=graph[graphno].maxanzN_RxR){
    graph[graphno].maxanzN_RxR+=optlen;
    graph[graphno].N_RxRname = (char **) 
      realloc(graph[graphno].N_RxRname,
	      graph[graphno].maxanzN_RxR*sizeof(char *));
    graph[graphno].dimN_RxR = (dim1 *) 
      realloc(graph[graphno].dimN_RxR,graph[graphno].maxanzN_RxR*sizeof(dim1));
    graph[graphno].pN_RxR = (double **) 
      realloc(graph[graphno].pN_RxR,
	      graph[graphno].maxanzN_RxR*sizeof(double *));
    graph[graphno].dN_RxR = (grdat *)
      realloc(graph[graphno].dN_RxR,graph[graphno].maxanzN_RxR*sizeof(grdat));
    graph[graphno].reqN_RxR = (int **)
      realloc(graph[graphno].reqN_RxR,graph[graphno].maxanzN_RxR*sizeof(int*));
  }
  graph[graphno].N_RxRname[graph[graphno].anzN_RxR-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].N_RxRname[graph[graphno].anzN_RxR-1],"%s",Name);
  graph[graphno].dimN_RxR[graph[graphno].anzN_RxR-1][0]=dim;
  graph[graphno].pN_RxR[graph[graphno].anzN_RxR-1]=data;
  graph[graphno].reqN_RxR[graph[graphno].anzN_RxR-1]=req;
  graph[graphno].dN_RxR[graph[graphno].anzN_RxR-1].color=DefaultColor;
  graph[graphno].dN_RxR[graph[graphno].anzN_RxR-1].linetype=DefaultLineType;
  graph[graphno].dN_RxR[graph[graphno].anzN_RxR-1].shape=DefaultShape;
  graph[graphno].dN_RxR[graph[graphno].anzN_RxR-1].fill=DefaultFill;
  graph[graphno].dN_RxR[graph[graphno].anzN_RxR-1].size=DefaultSize;
}

void DefineGraphN_RxRp(char *Name,double **data,
		       int *dim,int *req){
  const static int optlen =10;
  if (graph==NULL) InitGraph();
  
  graph[graphno].anzN_RxRp++;
  if (graph[graphno].anzN_RxRp>=graph[graphno].maxanzN_RxRp){
    graph[graphno].maxanzN_RxRp+=optlen;
    graph[graphno].N_RxRpname = (char **) 
      realloc(graph[graphno].N_RxRpname,
	      graph[graphno].maxanzN_RxRp*sizeof(char *));
    graph[graphno].dimN_RxRp = (dim1 *) 
      realloc(graph[graphno].dimN_RxRp,
	      graph[graphno].maxanzN_RxRp*sizeof(dim1));
    graph[graphno].pN_RxRp = (double ***) 
      realloc(graph[graphno].pN_RxRp,
	      graph[graphno].maxanzN_RxRp*sizeof(double **));
    graph[graphno].dN_RxRp = (grdat *)
      realloc(graph[graphno].dN_RxRp,
	      graph[graphno].maxanzN_RxRp*sizeof(grdat));
    graph[graphno].reqN_RxRp = (int **)
      realloc(graph[graphno].reqN_RxRp,
	      graph[graphno].maxanzN_RxRp*sizeof(int*));
  }
  graph[graphno].N_RxRpname[graph[graphno].anzN_RxRp-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].N_RxRpname[graph[graphno].anzN_RxRp-1],"%s",Name);
  graph[graphno].dimN_RxRp[graph[graphno].anzN_RxRp-1][0]=dim;
  graph[graphno].pN_RxRp[graph[graphno].anzN_RxRp-1]=data;
  graph[graphno].reqN_RxRp[graph[graphno].anzN_RxRp-1]=req;
  graph[graphno].dN_RxRp[graph[graphno].anzN_RxRp-1].color=DefaultColor;
  graph[graphno].dN_RxRp[graph[graphno].anzN_RxRp-1].linetype=DefaultLineType;
  graph[graphno].dN_RxRp[graph[graphno].anzN_RxRp-1].shape=DefaultShape;
  graph[graphno].dN_RxRp[graph[graphno].anzN_RxRp-1].fill=DefaultFill;
  graph[graphno].dN_RxRp[graph[graphno].anzN_RxRp-1].size=DefaultSize;
}

void DefineGraphNxN_R(char *Name,double *data,
                      int *mydim1,int *mydim2,int *req){
  const static int optlen =10;
  
  if (graph==NULL) InitGraph();
  graph[graphno].anzNxN_R++;
  if (graph[graphno].anzNxN_R>=graph[graphno].maxanzNxN_R){
    graph[graphno].maxanzNxN_R+=optlen;
    graph[graphno].NxN_Rname = (char **) 
      realloc(graph[graphno].NxN_Rname,graph[graphno].maxanzNxN_R*sizeof(char *));
    graph[graphno].dimNxN_R = (dim2 *) 
      realloc(graph[graphno].dimNxN_R,graph[graphno].maxanzNxN_R*sizeof(dim2));
    graph[graphno].pNxN_R = (double **) 
      realloc(graph[graphno].pNxN_R,graph[graphno].maxanzNxN_R*sizeof(double *));
    graph[graphno].reqNxN_R = (int **)
      realloc(graph[graphno].reqNxN_R,graph[graphno].maxanzNxN_R*sizeof(int*));
  }
  graph[graphno].NxN_Rname[graph[graphno].anzNxN_R-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].NxN_Rname[graph[graphno].anzNxN_R-1],"%s",Name);
  graph[graphno].dimNxN_R[graph[graphno].anzNxN_R-1][0]=mydim1;
  graph[graphno].dimNxN_R[graph[graphno].anzNxN_R-1][1]=mydim2;
  graph[graphno].pNxN_R[graph[graphno].anzNxN_R-1]=data;
  graph[graphno].reqNxN_R[graph[graphno].anzNxN_R-1]=req;
}

void DefineGraphNxN_Rp(char *Name,double **data,
                      int *mydim1,int *mydim2,int *req){
  const static int optlen =10;
  
  if (graph==NULL) InitGraph();
  graph[graphno].anzNxN_Rp++;
  if (graph[graphno].anzNxN_Rp>=graph[graphno].maxanzNxN_Rp){
    graph[graphno].maxanzNxN_Rp+=optlen;
    graph[graphno].NxN_Rpname = (char **) 
      realloc(graph[graphno].NxN_Rpname,graph[graphno].maxanzNxN_Rp*sizeof(char *));
    graph[graphno].dimNxN_Rp = (dim2 *) 
      realloc(graph[graphno].dimNxN_Rp,graph[graphno].maxanzNxN_Rp*sizeof(dim2));
    graph[graphno].pNxN_Rp = (double **) 
      realloc(graph[graphno].pNxN_Rp,graph[graphno].maxanzNxN_Rp*sizeof(double *));
    graph[graphno].reqNxN_Rp = (int **)
      realloc(graph[graphno].reqNxN_Rp,graph[graphno].maxanzNxN_Rp*sizeof(int*));
  }
  graph[graphno].NxN_Rpname[graph[graphno].anzNxN_Rp-1]=
    (char *) malloc((strlen(Name)+1)*sizeof(char));
  sprintf(graph[graphno].NxN_Rpname[graph[graphno].anzNxN_Rp-1],"%s",Name);
  graph[graphno].dimNxN_Rp[graph[graphno].anzNxN_Rp-1][0]=mydim1;
  graph[graphno].dimNxN_Rp[graph[graphno].anzNxN_Rp-1][1]=mydim2;
  graph[graphno].pNxN_Rp[graph[graphno].anzNxN_Rp-1]=data;
  graph[graphno].reqNxN_Rp[graph[graphno].anzNxN_Rp-1]=req;
}

void DefineGraphNxN_RxR(char *Name,double *data,
			int *mydim1,int *mydim2,int *req){
  const static int optlen =10;
  
  graph[graphno].anzNxN_RxR++;
  if (graph[graphno].anzNxN_RxR>=graph[graphno].maxanzNxN_RxR){
    graph[graphno].maxanzNxN_RxR+=optlen;
    graph[graphno].NxN_RxRname = (char **) 
      realloc(graph[graphno].NxN_RxRname,graph[graphno].maxanzNxN_RxR*sizeof(char *));
    graph[graphno].dimNxN_RxR = /*(dim2 *) What the hell is wrong here ? */ 
      realloc(graph[graphno].dimNxN_RxR,graph[graphno].maxanzNxN_RxR*sizeof(dim2));
    graph[graphno].pNxN_RxR = (double **) 
      realloc(graph[graphno].pNxN_RxR,graph[graphno].maxanzNxN_RxR*sizeof(double *));
    graph[graphno].reqNxN_RxR = (int **)
      realloc(graph[graphno].reqNxN_RxR,graph[graphno].maxanzNxN_RxR*sizeof(int*));
  }
  graph[graphno].NxN_RxRname[graph[graphno].anzNxN_RxR-1]=Name;
  graph[graphno].dimNxN_RxR[graph[graphno].anzNxN_RxR-1][0]=mydim1;
  graph[graphno].dimNxN_RxR[graph[graphno].anzNxN_RxR-1][1]=mydim2;
  graph[graphno].pNxN_RxR[graph[graphno].anzNxN_RxR-1]=data;
  graph[graphno].reqNxN_RxR[graph[graphno].anzNxN_RxR-1]=req;
}

void DefineGraphNxN_RxRxRt(char *Name,double *data,
			   int *mydim1,int *mydim2,int *req){
  const static int optlen =10;
  
  if (graph==NULL) InitGraph();
  graph[graphno].anzNxN_RxRxRt++;
  if (graph[graphno].anzNxN_RxRxRt>=graph[graphno].maxanzNxN_RxRxRt){
    graph[graphno].maxanzNxN_RxRxRt+=optlen;
    graph[graphno].NxN_RxRxRtname = (char **) 
      realloc(graph[graphno].NxN_RxRxRtname,graph[graphno].maxanzNxN_RxRxRt*sizeof(char *));
    graph[graphno].dimNxN_RxRxRt = /*(dim2 *) What the hell is wrong here ? */ 
      realloc(graph[graphno].dimNxN_RxRxRt,graph[graphno].maxanzNxN_RxRxRt*sizeof(dim2));
    graph[graphno].pNxN_RxRxRt = (double **) 
      realloc(graph[graphno].pNxN_RxRxRt,graph[graphno].maxanzNxN_RxRxRt*sizeof(double *));
    graph[graphno].reqNxN_RxRxRt = (int **)
      realloc(graph[graphno].reqNxN_RxRxRt,graph[graphno].maxanzNxN_RxRxRt*sizeof(int*));
  }
  graph[graphno].NxN_RxRxRtname[graph[graphno].anzNxN_RxRxRt-1]=Name;
  graph[graphno].dimNxN_RxRxRt[graph[graphno].anzNxN_RxRxRt-1][0]=mydim1;
  graph[graphno].dimNxN_RxRxRt[graph[graphno].anzNxN_RxRxRt-1][1]=mydim2;
  graph[graphno].pNxN_RxRxRt[graph[graphno].anzNxN_RxRxRt-1]=data;
  graph[graphno].reqNxN_RxRxRt[graph[graphno].anzNxN_RxRxRt-1]=req;
}

void DefineGraphNxNxN_R(char *Name,double *data,
                      int *mydim1,int *mydim2,int *mydim3,int *req){
  const static int optlen =10;
  
  if (graph==NULL) InitGraph();
  graph[graphno].anzNxNxN_R++;
  if (graph[graphno].anzNxNxN_R>=graph[graphno].maxanzNxNxN_R){
    graph[graphno].maxanzNxNxN_R+=optlen;
    graph[graphno].NxNxN_Rname = (char **) 
      realloc(graph[graphno].NxNxN_Rname,graph[graphno].maxanzNxNxN_R*sizeof(char *));
    graph[graphno].dimNxNxN_R = (dim3 *) 
      realloc(graph[graphno].dimNxNxN_R,graph[graphno].maxanzNxNxN_R*sizeof(dim2));
    graph[graphno].pNxNxN_R = (double **) 
      realloc(graph[graphno].pNxNxN_R,graph[graphno].maxanzNxNxN_R*sizeof(double *));
    graph[graphno].reqNxNxN_R = (int **)
      realloc(graph[graphno].reqNxNxN_R,graph[graphno].maxanzNxNxN_R*sizeof(int*));
  }
  graph[graphno].NxNxN_Rname[graph[graphno].anzNxNxN_R-1]=Name;
  graph[graphno].dimNxNxN_R[graph[graphno].anzNxNxN_R-1][0]=mydim1;
  graph[graphno].dimNxNxN_R[graph[graphno].anzNxNxN_R-1][1]=mydim2;
  graph[graphno].dimNxNxN_R[graph[graphno].anzNxNxN_R-1][2]=mydim3;
  graph[graphno].pNxNxN_R[graph[graphno].anzNxNxN_R-1]=data;
  graph[graphno].reqNxNxN_R[graph[graphno].anzNxNxN_R-1]=req;
}

void DefineGraphContour3d(char *Name,Contour **data,
			  int *mydim1,int *mydim2,int *mydim3,int *req){
  const static int optlen =10;
  
  if (graph==NULL) InitGraph();
  graph[graphno].anzcont3d++;
  if (graph[graphno].anzcont3d>=graph[graphno].maxanzcont3d){
    graph[graphno].maxanzcont3d+=optlen;
    graph[graphno].cont3dname = (char **) 
      realloc(graph[graphno].cont3dname,graph[graphno].maxanzcont3d*sizeof(char *));
    graph[graphno].dimcont3d = (dim3 *) 
      realloc(graph[graphno].dimcont3d,graph[graphno].maxanzcont3d*sizeof(dim3));
    graph[graphno].pcont3d = (Contour ***) 
      realloc(graph[graphno].pcont3d,graph[graphno].maxanzcont3d*sizeof(Contour **));
    graph[graphno].reqcont3d = (int **)
      realloc(graph[graphno].reqcont3d,graph[graphno].maxanzcont3d*sizeof(int*));
  }
  graph[graphno].cont3dname[graph[graphno].anzcont3d-1]=Name;
  graph[graphno].dimcont3d[graph[graphno].anzcont3d-1][0]=mydim1;
  graph[graphno].dimcont3d[graph[graphno].anzcont3d-1][1]=mydim2;
  graph[graphno].dimcont3d[graph[graphno].anzcont3d-1][2]=mydim3;
  graph[graphno].pcont3d[graph[graphno].anzcont3d-1]=data;
  graph[graphno].reqcont3d[graph[graphno].anzcont3d-1]=req;
}

void getgraph3d(grstr graph,int type,double **gp,int *no,int get, 
		int rough, int *copy)
     /* rough and copy options are void for now. */
{
  int rx_size,ry_size,rz_size;
  
#ifdef notdef
  /* Make general ! 
  if ((copy[0]<=0)||(copy[1]<=0)){
    printf("twocomp.c:getgraph3d() -> copyx=%i copyy=%i\n Exiting...",
	   copyx,copyy);
    exit(1);
  }
  */
#endif
  switch (type){
  case N_R:
    rx_size=/*copy[0]**/*graph.dimN_R[get][0];
#ifdef notdef
    for (finish=0;finish==0;){
      no[0]=(rx_size-1)/rough+1;
      gptmp=(double *) realloc(*gp,no[0]*sizeof(double));
      if (gptmp==NULL){
	fprintf(stderr,"Error in getgraph3d: realloc:%i,->rough++;\n",
		no[0]*sizeof(double));
	rough++;
      }
      else {
	finish=1;
	*gp=gptmp;
      }
    }
    for (x=0;x<no[0];x++)
      {
	graph.getN_R[get](x*rough,&(*gp)[x]);
      }
#endif
    *gp=graph.pN_R[get];
    break;
  case N_Rp:
    rx_size=/*copy[0]**/*graph.dimN_Rp[get][0];
    *gp=*graph.pN_Rp[get];
    break;
  case N_RxR:
    rx_size=/*copy[0]**/*graph.dimN_RxR[get][0];
    no[0]=rx_size;
    *gp=graph.pN_RxR[get];
    break;

  case N_RxRp:
    rx_size=/*copy[0]**/*graph.dimN_RxRp[get][0];
    no[0]=rx_size;
    *gp=*graph.pN_RxRp[get];
    break;

  case NxN_R:
    rx_size=/*copy[0]**/*graph.dimNxN_R[get][0];
    ry_size=/*copy[1]**/*graph.dimNxN_R[get][1];
    no[0]=rx_size;
    no[1]=ry_size;
    *gp=graph.pNxN_R[get];
    break;
  case NxN_Rp:
    rx_size=/*copy[0]**/*graph.dimNxN_Rp[get][0];
    ry_size=/*copy[1]**/*graph.dimNxN_Rp[get][1];
    no[0]=rx_size;
    no[1]=ry_size;
    *gp=*graph.pNxN_Rp[get];
    break;
  case NxN_RxR:
    rx_size=/*copy[0]**/*graph.dimNxN_RxR[get][0];
    ry_size=/*copy[1]**/*graph.dimNxN_RxR[get][1];
    no[0]=rx_size;
    no[1]=ry_size;
    *gp=graph.pNxN_RxR[get];
    break;
  case NxN_RxRxRt:
    rx_size=/*copy[0]**/*graph.dimNxN_RxRxRt[get][0];
    ry_size=/*copy[1]**/*graph.dimNxN_RxRxRt[get][1];
    no[0]=rx_size;
    no[1]=ry_size;
    *gp=graph.pNxN_RxRxRt[get];
    break;
  case NxNxN_R:
    rx_size=/*copy[0]**/*graph.dimNxNxN_R[get][0];
    ry_size=/*copy[1]**/*graph.dimNxNxN_R[get][1];
    rz_size=/*copy[1]**/*graph.dimNxNxN_R[get][2];
    no[0]=rx_size;
    no[1]=ry_size;
    no[2]=rz_size;
    *gp=graph.pNxNxN_R[get];
    break;
  case cont3d:
    no[0]=/*copy[0]**/*graph.dimNxN_R[get][0];
    no[1]=/*copy[1]**/*graph.dimNxN_R[get][1];
    no[2]=/*copy[2]**/*graph.dimNxN_R[get][2];
    *gp=(double *) *graph.pcont3d[get]; /* Is actually Contour */
    break;

  default: printf("Wrong type in getgraph3d");
  }
}




