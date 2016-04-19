#include "global.h"

//#include <stdio.h>
//#include <conio.h>
//#include <i86.h>
#include <time.h>
#include <math.h>

#include "resource.hpp"
#include "global.hpp"
#include "hlrender.hpp"
#include "complex.hpp"
#include "animated.hpp"
#include "visor.hpp"

char *ERROR=NULL;

animated      animado;
fileanim      fichero_animaciones;
Resource      resource_camara;
hlrender      world_camara;
sprite3d      camara;
buffer_sprite3d  *sbuffer=NULL;
buffer_sprite3d  *my_buffer=NULL;

char *puntero_textura;
char *puntero_dac;

int lib_3d=0;
int lib_3d_ancho=0;
int lib_3d_alto=0;
char *lib_3d_buffer=NULL;
#define lfset memset

/*
void lfset(void *, long, long);

#pragma aux lfset = \
	"rep stosw" \
        parm caller [edi] [eax] [ecx]\
	modify exact [edi eax ecx] \



void lfset(void *buf, long c, long d) {
	printf("Should fill buffer [%x] with c [%d] for d [%d]  bytes\n",buf,c,d);
	long f=0;
	
	while(f++<d) {
		*buf++=c;
	}
	
	return;
}
*/

void reescalar_sprite3d(buffer_sprite3d  *sbuffer);

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//  Procedure that initializes the viewfinder
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

buffer_sprite3d *visor_init(lptvisor datos_render)
{
  if (lib_3d)
    visor_end();

  //---------------------------------------------------------------------------
  //  Inicializo el modulo matematico
  //---------------------------------------------------------------------------
  IniMath();

  //---------------------------------------------------------------------------
  //  Crea el buffer del sprite
  //---------------------------------------------------------------------------
  if ((my_buffer=(buffer_sprite3d *)lf_malloc(sizeof(buffer_sprite3d)))==NULL)
    return(NULL);
  my_buffer->ancho = 256*datos_render->porcentaje/100;
  my_buffer->alto  = 256*datos_render->porcentaje/100;
  if ((my_buffer->buffer=(char *)lf_malloc(my_buffer->ancho*my_buffer->alto*2))==NULL)
    return(NULL);
  memset(my_buffer->buffer,0,my_buffer->ancho*my_buffer->alto*2);

  if ((sbuffer=(buffer_sprite3d *)lf_malloc(sizeof(buffer_sprite3d)))==NULL)
    return(NULL);
  sbuffer->ancho = 256*datos_render->porcentaje/100;
  sbuffer->alto  = 256*datos_render->porcentaje/100;
  if ((sbuffer->buffer=(char *)lf_malloc(sbuffer->ancho*sbuffer->alto*2))==NULL)
    return(NULL);
  memset(sbuffer->buffer,0,sbuffer->ancho*sbuffer->alto*2);

  //---------------------------------------------------------------------------
  //  Inicializo la camara y el objeto asociado
  //---------------------------------------------------------------------------

  puntero_textura=datos_render->textura;
  puntero_dac=datos_render->dac;

  fileanim_create(&fichero_animaciones,datos_render->animacion);
  if (num_mallocs==0) return(NULL);

  sprite3d_create(&camara,my_buffer->buffer,my_buffer->ancho,my_buffer->alto);
  if (num_mallocs==0) return(NULL);
  sprite3d_setposition(&camara,0,100,-800);
  sprite3d_setrotation(&camara,0,0,0);
  sprite3d_angulosvision(&camara,30,20);
  sprite3d_look(&camara,0,100,0);

  hlrender_create(&world_camara,&camara);
  if (num_mallocs==0) return(NULL);
  hlrender_setlight(&world_camara,-10,5,-10);

//  resource_create(&resource_camara,datos_render->textura);
  animated_create(&animado,&fichero_animaciones,&world_camara,datos_render->nombre,&resource_camara);
  	printf("115: name: %s\n",animado.complex_struct.Objects[0]->name);

  if (num_mallocs==0) return(NULL);

  lib_3d=1;

  return(sbuffer);
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//  Procedure that manages the viewfinder
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void visor_loop(lptvisor datos_render)
{
  int i;

  if (!lib_3d)
    return;

  //lfset(my_buffer->buffer,0xf81f,my_buffer->ancho*my_buffer->alto);
  memset(my_buffer->buffer,0,my_buffer->ancho*my_buffer->alto*2);

  //---------------------------------------------------------------------------
  //  Convierto de grados en radianes
  //---------------------------------------------------------------------------
  datos_render->alfa=(datos_render->alfa * 3.141592) / 180;
  datos_render->beta=(datos_render->beta * 3.141592) / 180;

  //-------------------------------------------------------------------------
  //  Actualizo el objeto y lo pinto
  //-------------------------------------------------------------------------
  animated_setanim(&animado,datos_render->num_animacion);
  animado.complex_struct.curframe=datos_render->frame+1;

  complex_setposition(&animado.complex_struct,0,0,0);
  complex_setmaptype(&animado.complex_struct,TEXT_GOUR);

  complex_setrotation(&animado.complex_struct,datos_render->beta,datos_render->alfa,0);
  complex_update(&animado.complex_struct);
  hlrender_transformobjects(&world_camara);

  complex_sortdraw(&animado.complex_struct);
  if (num_mallocs==0) return;

  datos_render->num_frames=animado.complex_struct.nFrames-1;

  sbuffer->punto_x=10;
  sbuffer->punto_y=10;

  for (i=0;i<animado.complex_struct.nObjects;i++) {
	  printf("name: %s\n",animado.complex_struct.Objects[i]->name);
    if (!stricmp(animado.complex_struct.Objects[i]->name,"control") || i==animado.complex_struct.nObjects-1) {
      sbuffer->punto_x=(int)animado.complex_struct.Objects[i]->Centro.fpx-animado.complex_struct.bbox_x_ini;
      sbuffer->punto_y=(int)animado.complex_struct.Objects[i]->Centro.fpy-animado.complex_struct.bbox_y_ini;
      break;
    }
  }

  reescalar_sprite3d(my_buffer);
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//  Procedimiento que finaliza el visor
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void visor_end()
{
  if (!lib_3d)
    return;

  //---------------------------------------------------------------------------
  //  Libero la memoria reservada para el menu
  //---------------------------------------------------------------------------
  hlrender_destroy(&world_camara);
  animated_destroy(&animado);
//  resource_destroy(&resource_camara);
  fileanim_destroy(&fichero_animaciones);

  lf_free(sbuffer->buffer);
  lf_free(sbuffer);
  lf_free(my_buffer->buffer);
  lf_free(my_buffer);

  lf_free_all();
  lib_3d=0;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
//  Procedimiento que reescala un sprite ajustando al borde
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

void reescalar_sprite3d(buffer_sprite3d  *wbuffer)
{
  int i,j;
  int ancho=(animado.complex_struct.bbox_x_fin-animado.complex_struct.bbox_x_ini+1)*2;
  int alto=animado.complex_struct.bbox_y_fin-animado.complex_struct.bbox_y_ini+1;

  if (ancho&2) ancho+=2;
  if (alto&1) {
    alto++;
    animado.complex_struct.bbox_y_fin++;
  }

  lf_free(sbuffer->buffer);
  if ((sbuffer->buffer=(char *)lf_malloc(ancho*alto))==NULL)
    return;

  j=0;
  for (i=animado.complex_struct.bbox_y_ini;i<=animado.complex_struct.bbox_y_fin;i++) {
    memcpy(sbuffer->buffer+j*ancho,wbuffer->buffer+i*wbuffer->ancho*2+animado.complex_struct.bbox_x_ini*2,ancho);
    j++;
  }

  sbuffer->ancho=ancho/2;
  sbuffer->alto=alto;
}
