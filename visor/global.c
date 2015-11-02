//#include <mem.h>
#include <math.h>
#include "global.hpp"

tmatrix Identity;
int     red,green,blue;

#define TOTAL_MALLOCS 2048

int num_mallocs=0;
int mem_mallocs=0;
void *tabla_mallocs[TOTAL_MALLOCS];
int tabla_mem_mallocs[TOTAL_MALLOCS];

char ERRORSTR[256];

void IniMath()
{
  Identity._00=(float)1.0;Identity._01=(float)0.0;Identity._02=(float)0.0;Identity._03=(float)0.0;
  Identity._10=(float)0.0;Identity._11=(float)1.0;Identity._12=(float)0.0;Identity._13=(float)0.0;
  Identity._20=(float)0.0;Identity._21=(float)0.0;Identity._22=(float)1.0;Identity._23=(float)0.0;
  Identity._30=(float)0.0;Identity._31=(float)0.0;Identity._32=(float)0.0;Identity._33=(float)1.0;
}

void MatMult(lptmatrix matriz_c,lptmatrix matriz_a,lptmatrix matriz_b)
{
float *result=(float *)matriz_c;

float *mat1  =(float *)matriz_a;
float *mat2  =(float *)matriz_b;
int i,j;
//printf("result_addr %x\n",result);

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      result[(i<<2)+j]=((mat1[(i<<2)+0]*mat2[ 0+j])
                       +(mat1[(i<<2)+1]*mat2[ 4+j])
                       +(mat1[(i<<2)+2]*mat2[ 8+j])
                       +(mat1[(i<<2)+3]*mat2[12+j]));
}

void normalize(float *x,float *y,float *z)
{
float size=(float)sqrt((*x)*(*x)+(*y)*(*y)+(*z)*(*z));
  if (size>0)
  {
     (*x)=(*x)/size;
     (*y)=(*y)/size;
     (*z)=(*z)/size;
  }
}

float producto_escalar(lptvector v, lptvector p)
{
  return (v->x*p->x+v->y*p->y+v->z*p->z);
}

void GetAngs(lptmatrix mat,float *ax,float *ay,float *az,float *x,float *y,float *z)
{
  *x=mat->_30;
  *y=mat->_31;
  *z=mat->_32;
//  printf("1 %f\n",mat->_02);
  *ay=asin(-mat->_02);
//  printf("2 %f\n",mat->_01/cos(*ay));
  *az=asin(-(mat->_01/cos(*ay)));
//  printf("3 %f\n",mat->_12/cos(*az));
  *ax=asin((mat->_12/cos(*az)));
}

void MatShow(lptmatrix matriz_a)
{
float *mat1  =(float *)matriz_a;
int i,j;
  for (i=0; i<4; i++)
  {
    for (j=0; j<4; j++)
    {
      printf("%02.2f ",mat1[(i<<2)+j]);
    }
    printf("\n");
  }
}

void *lf_malloc(int size)
{
  void *puntero;

  if ((puntero=malloc(size))==NULL) {
    lf_free_all();
    return(NULL);
  }
  tabla_mallocs[num_mallocs]=puntero;
  tabla_mem_mallocs[num_mallocs]=size;
  mem_mallocs+=size;
  num_mallocs++;
//  printf("Reservado: %d, Total: %d\n",size,mem_mallocs);
  if (num_mallocs>=TOTAL_MALLOCS) {
	  printf("TOTAL MALLOC REACHED\n");
    lf_free_all();
    return(NULL);
  }
//  printf("returned: %x\n",puntero);
  return(puntero);
}

void lf_free(void *puntero)
{
  int i;

  for (i=0;i<num_mallocs;i++) {
    if (tabla_mallocs[i]==puntero) {
      free(puntero);
      mem_mallocs-=tabla_mem_mallocs[i];
      num_mallocs--;
//      printf("Liberado: %d, Total: %d\n",tabla_mem_mallocs[i],mem_mallocs);
      tabla_mallocs[i]=tabla_mallocs[num_mallocs];
      tabla_mem_mallocs[i]=tabla_mem_mallocs[num_mallocs];
      tabla_mallocs[num_mallocs]=NULL;
      return;
    }
  }
}

void lf_free_all()
{
  int i;

//  printf("Por liberar: %d, mallocs: %d\n",mem_mallocs,num_mallocs);
//  printf("Liberado Todo\n");
  for (i=num_mallocs-1;i>=0;i--) {
    free(tabla_mallocs[i]);
    tabla_mallocs[i]=NULL;
  }
  num_mallocs=0;
  mem_mallocs=0;
}
