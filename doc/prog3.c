#include <stdio.h>
#include <math.h>
#include <mygraph.h>

static int size=100, Repeat=1000,done=0,sstep=0,pause=1;
static double *f0=NULL,*f1=NULL,*f2=NULL, omega=1, T=0.3,Amplitude=1;
static double *density=NULL;
static int densityreq=0;


void init(){
  int i;
  f0= (double *) realloc(f0,size*sizeof(double));
  f1= (double *) realloc(f1,size*sizeof(double));
  f2= (double *) realloc(f2,size*sizeof(double));
  density= (double *) realloc(density,size*sizeof(double));
  for (i=0;i<size;i++){
    density[i]=(2+Amplitude*sin(2*M_PI*i/size));
    f0[i]=density[i]*(1-T);
    f1[i]=density[i]*T*0.5;
    f2[i]=density[i]*T*0.5;
  }
}

void iterate(){
  int i;
  double tmp1,tmp2;
  for (i=0;i<size;i++){
    density[i]=f0[i]+f1[i]+f2[i];
    f0[i]+=omega*(density[i]*(1-T)-f0[i]);
    f1[i]+=omega*(density[i]*T*0.5-f1[i]);
    f2[i]+=omega*(density[i]*T*0.5-f2[i]);
  }
  tmp1=f1[size-1];
  tmp2=f2[0];
  for (i=1;i<size;i++){
    f1[size-i]=f1[size-i-1];
    f2[i-1]=f2[i];
  }
  f1[0]=tmp1;
  f2[size-1]=tmp2;
}


void GUI(){
  DefineGraphN_R("Density",density,&size,&densityreq);
  StartMenu("GUI",1);
    DefineDouble("T",&T);
    DefineDouble("omega",&omega);
    StartMenu("Restart",0);
      DefineDouble("Amplitude",&Amplitude);
      DefineFunction("Restart now",&init);
    EndMenu();
    DefineGraph(curve2d_,"Density graph");
    DefineBool("Pause",&pause);
    DefineBool("Single step",&sstep);
    DefineInt("Repeat",&Repeat);    DefineBool("Done",&done);
  EndMenu();
}

int main(){
  int i;
  init();
  GUI();
  while (!done){
    Events(1); /* Whenever there are new data the argument of 
		  Events() should be nonzero. This will set the
		  requests for data so that you can calculate them
		  on demand only. For this simple program you can
		  always set it to one. */
    DrawGraphs();
    if (!pause || sstep){
      sstep=0;
      for (i=0;i<Repeat;i++) iterate();
    } else {
      sleep(1);/*when the program is waiting it returns the 
		 CPU time to the operating system */
    }
  }
  return 0;
}
