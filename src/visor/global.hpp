#ifndef __GLOBAL_HPP_
#define __GLOBAL_HPP_

#include "errores.hpp"
#include "struct.hpp"
#include <stdlib.h>

/////////////////////////////// Matriz Identidad.

extern tmatrix    Identity;
void IniMath();
void MatMult(lptmatrix matriz_c,lptmatrix matriz_a,lptmatrix matriz_b);
void normalize(float *x,float *y,float *z);
float producto_escalar(lptvector v, lptvector p);
void MatShow(lptmatrix matriz_a);
void GetAngs(lptmatrix mat,float *ax,float *ay,float *az,float *x,float *y,float *z);

/////////////////////////////// Cambio de modo de video (temporal)

void SetVideoMode(int);
#ifdef DOS
#pragma aux SetVideoMode parm[eax]="int 10h";
#endif

long schop(float);
/*
#pragma aux schop = \
        "push eax"\
        "fld dword ptr[esp]" \
        "fistp dword ptr[esp]" \
        "pop eax"\
        parm caller [eax]\
        value [eax]
*/

void *lf_malloc(int32_t size);
void lf_free(void *puntero);
void lf_free_all();


extern char ERRORSTR[256];


#endif
