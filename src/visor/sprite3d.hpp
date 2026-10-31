#ifndef __SPRITE3D_HPP_
#define __SPRITE3D_HPP_

#include <math.h>
#include "global.hpp"

typedef struct {
    float       cx,cy,cz;           // Camera coordinates.
    float       cangx,cangy,cangz;  // Camera angles.
    tmatrix     cmatrix;            // Camera matrix.
    int         width;              // Buffer width
    int         height;               // Buffer height
    char        *buffer;            // Dump buffer
    float       projection_x;       // Lens type.
    float       projection_y;       // Lens type.

    int   nTexturesLoad;
    lptexturebuffer texturesload[256];

} sprite3d;

void sprite3d_create(sprite3d *sprite3d_struct,char *vbuffer,long width,long height);
// void sprite3d_destroy(sprite3d *sprite3d_struct);

void sprite3d_setrotation(sprite3d *sprite3d_struct,float angx,float angy,float angz);
void sprite3d_setposition(sprite3d *sprite3d_struct,float x,float y,float z);
void sprite3d_look(sprite3d *sprite3d_struct,float x,float y,float z);
void sprite3d_visionangles(sprite3d *sprite3d_struct,float anglex,float angley);
void sprite3d_transform(sprite3d *sprite3d_struct);

lpttexture sprite3d_SeekIfTextureLoad(sprite3d *sprite3d_struct,char *name,Resource *res);
lpttexture sprite3d_CreateTexture(sprite3d *sprite3d_struct,Resource *res,char *name);
void sprite3d_DestroyTexture(sprite3d *sprite3d_struct,lpttexture txt);

#endif
