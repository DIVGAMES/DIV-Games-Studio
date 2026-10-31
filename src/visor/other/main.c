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

  InitMath();
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
  tviewer render_data;

  //---------------------------------------------------------------------------
  //  Inicializo el sistema
  //---------------------------------------------------------------------------
  init_system();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  render_data.type=0;
  render_data.percentage=100;
  render_data.alpha=1.57;
  render_data.beta=0.78;
  render_data.animation="anims\\anim.a3d";
  render_data.num_animation=1;
  render_data.frame=7;
  render_data.name="models\\tenista.o3d";
  render_data.dac=NULL;
  render_data.texture="data\\tenista.dat";

  //---------------------------------------------------------------------------
  //  Activo el visor
  //---------------------------------------------------------------------------
  viewer(&render_data);
}
