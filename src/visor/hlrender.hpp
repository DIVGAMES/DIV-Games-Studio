#ifndef __HLRENDER_HPP_
#define __HLRENDER_HPP_

//#include <mem.h>
#include <math.h>
#include "llrender.hpp"
#include "struct.hpp"
#include "global.hpp"
#include "sprite3d.hpp"

#define N_OBJECTS 1024

typedef struct {
  sprite3d    *ccamera;
  lptobject   objects[N_OBJECTS]; // Array of 128 objects.
  lptobject   wobjects[N_OBJECTS];// Array of 128 objects.
  int         nobjects;           // Number of objects.
  int         ppf,oldppf;

  // Variables for lights
  float       lgtx,lgty,lgtz;
  float       mlgtx,mlgty,mlgtz;
  // Variables for lights
  float       center_screen_x;
  float       center_screen_y;

  llrender    llrender_struct;

} hlrender;

void hlrender_create(hlrender *hlrender_struct,sprite3d *newcamera);
void hlrender_destroy(hlrender *hlrender_struct);

void hlrender_rotate(hlrender *hlrender_struct,lptvertex v,lptmatrix matrix);
void hlrender_rotate_normals(lptvertex v,lptmatrix matrix);
void hlrender_transform(hlrender *hlrender_struct,lptobject obj);
void hlrender_transform_shadow(hlrender *hlrender_struct,lptobject obj);

void hlrender_transform_center(hlrender *hlrender_struct,lptobject obj);
int  hlrender_addobject(hlrender *hlrender_struct,lptobject object);
void hlrender_deleteobject(hlrender *hlrender_struct,lptobject object);
void hlrender_modifyobject(hlrender *hlrender_struct,int,lptmatrix matrix);
//void hlrender_deleteobject(hlrender *hlrender_struct,int);

void hlrender_sort(hlrender *hlrender_struct);

void hlrender_transformobjects(hlrender *hlrender_struct);
void hlrender_transdrawobjects(hlrender *hlrender_struct);
void hlrender_drawobjects(hlrender *hlrender_struct);
void hlrender_drawshadowobject(hlrender *hlrender_struct,lptobject object);
void hlrender_drawobject(hlrender *hlrender_struct,lptobject object);
void hlrender_setcamera(hlrender *hlrender_struct,sprite3d *newcamera);

void hlrender_setlight(hlrender *hlrender_struct,float x, float y, float z);
void hlrender_setmovelight(hlrender *hlrender_struct,float x, float y, float z);

#endif

