#ifndef __LLRENDER_CPP_
#define __LLRENDER_CPP_

#include "struct.hpp"
#include "sprite3d.hpp"

typedef struct {
    int     dist_max;
    int     mmdist[2];
    int     ancho_pantalla;
    int     alto_pantalla;
    float   fancho_pantalla;
    float   falto_pantalla;
    int     vancho;
    char    *copia;
    char    *fin_copia;

    float   proyeccion_x;     // Tipo de lente.
    float   proyeccion_y;     // Tipo de lente.

} llrender;

void llrender_Pinta_Triangulo(llrender *llrender_struct,lptface face, int mmlevel, int tipo);
int  llrender_Cara_Oculta(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_near(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_left(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_right(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_up(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_down(llrender *llrender_struct,lptface face);
void llrender_pintar_triangulo(llrender *llrender_struct,lptface face);

void llrender_create(llrender *llrender_struct,sprite3d *newcamera); //Video *vid);
void llrender_destroy(llrender *llrender_struct);
void llrender_render(llrender *llrender_struct,lptface faces,int nfaces);

extern int num_mallocs;

#endif

