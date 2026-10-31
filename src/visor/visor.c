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

animated      animation;
fileanim      animations_file;
Resource      resource_camera;
hlrender      world_camera;
sprite3d      camera;
buffer_sprite3d  *sbuffer=NULL;
buffer_sprite3d  *my_buffer=NULL;

char *texture_pointer;
char *dac_pointer;

int lib_3d=0;
int lib_3d_width=0;
int lib_3d_height=0;
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

void rescale_sprite3d(buffer_sprite3d  *sbuffer);

//─────────────────────────────────────────────────────────────────────────────
//  Procedure that initializes the viewfinder
//─────────────────────────────────────────────────────────────────────────────

buffer_sprite3d *viewer_init(lptviewer render_data)
{
  if (lib_3d)
    viewer_end();

  //---------------------------------------------------------------------------
  //  Inicializo el modulo matematico
  //---------------------------------------------------------------------------
  InitMath();

  //---------------------------------------------------------------------------
  //  Crea el buffer del sprite
  //---------------------------------------------------------------------------
  if ((my_buffer=(buffer_sprite3d *)lf_malloc(sizeof(buffer_sprite3d)))==NULL)
    return(NULL);
  my_buffer->width = 256*render_data->percentage/100;
  my_buffer->height  = 256*render_data->percentage/100;
  if ((my_buffer->buffer=(char *)lf_malloc(my_buffer->width*my_buffer->height*2))==NULL)
    return(NULL);
  memset(my_buffer->buffer,0,my_buffer->width*my_buffer->height*2);

  if ((sbuffer=(buffer_sprite3d *)lf_malloc(sizeof(buffer_sprite3d)))==NULL)
    return(NULL);
  sbuffer->width = 256*render_data->percentage/100;
  sbuffer->height  = 256*render_data->percentage/100;
  if ((sbuffer->buffer=(char *)lf_malloc(sbuffer->width*sbuffer->height*2))==NULL)
    return(NULL);
  memset(sbuffer->buffer,0,sbuffer->width*sbuffer->height*2);

  //---------------------------------------------------------------------------
  //  Inicializo la camara y el objeto asociado
  //---------------------------------------------------------------------------

  texture_pointer=render_data->texture;
  dac_pointer=render_data->dac;

  fileanim_create(&animations_file,render_data->animation);
  if (num_mallocs==0) return(NULL);

  sprite3d_create(&camera,my_buffer->buffer,my_buffer->width,my_buffer->height);
  if (num_mallocs==0) return(NULL);
  sprite3d_setposition(&camera,0,100,-800);
  sprite3d_setrotation(&camera,0,0,0);
  sprite3d_visionangles(&camera,30,20);
  sprite3d_look(&camera,0,100,0);

  hlrender_create(&world_camera,&camera);
  if (num_mallocs==0) return(NULL);
  hlrender_setlight(&world_camera,-10,5,-10);

//  resource_create(&resource_camera,render_data->texture);
  animated_create(&animation,&animations_file,&world_camera,render_data->name,&resource_camera);
  	printf("115: name: %s\n",animation.complex_struct.Objects[0]->name);

  if (num_mallocs==0) return(NULL);

  lib_3d=1;

  return(sbuffer);
}

//─────────────────────────────────────────────────────────────────────────────
//  Procedure that manages the viewfinder
//─────────────────────────────────────────────────────────────────────────────

void viewer_loop(lptviewer render_data)
{
  int i;

  if (!lib_3d)
    return;

  //lfset(my_buffer->buffer,0xf81f,my_buffer->width*my_buffer->height);
  memset(my_buffer->buffer,0,my_buffer->width*my_buffer->height*2);

  //---------------------------------------------------------------------------
  //  Convierto de grados en radianes
  //---------------------------------------------------------------------------
  render_data->alpha=(render_data->alpha * 3.141592) / 180;
  render_data->beta=(render_data->beta * 3.141592) / 180;

  //-------------------------------------------------------------------------
  //  Actualizo el objeto y lo pinto
  //-------------------------------------------------------------------------
  animated_setanim(&animation,render_data->num_animation);
  animation.complex_struct.curframe=render_data->frame+1;

  complex_setposition(&animation.complex_struct,0,0,0);
  complex_setmaptype(&animation.complex_struct,TEXT_GOUR);

  complex_setrotation(&animation.complex_struct,render_data->beta,render_data->alpha,0);
  complex_update(&animation.complex_struct);
  hlrender_transformobjects(&world_camera);

  complex_sortdraw(&animation.complex_struct);
  if (num_mallocs==0) return;

  render_data->num_frames=animation.complex_struct.nFrames-1;

  sbuffer->point_x=10;
  sbuffer->point_y=10;

  for (i=0;i<animation.complex_struct.nObjects;i++) {
	  printf("name: %s\n",animation.complex_struct.Objects[i]->name);
    if (!stricmp(animation.complex_struct.Objects[i]->name,"control") || i==animation.complex_struct.nObjects-1) {
      sbuffer->point_x=(int)animation.complex_struct.Objects[i]->Center.fpx-animation.complex_struct.bbox_x_start;
      sbuffer->point_y=(int)animation.complex_struct.Objects[i]->Center.fpy-animation.complex_struct.bbox_y_start;
      break;
    }
  }

  rescale_sprite3d(my_buffer);
}

//─────────────────────────────────────────────────────────────────────────────
//  Procedimiento que finaliza el visor
//─────────────────────────────────────────────────────────────────────────────

void viewer_end()
{
  if (!lib_3d)
    return;

  //---------------------------------------------------------------------------
  //  Libero la memoria reservada para el menu
  //---------------------------------------------------------------------------
  hlrender_destroy(&world_camera);
  animated_destroy(&animation);
//  resource_destroy(&resource_camera);
  fileanim_destroy(&animations_file);

  lf_free(sbuffer->buffer);
  lf_free(sbuffer);
  lf_free(my_buffer->buffer);
  lf_free(my_buffer);

  lf_free_all();
  lib_3d=0;
}

//─────────────────────────────────────────────────────────────────────────────
//  Procedimiento que reescala un sprite ajustando al borde
//─────────────────────────────────────────────────────────────────────────────

void rescale_sprite3d(buffer_sprite3d  *wbuffer)
{
  int i,j;
  int width=(animation.complex_struct.bbox_x_end-animation.complex_struct.bbox_x_start+1)*2;
  int height=animation.complex_struct.bbox_y_end-animation.complex_struct.bbox_y_start+1;

  if (width&2) width+=2;
  if (height&1) {
    height++;
    animation.complex_struct.bbox_y_end++;
  }

  lf_free(sbuffer->buffer);
  if ((sbuffer->buffer=(char *)lf_malloc(width*height))==NULL)
    return;

  j=0;
  for (i=animation.complex_struct.bbox_y_start;i<=animation.complex_struct.bbox_y_end;i++) {
    memcpy(sbuffer->buffer+j*width,wbuffer->buffer+i*wbuffer->width*2+animation.complex_struct.bbox_x_start*2,width);
    j++;
  }

  sbuffer->width=width/2;
  sbuffer->height=height;
}
