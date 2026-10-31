#ifndef __LLRENDER_CPP_
#define __LLRENDER_CPP_

#include "struct.hpp"
#include "sprite3d.hpp"

typedef struct {
    int     dist_max;
    int     mmdist[2];
    int     screen_width;
    int     screen_height;
    float   screen_fwidth;
    float   screen_fheight;
    int     vwidth;
    char    *copy_buffer;
    char    *end_copy;

    float   projection_x;     // Lens type.
    float   projection_y;     // Lens type.

} llrender;

void llrender_Draw_Triangle(llrender *llrender_struct,lptface face, int mmlevel, int type);
int  llrender_Hidden_Face(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_near(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_left(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_right(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_up(llrender *llrender_struct,lptface face);
void llrender_clipping_3d_down(llrender *llrender_struct,lptface face);
void llrender_draw_triangle(llrender *llrender_struct,lptface face);

void llrender_create(llrender *llrender_struct,sprite3d *newcamera); //Video *vid);
void llrender_destroy(llrender *llrender_struct);
void llrender_render(llrender *llrender_struct,lptface faces,int nfaces);

extern int num_mallocs;

#endif

