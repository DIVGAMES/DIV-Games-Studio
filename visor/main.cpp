#include <stdio.h>
#include <conio.h>
#include <i86.h>
#include <ctype.h>
#include <math.h>

#include "video.hpp"
#include "resource.hpp"
#include "global.hpp"
#include "hlrender.hpp"
#include "complex.hpp"
#include "animated.hpp"
#include "visor.hpp"

Video     *myvideo;
char      *ERROR;

void end_system();
void ShowVesa20();

void init_system()
{
  ERROR=NULL;

  IniMath();
  myvideo =new Video(640,480,16);

  onexit(end_system);

}

void end_system()
{
  delete myvideo;
  SetVideoMode(3);
  if(ERROR!=NULL) printf(ERROR);
}

void main()
{
  tvisor datos_render;

  //---------------------------------------------------------------------------
  //  Inicializo el sistema
  //---------------------------------------------------------------------------
  init_system();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  datos_render.tipo=0;
  datos_render.porcentaje=100;
  datos_render.alfa=1.57;
  datos_render.beta=0.78;
  datos_render.animacion="anims\\anim.a3d";
  datos_render.num_animacion=1;
  datos_render.frame=7;
  datos_render.nombre="models\\tenista.o3d";
  datos_render.dac=NULL;
  datos_render.textura="data\\tenista.dat";

  //---------------------------------------------------------------------------
  //  Activo el visor
  //---------------------------------------------------------------------------
  visor(&datos_render);
}
