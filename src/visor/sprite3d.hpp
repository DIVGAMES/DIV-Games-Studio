#ifndef __SPRITE3D_HPP_
#define __SPRITE3D_HPP_

#include <math.h>
#include "global.hpp"

typedef struct {
    float       cx,cy,cz;           // Coordenadas de la camara.
    float       cangx,cangy,cangz;  // Angulos de la camara.
    tmatrix     cmatrix;            // Matriz de la camara.
    int         width;              // Ancho del buffer
    int         height;               // Alto del buffer
    char        *buffer;            // Buffer de volcado
    float       projection_x;       // Tipo de lente.
    float       projection_y;       // Tipo de lente.

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
