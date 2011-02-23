#ifndef ANALYSE_H
#define ANALYSE_H

#include "basicdef.h"

extern void savegrdata(grstr graph,char *name,int no,int rough);
extern double calc_vol(grstr graph, int nogr, double cut);
extern int my_calc_center(double *g,double *center,int x_size,int y_size);
extern int calc_ABtet(grstr graph,double *a, double *b, double *tet);
extern double calc_D(grstr graph,double *tet);
extern int checkcontours(grstr graph,double cut);
extern void data_from_contour(double *cont, int anz,
		       double *a, double *b, double *tet);
extern double pressure_difference(grstr graph);
extern double get_min(grstr graph,int nograph);
extern double get_max(grstr graph,int nograph);
extern void get_minmax(double *gp,int anz,double *min, double *max);
extern double get_vecmax(grstr graph,int nograph);
extern double get_vecav(grstr graph,int nograph);
extern double get_av(grstr graph,int nograph);
extern double get_av2(grstr graph,int nograph);
extern double get_areafrac(grstr graph,int nograph,double cut);
extern double S0(double data[],int nn[]);
extern double S1(double data[],int nn[]);
extern void Sk(double data[],int nn[],double kav,double vec[2]);
extern void Sk2(double data[],int nn[],double kav,double mat[3]);
extern void Sd2(double data[],int nn[],double mat[3]);
extern void Sd2sin(double data[],int nn[],double mat[3]);
extern double Sd0(double data[],int nn[]);
extern int drop_data(grstr graph,double cut,double *a,double *b, double *tet);
extern void get_fftdata(grstr graph,int nogr,double *d0, double *kav, 
		 double d1[2],double d2[3]);
extern void get_Struct_data(grstr graph,int nogr,double *S_0, double *S_1, 
		     double *S_2, double *S_3, int *Slen, int *size);
extern double* get_structfact(grstr graph,int nogr,int *Slen);
extern double* GetStructFactFromData(double *gp,int nox,int noy,int *Slen);
extern void get_derivdata(grstr graph, int nogr,double d2[3]);
extern int addmat(double data[],int nn[],int xg,int y,double mat[4],int dy,int *no);
extern void Sd2(double data[],int nn[],double mat[3]);
extern int add(double data[],int nn[],int xg,int y,double *sum,int dy,int *no);
extern double Sd0(double data[],int nn[]);
extern void contourdata(double *cont,int *length,int *N,int *no1,int *no2,
		 double *len);
extern void dircontourdata(double *cont,int *length,int *N,int *no1,int *no2,
		    double *len, double *len1, double *len2,double *tet);

#endif
