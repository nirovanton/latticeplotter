#include <stdio.h>
#include <math.h>

#define size 100
static Repeat=1000;
static double f0[size],f1[size],f2[size], omega=1, T=0.3;

void init(){
  int i;
  double density;
  for (i=0;i<size;i++){
    density=(2+sin(2*M_PI*i/size));
    f0[i]=density*(1-T);
    f1[i]=density*T*0.5;
    f2[i]=density*T*0.5;
  }
}

void iterate(){
  int i;
  double density,tmp1,tmp2;
  for (i=0;i<size;i++){
    density=f0[i]+f1[i]+f2[i];
    f0[i]+=omega*(density*(1-T)-f0[i]);
    f1[i]+=omega*(density*T*0.5-f1[i]);
    f2[i]+=omega*(density*T*0.5-f2[i]);
  }
  tmp1=f1[size-1];
  tmp2=f2[0];
  for (i=1;i<size;i++){
    f1[size-i]=f1[size-1-i];
    f2[i-1]=f2[i];
  }
  f1[0]=tmp1;
  f2[size-1]=tmp2;
}

int main(){
  int i;
  init();
  for (i=0;i<Repeat;i++) iterate();
  return 0;
}
