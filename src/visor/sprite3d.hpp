#ifndef __SPRITE3D_HPP_
#define __SPRITE3D_HPP_

#include <math.h>
#include "global.hpp"

typedef struct {
    float       cx,cy,cz;           // Coordenadas de la camara.
    float       cangx,cangy,cangz;  // Angulos de la camara.
    tmatrix     cmatrix;            // Matriz de la camara.
    int         ancho;              // Ancho del buffer
    int         alto;               // Alto del buffer
    char        *buffer;            // Buffer de volcado
    float       proyeccion_x;       // Tipo de lente.
    float       proyeccion_y;       // Tipo de lente.

    int   nTexturesLoad;
    lptexturebuffer texturesload[256];

} sprite3d;

void sprite3d_create(sprite3d *sprite3d_struct,char *vbuffer,long ancho,long alto);
// void sprite3d_destroy(sprite3d *sprite3d_struct);

void sprite3d_setrotation(sprite3d *sprite3d_struct,float angx,float angy,float angz);
void sprite3d_setposition(sprite3d *sprite3d_struct,float x,float y,float z);
void sprite3d_look(sprite3d *sprite3d_struct,float x,float y,float z);
void sprite3d_angulosvision(sprite3d *sprite3d_struct,float angulox,float anguloy);
void sprite3d_transform(sprite3d *sprite3d_struct);

lpttextura sprite3d_SeekIfTextureLoad(sprite3d *sprite3d_struct,char *name,Resource *res);
lpttextura sprite3d_CreateTexture(sprite3d *sprite3d_struct,Resource *res,char *name);
void sprite3d_DestroyTexture(sprite3d *sprite3d_struct,lpttextura txt);

#endif
