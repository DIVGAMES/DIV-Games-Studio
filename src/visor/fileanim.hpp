#ifndef __FILEANIM__HPP_
#define __FILEANIM__HPP_

#include "struct.hpp"

typedef struct _a3d_header{
  char  Chunk[4];
  short Version;
  short nObjects;
  short nAnims;
  short AUX;
} a3d_header;

typedef struct _a3d_anim{
  int   offset;
  short nFrames;
  short AUX;
} a3d_anim;

typedef struct {
  a3d_header  header;
  a3d_anim    *anims;
  lptmatrix   buffer;
} fileanim;

void fileanim_create(fileanim *fileanim_struct,char *name);
void fileanim_destroy(fileanim *fileanim_struct);
lptmatrix fileanim_getanim(fileanim *fileanim_struct,int order,short *frames);

#endif
