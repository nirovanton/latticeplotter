#ifndef _VECTOR_H
#define _VECTOR_H

#define vecdim 3
#define vecdimsub1 2
typedef double 	vec[vecdim];
typedef double 	*vecp;
typedef double 	mat[vecdim*vecdim];
typedef double	*matp;

extern vecp vnext(void);
extern vecp vsum(vecp v1, vecp v2);
extern vecp vdiff(vecp v1, vecp v2);
extern double sp(vecp v1, vecp v2);
extern double vsqr(vecp v);
extern double vabs(vecp v);
extern double vsqr(vecp v);
extern vecp vnorm(vecp v);
extern vecp vp(vecp v1, vecp v2);
extern vecp sm(double s,vecp v);
extern vecp vnew();
extern void veq(vecp v1, vecp v2);
extern void vreset();
extern void pv(vecp v);
extern void pve(vecp v);

extern vecp matvecmul(matp m, vecp v);
extern matp matmatmul(matp m, matp m2);
extern matp mnew();
extern void meq(matp v1, matp v2);
extern void mreset();   
extern void pm(matp v);

#endif /* _VECTOR_H */




