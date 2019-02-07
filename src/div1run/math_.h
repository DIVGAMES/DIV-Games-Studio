/*
 *  math.h	Math functions
 *
 *  Copyright by WATCOM International Corp. 1988-1994.  All rights reserved.
 */
#ifndef _MATH_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(1);
#ifdef M_I86HM
extern const double __far _HugeValue;
#else
extern const double __near _HugeValue;
#endif
#define HUGE_VAL _HugeValue

extern double acos(double __x);
extern double asin(double __x);
extern double atan(double __x);
extern double atan2(double __y, double __x);
extern double ceil(double __x);
extern double cos(double __x);
extern double cosh(double __x);
// extern double   exp( double __x );
extern double fabs(double __x);
extern double floor(double __x);
extern double fmod(double __x, double __y);
extern double frexp(double __value, int *__exp);
extern double ldexp(double __x, int __exp);
extern double log(double __x);
extern double log10(double __x);
extern double modf(double __value, double *__iptr);
extern double pow(double __x, double __y);
extern double sin(double __x);
extern double sinh(double __x);
extern double sqrt(double __x);
extern double tan(double __x);
extern double tanh(double __x);

/* non-ANSI */
#if !defined(NO_EXT_KEYS) /* extensions enabled */

struct _complex {
	double x;
	double y;
};
#ifndef __cplusplus
struct complex {
	double x;
	double y;
};
#endif

extern double acosh(double __x);
extern double asinh(double __x);
extern double atanh(double __x);
extern double cabs(struct _complex);
extern double hypot(double __x, double __y);
extern double j0(double __x);
extern double j1(double __x);
extern double jn(int __n, double __x);
extern double log2(double __x);
extern double y0(double __x);
extern double y1(double __x);
extern double yn(int __n, double __x);

/* The following struct is used to record errors detected in the math library.
 * matherr is called with a pointer to this struct for possible error recovery.
 */

struct exception {
	int type;      /* type of error, see below */
	char *name;    /* name of math function */
	double arg1;   /* value of first argument to function */
	double arg2;   /* second argument (if indicated) */
	double retval; /* default return value */
};

#define DOMAIN 1    /* argument domain error */
#define SING 2      /* argument singularity  */
#define OVERFLOW 3  /* overflow range error  */
#define UNDERFLOW 4 /* underflow range error */
#define TLOSS 5     /* total loss of significance */
#define PLOSS 6     /* partial loss of significance */

extern int matherr(struct exception *);
extern double _matherr(struct exception *);
#endif /* EXTENSIONS */

#ifndef __NO_MATH_OPS
/*
    Defining the __NO_MATH_OPS macro will stop the compiler from recogizing
    the following functions as intrinsic operators.
*/
#pragma intrinsic(log, cos, sin, tan, sqrt, fabs, pow, atan2, fmod)
#pragma intrinsic(acos, asin, atan, cosh, exp, log10, sinh, tanh)
//    #pragma intrinsic(acos,asin,atan,cosh,log10,sinh,tanh)
#endif
#pragma pack();
#define _MATH_H_INCLUDED
#ifdef __cplusplus
};
#endif
#endif
