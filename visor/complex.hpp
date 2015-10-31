#ifndef __COMPLEX_HPP_
#define __COMPLEX_HPP_
#define MAXOBJECTS  128

#include "hlrender.hpp"
#include "resource.hpp"
#include "global.hpp"

typedef struct {
        float x,y,z;
        int             activo;
        int             Clon;
        int             Shadow;
        int             AIncrustada;
        float           angx,angy,angz;
        hlrender        *hl_render;
        short           nObjects;
        lptobject       Objects[MAXOBJECTS];
        lptobject       wobjects[MAXOBJECTS];
        short           nMaterials;
        lptmaterial     Materials;
        short           nDummies;
        lptbbox         Dummies;
        short           nTapes;
        lpttape         Tapes;
        short           nFrames;
        lptmatrix       Anims;
        tmatrix         CompMat;
        int            curframe;
        int            bbox_x_ini; //Coordenadas de pantalla de la bounding box
        int            bbox_x_fin;
        int            bbox_y_ini;
        int            bbox_y_fin;
} complex;

void complex_create(complex *complex_struct,hlrender *hlrnd,char *name,Resource *res);
void complex_destroy(complex *complex_struct);

void complex_update(complex *complex_struct);
void complex_setposition(complex *complex_struct,float cx,float cy,float cz);
void complex_setrotation(complex *complex_struct,float cangx,float cangy,float cangz);
void complex_transform(complex *complex_struct);
void complex_sort(complex *complex_struct);
void complex_draw(complex *complex_struct);
void complex_sortdraw(complex *complex_struct);
void complex_drawshadow(complex *complex_struct);
void complex_setmaptype(complex *complex_struct,int type);
void complex_setambientlight(complex *complex_struct,float intensidad); //Valores entre -1.0 y 1.0
void complex_setcolor(complex *complex_struct,float r, float g, float b);
void complex_makefacenormals(complex *complex_struct);
void complex_look(complex *complex_struct,float nx,float ny,float nz);
void complex_active_object(complex *complex_struct,int act);

typedef struct _wvertice{
  float x,y,z;
  float nx,ny,nz;
  float u,v;
}wvertice;

typedef struct _wface{
  short a,b,c;
  short matidx;
}wface;

typedef struct _wmaterial{
  float dr,dg,db;
  float ar,ag,ab;
  float sr,sg,sb;
  int   transp;
  char  textura[16];
  int   map_type;
}wmaterial;

typedef struct { float x, y, z;
               } punto_3d;

#define TWIRE      0
#define TFLAT      1
#define TGOURAUD   2
#define TPHONG     3
#define TMETAL     4

#endif
