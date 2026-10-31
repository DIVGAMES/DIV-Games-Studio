//#include <mem.h>
#include <math.h>
//#include <graph.h>
#include "llrender.hpp"
#include "global.hpp"
#include "t.h"
#include "stdint.h"

long complex_x_start,complex_x_end;
long complex_y_start,complex_y_end;

short *rgba_table;
long num_llrenders=0;


float LMax=1.8; //1.8
float LMin=0.7; //0.7

//extern "C" {
  //extern 
  int texture_address;
  extern int clock1,clock2;
//}

#define PI ((float)3.141592)

int u_1,v_1,u_0,v_0;
float fu,fv,inverse;

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------

    int left_section, right_section;
    int left_section_height, right_section_height;
    int drdx, dgdx, dbdx,dintensitydx;
    int u_left, du_left, v_left, dv_left;
    int x_left, dx_left, x_right, dx_right;
    int r_left, dr_left, g_left, dg_left, b_left, db_left;
    int intensity_left,dintensity_left;
    float duz_left, dvz_left, unoz_left, dunoz_left;
    float duz_right, dvz_right, unoz_right, dunoz_right;
    float uz_left,vz_left,uz_right,vz_right;

//-----------------------------------------------------------------------------

long adjust_float(float x) //inline
{
  return ((schop(x*65536.0)+0xffff)>>16);
}

long adjust_integer(long x) //inline
{
  x += 0xffff;
  return (x>>16);
}

void (*kernel_textures)(int v, int u, int du, int dv, int width, short *destination);

int clocks();
#ifdef DOS
#pragma aux clocks =  \
        "push edx"    \
        "db 0fh,31h"  \
        "pop edx"     \
        value [eax]   \

#endif

void start_division(float *);
#ifdef DOS

#pragma aux start_division = \
        "fld1"                \
        "fdiv dword ptr[EAX]" \
        parm caller [EAX]     \

#endif

void end_division();

#ifdef DOS

#pragma aux end_division =    \
        "fstp dword ptr[inversa]" \

#endif

void assignment_u_v();

#ifdef DOS

#pragma aux assignment_u_v =      \
        "fld dword ptr[inversa]"  \
        "fld st(0)"               \
        "fmul dword ptr[fu]"      \
        "fistp dword ptr[u_0]"    \
        "fmul dword ptr[fv]"      \
        "fistp dword ptr[v_0]"    \

#endif
void lf_set(void *, long, long);

#ifdef DOS

#pragma aux lf_set = \
	"rep stosw" \
        parm caller [edi] [eax] [ecx]\
	modify exact [edi eax ecx] \

#endif
//=============================================================================

short *destination;
short color_b;
int mr,mg,mb,intensity,alpha;

void kernel1();

#ifdef DOS

#pragma aux kernel1 = \
        "pushad" \
        "mov edi,destino" \
        "mov esi,tabla_rgba" \
        "xor edx,edx" \
        "mov ebx,mr" \
        "shr ebx,3" \
        "and ebx,7e0h" \
        "mov ax,color_b" \
        "and eax,0f800h" \
        "add eax,ebx" \
        "add eax,alfa" \
        "mov dx,[esi+eax*2]" \
        "shl edx,5" \
        "mov ebx,mg" \
        "shr ebx,3" \
        "and ebx,7e0h" \

#endif

void kernel2();

#ifdef DOS
#pragma aux kernel2 = \
        "mov ax,color_b" \
        "and eax,07c0h" \
        "shl eax,5" \
        "add eax,ebx" \
        "add eax,alfa" \
        "add dx,[esi+eax*2]" \
        "shl edx,6" \
        "mov ebx,mb" \
        "shr ebx,3" \
        "and ebx,7e0h" \
        "mov ax,color_b" \
        "and eax,1fh" \
        "shl eax,11" \
        "add eax,ebx" \
        "add eax,alfa" \
        "add dx,[esi+eax*2]" \
        "mov [edi],dx" \
        "popad" \

#endif

//=============================================================================

void llrender_create(llrender *llrender_struct,sprite3d *newcamera)
{
  short alpha,color,gouraud;
  int i,c;
  float t;

  llrender_struct->dist_max=27000;
//  video=vid;
  llrender_struct->mmdist[0]=llrender_struct->dist_max*0.33;
  llrender_struct->mmdist[1]=llrender_struct->dist_max*0.66;
  for (i=0;i<256;i++)
// Changing data
  llrender_struct->screen_width=newcamera->width; //video->Width;
  llrender_struct->screen_height=newcamera->height; //video->Height;
  llrender_struct->screen_fwidth=llrender_struct->screen_width;
  llrender_struct->screen_fheight = llrender_struct->screen_height;
  llrender_struct->copy_buffer=newcamera->buffer; //video->Buffer;
  llrender_struct->vwidth=newcamera->width*2;
  llrender_struct->end_copy=llrender_struct->copy_buffer+llrender_struct->vwidth*llrender_struct->screen_height;

  if (num_llrenders==0) {

    rgba_table=(short *)lf_malloc(65536*2);

    for (i=0;i<65536;i++) {

      alpha=(short)(i&0x1f);         //0-32 Lighting level
      color=(short)((i>>10)&0x3e);  //0-64 Texture color
      gouraud=(short)((i>>5)&0x3f); //0-64 Lighting color

    	t = (LMax-LMin)*((float)alpha/32.0) + LMin;
      c = (int)(t*(float)color);
      rgba_table[i]=(short)( c > 63 ? 31 : c>>1 );

    //  rgba_table[i]=(((i>>10)&0x3e)*(31-alpha)+((i>>5)&0x3f)*alpha)/64;
    }
  }
  num_llrenders++;
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_destroy(llrender *llrender_struct)
{
  num_llrenders--;
  if (num_llrenders==0) {
    lf_free(rgba_table);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

int compare(const void *op1, const void *op2)
{
  return ((*((const lptface *)op1))->v1->fpz<(*((const lptface *)op2))->v1->fpz);
}

/*
int compare(const void *op1, const void *op2)
{
  const lptface p1= *((const lptface *)op1);
  const lptface p2= *((const lptface *)op2);
  return (p1->v1->fpz<p2->v1->fpz);
}
*/

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_render(llrender *llrender_struct,lptface faces,int nfaces)
{
  int i;
  lptface *face_list;

  if ((face_list=(lptface *)lf_malloc(sizeof(lptface)*nfaces))==NULL)
    return;

  for (i=0;i<nfaces;i++)
    face_list[i]=&faces[i];

  qsort(face_list,nfaces,sizeof(lptface *),compare);

  i=0;
  do {
    llrender_clipping_3d_near(llrender_struct,face_list[i]);
//    llrender_clipping_3d_near(llrender_struct,&faces[i]);
  } while (++i<nfaces);

  lf_free(face_list);
}

//─────────────────────────────────────────────────────────────────────────────
//  Draw a triangle in the video buffer
//─────────────────────────────────────────────────────────────────────────────

void llrender_draw_triangle(llrender *llrender_struct,lptface face)
{
int mmlvl;
int zmed;
//int color;

  if (face->map_type==-1) return;

  if (face->v1->state==0) {
    face->v1->px=adjust_float(face->v1->fpx);
    face->v1->py=adjust_float(face->v1->fpy);
    face->v1->state=1;
  }

  if (face->v2->state==0) {
    face->v2->px=adjust_float(face->v2->fpx);
    face->v2->py=adjust_float(face->v2->fpy);
    face->v2->state=1;
  }

  if (face->v3->state==0) {
    face->v3->px=adjust_float(face->v3->fpx);
    face->v3->py=adjust_float(face->v3->fpy);
    face->v3->state=1;
  }

  //---------------------------------------------------------------------------
  //  Compute the bounding box of the object on screen
  //---------------------------------------------------------------------------

  if (face->v1->px<complex_x_start) complex_x_start=face->v1->px;
  if (face->v2->px<complex_x_start) complex_x_start=face->v2->px;
  if (face->v3->px<complex_x_start) complex_x_start=face->v3->px;
  if (face->v1->px>complex_x_end) complex_x_end=face->v1->px+1;
  if (face->v2->px>complex_x_end) complex_x_end=face->v2->px+1;
  if (face->v3->px>complex_x_end) complex_x_end=face->v3->px+1;
  if (face->v1->py<complex_y_start) complex_y_start=face->v1->py;
  if (face->v2->py<complex_y_start) complex_y_start=face->v2->py;
  if (face->v3->py<complex_y_start) complex_y_start=face->v3->py;
  if (face->v1->py>complex_y_end) complex_y_end=face->v1->py+1;
  if (face->v2->py>complex_y_end) complex_y_end=face->v2->py+1;
  if (face->v3->py>complex_y_end) complex_y_end=face->v3->py+1;

  // If the polygon does not face the camera, it is not visible and therefore not drawn
  if( llrender_Hidden_Face(llrender_struct,face) ) return;
/*
  #ifdef DEBUG
    line(face->v1->px, face->v1->py, face->v2->px, face->v2->py, 0xFFFF);
    line(face->v2->px, face->v2->py, face->v3->px, face->v3->py, 0xFFFF);
    line(face->v3->px, face->v3->py, face->v1->px, face->v1->py, 0xFFFF);
  #else

  if (face->map_type==WIRE) {
    color=(schop(face->material->dr*65536.0)&0xf800)+
          (schop(face->material->dg*2048.0)&0x07e0)+
          (schop(face->material->db*32.0)&0x1f);
    line(face->v1->px, face->v1->py, face->v2->px, face->v2->py, color);
    line(face->v2->px, face->v2->py, face->v3->px, face->v3->py, color);
    line(face->v3->px, face->v3->py, face->v1->px, face->v1->py, color);
    return;
  }
*/
  zmed=schop(face->v1->fpz+face->v2->fpz+face->v3->fpz)>>2;

  //  Compute the Mipmap associated with the texture
  if(zmed<llrender_struct->mmdist[0])
    mmlvl=0;
  else
    if(zmed<llrender_struct->mmdist[1])
      mmlvl=1;
    else
      if(zmed<llrender_struct->dist_max)
	      mmlvl=2;
      else
	      mmlvl=3;


  switch (face->map_type) {
    case FLAT:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_FLAT);
      break;
    case ANTIALIAS:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_FLAT);
      break;
    case GOURAUD:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_GOURAUD);
      break;
    case TEXTURE:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE);
      break;
    case TEXT_GOUR:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_GOURAUD);
      break;
    case TEXT_ILLUM:
//      Pinta_Text_Ilum(face,mmlvl);
//      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_TRANSLUCENT);
      break;
    case TEXT_PER:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_PERSPECTIVE);
      break;
    case TEXT_PER_TRANS:
//      Pinta_Text_Persp_Translucido(face,mmlvl);
      break;
    case TEXT_TRANS:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_TRANSLUCENT);
      break;
    case SHADOW:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_FLAT);
      break;
    case TRANS_PER:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_PERSPECTIVE | T_MASK);
      break;
    case TEXT_BILINEAR:
//      Pinta_Text_Persp_Bilinear(face,mmlvl);
      break;
    case TRANSPARENT:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_MASK);
      break;
    case TRANSPARENT_MASK:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_TRANSLUCENT | T_MASK);
      break;
    case FLAT_TRANS:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_FLAT | T_TRANSLUCENT);
      break;
    case GOUR_MASK:
      llrender_Draw_Triangle(llrender_struct,face,mmlvl,T_TEXTURE | T_MASK | T_GOURAUD);
      break;
  }

//  #endif
}

//─────────────────────────────────────────────────────────────────────────────
//  Check whether a face is hidden
//─────────────────────────────────────────────────────────────────────────────

int llrender_Hidden_Face(llrender *llrender_struct,lptface face)
{
  float scalar_product;

  scalar_product= (face->v2->fpx - face->v1->fpx)*(face->v3->fpy - face->v1->fpy)-
                    (face->v2->fpy - face->v1->fpy)*(face->v3->fpx - face->v1->fpx);

  if (scalar_product<0) return(0);

  return 1;
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_near(llrender *llrender_struct,lptface face_in)
{
  long i=0;
  tface face;
  tvertex vertex[4];
  float fwork;

  //  Compute how many points lie behind the clipping plane
  i=face_in->v1->sutherland_cohen+face_in->v2->sutherland_cohen+face_in->v3->sutherland_cohen;
  if (!i) {
    llrender_draw_triangle(llrender_struct,face_in);
    return;
  }
  i=(i>>8)&3;
  if (i==3) return; // The polygon is off screen
  if (i==0) {
    llrender_clipping_3d_left(llrender_struct,face_in); // The polygon is inside the screen
    return;
  }

  memcpy(&face,face_in,sizeof(tface));
  face.v1->state=0;
  face.v2->state=0;
  face.v3->state=0;

  if (i==2) { // The result of the cut is a triangle
    // We start from a point located in front of the clipping plane
    if (face.v1->fpz>=1.0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpz>=1.0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    // Cut 1 with the clipping plane
    fwork=(1.0-vertex[0].fpz)/(vertex[1].fpz-vertex[0].fpz);
    vertex[1].sx=(vertex[1].sx-vertex[0].sx)*fwork+vertex[0].sx;
    vertex[1].sy=(vertex[1].sy-vertex[0].sy)*fwork+vertex[0].sy;
    vertex[1].u=(vertex[1].u-vertex[0].u)*fwork+vertex[0].u;
    vertex[1].v=(vertex[1].v-vertex[0].v)*fwork+vertex[0].v;
    vertex[1].r=(vertex[1].r-vertex[0].r)*fwork+vertex[0].r;
    vertex[1].g=(vertex[1].g-vertex[0].g)*fwork+vertex[0].g;
    vertex[1].b=(vertex[1].b-vertex[0].b)*fwork+vertex[0].b;
    vertex[1].intensity=(vertex[1].intensity-vertex[0].intensity)*fwork+vertex[0].intensity;
    // Project
    vertex[1].fpx=(llrender_struct->screen_fwidth/2)+(vertex[1].sx*llrender_struct->projection_x);
    vertex[1].fpy=(llrender_struct->screen_fheight/2)-(vertex[1].sy*llrender_struct->projection_y);
    vertex[1].fpz=1.0;
    vertex[1].sutherland_cohen=0;
    if (vertex[1].fpx<0.0) vertex[1].sutherland_cohen+=1;
    if (vertex[1].fpx>llrender_struct->screen_fwidth) vertex[1].sutherland_cohen+=4;
    if (vertex[1].fpy<0.0) vertex[1].sutherland_cohen+=16;
    if (vertex[1].fpy>llrender_struct->screen_fheight) vertex[1].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=(1.0-vertex[0].fpz)/(vertex[2].fpz-vertex[0].fpz);
    vertex[2].sx=(vertex[2].sx-vertex[0].sx)*fwork+vertex[0].sx;
    vertex[2].sy=(vertex[2].sy-vertex[0].sy)*fwork+vertex[0].sy;
    vertex[2].u=(vertex[2].u-vertex[0].u)*fwork+vertex[0].u;
    vertex[2].v=(vertex[2].v-vertex[0].v)*fwork+vertex[0].v;
    vertex[2].r=(vertex[2].r-vertex[0].r)*fwork+vertex[0].r;
    vertex[2].g=(vertex[2].g-vertex[0].g)*fwork+vertex[0].g;
    vertex[2].b=(vertex[2].b-vertex[0].b)*fwork+vertex[0].b;
    vertex[2].intensity=(vertex[2].intensity-vertex[0].intensity)*fwork+vertex[0].intensity;
    // Project
    vertex[2].fpx=(llrender_struct->screen_fwidth/2)+(vertex[2].sx*llrender_struct->projection_x);
    vertex[2].fpy=(llrender_struct->screen_fheight/2)-(vertex[2].sy*llrender_struct->projection_y);
    vertex[2].fpz=1.0;
    vertex[2].sutherland_cohen=0;
    if (vertex[2].fpx<0.0) vertex[2].sutherland_cohen+=1;
    if (vertex[2].fpx>llrender_struct->screen_fwidth) vertex[2].sutherland_cohen+=4;
    if (vertex[2].fpy<0.0) vertex[2].sutherland_cohen+=16;
    if (vertex[2].fpy>llrender_struct->screen_fheight) vertex[2].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_left(llrender_struct,&face);
  }
  else { // The result of the cut is two triangles
    // We start from a point located behind the clipping plane
    if (face.v1->fpz<1.0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpz<1.0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    // Cut 1 with the clipping plane
    fwork=(1.0-vertex[0].fpz)/(vertex[2].fpz-vertex[0].fpz);
    vertex[3].sx=(vertex[2].sx-vertex[0].sx)*fwork+vertex[0].sx;
    vertex[3].sy=(vertex[2].sy-vertex[0].sy)*fwork+vertex[0].sy;
    vertex[3].u=(vertex[2].u-vertex[0].u)*fwork+vertex[0].u;
    vertex[3].v=(vertex[2].v-vertex[0].v)*fwork+vertex[0].v;
    vertex[3].r=(vertex[2].r-vertex[0].r)*fwork+vertex[0].r;
    vertex[3].g=(vertex[2].g-vertex[0].g)*fwork+vertex[0].g;
    vertex[3].b=(vertex[2].b-vertex[0].b)*fwork+vertex[0].b;
    vertex[3].intensity=(vertex[2].intensity-vertex[0].intensity)*fwork+vertex[0].intensity;
    // Project
    vertex[3].fpx=(llrender_struct->screen_fwidth/2)+(vertex[3].sx*llrender_struct->projection_x);
    vertex[3].fpy=(llrender_struct->screen_fheight/2)-(vertex[3].sy*llrender_struct->projection_y);
    vertex[3].fpz=1.0;
    vertex[3].sutherland_cohen=0;
    if (vertex[3].fpx<0.0) vertex[3].sutherland_cohen+=1;
    if (vertex[3].fpx>llrender_struct->screen_fwidth) vertex[3].sutherland_cohen+=4;
    if (vertex[3].fpy<0.0) vertex[3].sutherland_cohen+=16;
    if (vertex[3].fpy>llrender_struct->screen_fheight) vertex[3].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=(1.0-vertex[0].fpz)/(vertex[1].fpz-vertex[0].fpz);
    vertex[0].sx=(vertex[1].sx-vertex[0].sx)*fwork+vertex[0].sx;
    vertex[0].sy=(vertex[1].sy-vertex[0].sy)*fwork+vertex[0].sy;
    vertex[0].u=(vertex[1].u-vertex[0].u)*fwork+vertex[0].u;
    vertex[0].v=(vertex[1].v-vertex[0].v)*fwork+vertex[0].v;
    vertex[0].r=(vertex[1].r-vertex[0].r)*fwork+vertex[0].r;
    vertex[0].g=(vertex[1].g-vertex[0].g)*fwork+vertex[0].g;
    vertex[0].b=(vertex[1].b-vertex[0].b)*fwork+vertex[0].b;
    vertex[0].intensity=(vertex[1].intensity-vertex[0].intensity)*fwork+vertex[0].intensity;
    // Project
    vertex[0].fpx=(llrender_struct->screen_fwidth/2)+(vertex[0].sx*llrender_struct->projection_x);
    vertex[0].fpy=(llrender_struct->screen_fheight/2)-(vertex[0].sy*llrender_struct->projection_y);
    vertex[0].fpz=1.0;
    vertex[0].sutherland_cohen=0;
    if (vertex[0].fpx<0.0) vertex[0].sutherland_cohen+=1;
    if (vertex[0].fpx>llrender_struct->screen_fwidth) vertex[0].sutherland_cohen+=4;
    if (vertex[0].fpy<0.0) vertex[0].sutherland_cohen+=16;
    if (vertex[0].fpy>llrender_struct->screen_fheight) vertex[0].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_left(llrender_struct,&face);
    face.v1=&vertex[0];
    face.v2=&vertex[2];
    face.v3=&vertex[3];
    face.v3->state=0;
    llrender_clipping_3d_left(llrender_struct,&face);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_left(llrender *llrender_struct,lptface face_in)
{
  long i=0;
  tface face;
  tvertex vertex[4];
  float fwork;
  float zero_z,one_z,two_z;

  //  Compute how many points lie outside the screen
  i=face_in->v1->sutherland_cohen+face_in->v2->sutherland_cohen+face_in->v3->sutherland_cohen;
  if (!i) {
    llrender_draw_triangle(llrender_struct,face_in);
    return;
  }
  i=i&3;
  if (i==3) return; // The polygon is off screen
  if (i==0) {
    llrender_clipping_3d_right(llrender_struct,face_in); // The polygon is inside the screen
    return;
  }

  memcpy(&face,face_in,sizeof(tface));
  face.v1->state=0;
  face.v2->state=0;
  face.v3->state=0;

  if (i==1) { // The result of the cut is two triangles
    // We start from a point located above the screen
    if (face.v1->fpx<0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpx<0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the clipping plane
    fwork=-vertex[0].fpx/(vertex[2].fpx-vertex[0].fpx);
    vertex[3].fpx=0;
    vertex[3].fpy=(vertex[2].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[3].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[3].sutherland_cohen=0;
    if (vertex[3].fpx>llrender_struct->screen_fwidth) vertex[3].sutherland_cohen+=4;
    if (vertex[3].fpy<0.0) vertex[3].sutherland_cohen+=16;
    if (vertex[3].fpy>llrender_struct->screen_fheight) vertex[3].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=-vertex[0].fpx/(vertex[1].fpx-vertex[0].fpx);
    vertex[0].fpx=0;
    vertex[0].fpy=(vertex[1].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[0].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[0].sutherland_cohen=0;
    if (vertex[0].fpx>llrender_struct->screen_fwidth) vertex[0].sutherland_cohen+=4;
    if (vertex[0].fpy<0.0) vertex[0].sutherland_cohen+=16;
    if (vertex[0].fpy>llrender_struct->screen_fheight) vertex[0].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_right(llrender_struct,&face);
    face.v1=&vertex[0];
    face.v2=&vertex[2];
    face.v3=&vertex[3];
    face.v3->state=0;
    llrender_clipping_3d_right(llrender_struct,&face);
  }
  else { // The result of the cut is a triangle
    // We start from a point located below the top margin of the screen
    if (face.v1->fpx>=0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpx>=0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the top margin of the screen
    fwork=-vertex[0].fpx/(vertex[1].fpx-vertex[0].fpx);
    vertex[1].fpx=0;
    vertex[1].fpy=(vertex[1].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[1].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[1].sutherland_cohen=0;
    if (vertex[1].fpx>llrender_struct->screen_fwidth) vertex[1].sutherland_cohen+=4;
    if (vertex[1].fpy<0.0) vertex[1].sutherland_cohen+=16;
    if (vertex[1].fpy>llrender_struct->screen_fheight) vertex[1].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=-vertex[0].fpx/(vertex[2].fpx-vertex[0].fpx);
    vertex[2].fpx=0;
    vertex[2].fpy=(vertex[2].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[2].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[2].sutherland_cohen=0;
    if (vertex[2].fpx>llrender_struct->screen_fwidth) vertex[2].sutherland_cohen+=4;
    if (vertex[2].fpy<0.0) vertex[2].sutherland_cohen+=16;
    if (vertex[2].fpy>llrender_struct->screen_fheight) vertex[2].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_right(llrender_struct,&face);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_right(llrender *llrender_struct,lptface face_in)
{
  long i=0;
  tface face;
  tvertex vertex[4];
  float fwork;
  float zero_z,one_z,two_z;

  //  Compute how many points lie outside the screen
  i=face_in->v1->sutherland_cohen+face_in->v2->sutherland_cohen+face_in->v3->sutherland_cohen;
  if (!i) {
    llrender_draw_triangle(llrender_struct,face_in);
    return;
  }
  i=(i>>2)&3;
  if (i==3) return; // The polygon is off screen
  if (i==0) {
    llrender_clipping_3d_up(llrender_struct,face_in); // The polygon is inside the screen
    return;
  }

  memcpy(&face,face_in,sizeof(tface));
  face.v1->state=0;
  face.v2->state=0;
  face.v3->state=0;

  if (i==1) { // The result of the cut is two triangles
    // We start from a point located above the screen
    if (face.v1->fpx>llrender_struct->screen_fwidth) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpx>llrender_struct->screen_fwidth) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the clipping plane
    fwork=(llrender_struct->screen_fwidth-vertex[0].fpx)/(vertex[2].fpx-vertex[0].fpx);
    vertex[3].fpx=llrender_struct->screen_fwidth;
    vertex[3].fpy=(vertex[2].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[3].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[3].sutherland_cohen=0;
    if (vertex[3].fpy<0.0) vertex[3].sutherland_cohen+=16;
    if (vertex[3].fpy>llrender_struct->screen_fheight) vertex[3].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=(llrender_struct->screen_fwidth-vertex[0].fpx)/(vertex[1].fpx-vertex[0].fpx);
    vertex[0].fpx=llrender_struct->screen_fwidth;
    vertex[0].fpy=(vertex[1].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[0].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[0].sutherland_cohen=0;
    if (vertex[0].fpy<0.0) vertex[0].sutherland_cohen+=16;
    if (vertex[0].fpy>llrender_struct->screen_fheight) vertex[0].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_up(llrender_struct,&face);
    face.v1=&vertex[0];
    face.v2=&vertex[2];
    face.v3=&vertex[3];
    face.v3->state=0;
    llrender_clipping_3d_up(llrender_struct,&face);
  }
  else { // The result of the cut is a triangle
    // We start from a point located below the top margin of the screen
    if (face.v1->fpx<=llrender_struct->screen_fwidth) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpx<=llrender_struct->screen_fwidth) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the top margin of the screen
    fwork=(llrender_struct->screen_fwidth-vertex[0].fpx)/(vertex[1].fpx-vertex[0].fpx);
    vertex[1].fpx=llrender_struct->screen_fwidth;
    vertex[1].fpy=(vertex[1].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[1].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[1].sutherland_cohen=0;
    if (vertex[1].fpy<0.0) vertex[1].sutherland_cohen+=16;
    if (vertex[1].fpy>llrender_struct->screen_fheight) vertex[1].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=(llrender_struct->screen_fwidth-vertex[0].fpx)/(vertex[2].fpx-vertex[0].fpx);
    vertex[2].fpx=llrender_struct->screen_fwidth;
    vertex[2].fpy=(vertex[2].fpy-vertex[0].fpy)*fwork+vertex[0].fpy;
    vertex[2].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[2].sutherland_cohen=0;
    if (vertex[2].fpy<0.0) vertex[2].sutherland_cohen+=16;
    if (vertex[2].fpy>llrender_struct->screen_fheight) vertex[2].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_up(llrender_struct,&face);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_up(llrender *llrender_struct,lptface face_in)
{
  long i=0;
  tface face;
  tvertex vertex[4];
  float fwork;
  float zero_z,one_z,two_z;

  //  Compute how many points lie outside the screen
  i=face_in->v1->sutherland_cohen+face_in->v2->sutherland_cohen+face_in->v3->sutherland_cohen;
  if (!i) {
    llrender_draw_triangle(llrender_struct,face_in);
    return;
  }
  i=(i>>4)&3;
  if (i==3) return; // The polygon is off screen
  if (i==0) {
    llrender_clipping_3d_down(llrender_struct,face_in); // The polygon is inside the screen
    return;
  }

  memcpy(&face,face_in,sizeof(tface));
  face.v1->state=0;
  face.v2->state=0;
  face.v3->state=0;

  if (i==1) { // The result of the cut is two triangles
    // We start from a point located above the screen
    if (face.v1->fpy<0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpy<0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the clipping plane
    fwork=-vertex[0].fpy/(vertex[2].fpy-vertex[0].fpy);
    vertex[3].fpx=(vertex[2].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[3].fpy=0;
    vertex[3].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[3].sutherland_cohen=0;
    if (vertex[3].fpy>llrender_struct->screen_fheight) vertex[3].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=-vertex[0].fpy/(vertex[1].fpy-vertex[0].fpy);
    vertex[0].fpx=(vertex[1].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[0].fpy=0;
    vertex[0].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[0].sutherland_cohen=0;
    if (vertex[0].fpy>llrender_struct->screen_fheight) vertex[0].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_down(llrender_struct,&face);
    face.v1=&vertex[0];
    face.v2=&vertex[2];
    face.v3=&vertex[3];
    face.v3->state=0;
    llrender_clipping_3d_down(llrender_struct,&face);
  }
  else { // The result of the cut is a triangle
    // We start from a point located below the top margin of the screen
    if (face.v1->fpy>=0) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpy>=0) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the top margin of the screen
    fwork=-vertex[0].fpy/(vertex[1].fpy-vertex[0].fpy);
    vertex[1].fpx=(vertex[1].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[1].fpy=0;
    vertex[1].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].fpz=1/((one_z-zero_z)*fwork+zero_z);
    vertex[1].sutherland_cohen=0;
    if (vertex[1].fpy>llrender_struct->screen_fheight) vertex[1].sutherland_cohen+=64;

    // Cut 2 with the clipping plane
    fwork=-vertex[0].fpy/(vertex[2].fpy-vertex[0].fpy);
    vertex[2].fpx=(vertex[2].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[2].fpy=0;
    vertex[2].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].fpz=1/((two_z-zero_z)*fwork+zero_z);
    vertex[2].sutherland_cohen=0;
    if (vertex[2].fpy>llrender_struct->screen_fheight) vertex[2].sutherland_cohen+=64;

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_clipping_3d_down(llrender_struct,&face);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_down(llrender *llrender_struct,lptface face_in)
{
  long i=0;
  tface face;
  tvertex vertex[4];
  float fwork;
  float zero_z,one_z,two_z;

  //  Compute how many points lie outside the screen
  i=face_in->v1->sutherland_cohen+face_in->v2->sutherland_cohen+face_in->v3->sutherland_cohen;
  if (!i) {
    llrender_draw_triangle(llrender_struct,face_in);
    return;
  }
  i=(i>>6)&3;
  if (i==3) return; // The polygon is off screen
  if (i==0) {
    llrender_draw_triangle(llrender_struct,face_in); // The polygon is inside the screen
    return;
  }

  memcpy(&face,face_in,sizeof(tface));
  face.v1->state=0;
  face.v2->state=0;
  face.v3->state=0;

  if (i==1) { // The result of the cut is two triangles
    // We start from a point located above the screen

    if (face.v1->fpy>llrender_struct->screen_fheight) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpy>llrender_struct->screen_fheight) {
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the clipping plane
    fwork=(llrender_struct->screen_fheight-vertex[0].fpy)/(vertex[2].fpy-vertex[0].fpy);
    vertex[3].fpx=(vertex[2].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[3].fpy=llrender_struct->screen_fheight;
    vertex[3].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[3].fpz=1/((two_z-zero_z)*fwork+zero_z);

    // Cut 2 with the clipping plane
    fwork=(llrender_struct->screen_fheight-vertex[0].fpy)/(vertex[1].fpy-vertex[0].fpy);
    vertex[0].fpx=(vertex[1].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[0].fpy=llrender_struct->screen_fheight;
    vertex[0].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[0].fpz=1/((one_z-zero_z)*fwork+zero_z);

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_draw_triangle(llrender_struct,&face);
    face.v1=&vertex[0];
    face.v2=&vertex[2];
    face.v3=&vertex[3];
    face.v3->state=0;
    llrender_draw_triangle(llrender_struct,&face);
  }
  else { // The result of the cut is a triangle
    // We start from a point located below the top margin of the screen
    if (face.v1->fpy<=llrender_struct->screen_fheight) {
      memcpy(&vertex[0],face.v1,sizeof(tvertex));
      memcpy(&vertex[1],face.v2,sizeof(tvertex));
      memcpy(&vertex[2],face.v3,sizeof(tvertex));
    }
    else if (face.v2->fpy<=llrender_struct->screen_fheight){
      memcpy(&vertex[0],face.v2,sizeof(tvertex));
      memcpy(&vertex[1],face.v3,sizeof(tvertex));
      memcpy(&vertex[2],face.v1,sizeof(tvertex));
    }
    else {
      memcpy(&vertex[0],face.v3,sizeof(tvertex));
      memcpy(&vertex[1],face.v1,sizeof(tvertex));
      memcpy(&vertex[2],face.v2,sizeof(tvertex));
    }
    zero_z=1.0/vertex[0].fpz;
    one_z=1.0/vertex[1].fpz;
    two_z=1.0/vertex[2].fpz;
    // Cut 1 with the top margin of the screen
    fwork=(llrender_struct->screen_fheight-vertex[0].fpy)/(vertex[1].fpy-vertex[0].fpy);
    vertex[1].fpx=(vertex[1].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[1].fpy=llrender_struct->screen_fheight;
    vertex[1].u=((vertex[1].u*one_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].v=((vertex[1].v*one_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].r=((vertex[1].r*one_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].g=((vertex[1].g*one_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].b=((vertex[1].b*one_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].intensity=((vertex[1].intensity*one_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((one_z-zero_z)*fwork+zero_z);
    vertex[1].fpz=1/((one_z-zero_z)*fwork+zero_z);

    // Cut 2 with the clipping plane
    fwork=(llrender_struct->screen_fheight-vertex[0].fpy)/(vertex[2].fpy-vertex[0].fpy);
    vertex[2].fpx=(vertex[2].fpx-vertex[0].fpx)*fwork+vertex[0].fpx;
    vertex[2].fpy=llrender_struct->screen_fheight;
    vertex[2].u=((vertex[2].u*two_z-vertex[0].u*zero_z)*fwork+vertex[0].u*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].v=((vertex[2].v*two_z-vertex[0].v*zero_z)*fwork+vertex[0].v*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].r=((vertex[2].r*two_z-vertex[0].r*zero_z)*fwork+vertex[0].r*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].g=((vertex[2].g*two_z-vertex[0].g*zero_z)*fwork+vertex[0].g*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].b=((vertex[2].b*two_z-vertex[0].b*zero_z)*fwork+vertex[0].b*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].intensity=((vertex[2].intensity*two_z-vertex[0].intensity*zero_z)*fwork+vertex[0].intensity*zero_z)/((two_z-zero_z)*fwork+zero_z);
    vertex[2].fpz=1/((two_z-zero_z)*fwork+zero_z);

    // Draw
    face.v1=&vertex[0];
    face.v2=&vertex[1];
    face.v3=&vertex[2];
    llrender_draw_triangle(llrender_struct,&face);
  }
}

//─────────────────────────────────────────────────────────────────────────────
//  Maps a texture on a triangle with gouraud
//─────────────────────────────────────────────────────────────────────────────

int dp=0;
float fids[255];



void start_division(float *fp) {
	
	fids[dp]=*fp;
	dp++;
	
}

void end_division()
{
    inverse = (float)fids[dp];
    dp--;
}
void assignment_u_v(void) {
	 u_0 = (uint32_t) (inverse * fu);
    v_0 = (uint32_t) (inverse * fv);
}

#ifndef UINT16_C
#define UINT16_C(value) (uint16_t)(value)
//__CONCAT(value,U)
#endif
static inline void combined_kernel(void)
{
    uint16_t output_pixel = 0;
    uint16_t mask = (mr >> 3) & UINT16_C(0x7E0);
    uint16_t index = (color_b & UINT16_C(0xF800)) + mask + alpha;
    output_pixel = rgba_table[index];
    output_pixel <<= 5;
    mask = (mg >> 3) & UINT16_C(0x7E0);
    index = ((color_b & UINT16_C(0x7C0)) << 5) + mask + alpha;
    output_pixel += rgba_table[index];
    output_pixel <<= 6;
    mask = (mb >> 3) & UINT16_C(0x7E0);
    index = ((color_b & UINT16_C(0x1F)) << 11) + mask + alpha;
    output_pixel += rgba_table[index];
    *destination = output_pixel;
//    printf("output pixel: %d\n",output_pixel);
    
}

void kernel1(void) {
	combined_kernel();
}

void kernel2(void) {
}

void llrender_Draw_Triangle(llrender *llrender_struct,lptface face, int mmlevel, int type)
{
  int tri_height;
  float fheight,temp,length;
  int x1,u,v,dudx,dvdx;
  short *screen_ptr;
///////////////////////////////////////////////////////
  lptmaterial mat=face->material;
  int width=mat->texture->Width[mmlevel];
  int height =mat->texture->Height[mmlevel];
  short *dir_texture=(short *)mat->texture->Levels[mmlevel];
///////////////////////////////////////////////////////
  float widtht=(float)width;
  float heightt=(float)width;
  long file_mask=(width-1)*65536;
  long rotation_mask;
  lptvertex v1=face->v1;
  lptvertex v2=face->v2;
  lptvertex v3=face->v3;
  lptvertex vertex;
  float unoz_aux,dunoz_aux,fdudx,fdvdx;
  float dxdy,prestep;
  int preincrement;
  long v1_r,v1_g,v1_b;
  long v2_r,v2_g,v2_b;
  long v3_r,v3_g,v3_b;
  long v1_alpha,v2_alpha,v3_alpha;
  long col_r,col_g,col_b,color;
  float ru1,ru2,ru3,rv1,rv2,rv3;

  texture_address=(long)mat->texture->Levels[mmlevel];

  if (!(type & T_TEXTURE)) {
    col_r=schop(face->material->dr*65536.0)&0xf800;
    col_g=schop(face->material->dg*2048.0)&0x07e0;
    col_b=schop(face->material->db*32.0)&0x1f;
    color=col_r+col_g+col_b;
  }
//rotation_mask=10;

//printf("width: %d\n",width);

  switch (width) {
    case 8:   /*if (type & T_MASK) kernel_textures=mask_kernel8_8;
              else kernel_textures=kernel8_8;
              * */
              rotation_mask=13;
              break;
    case 16:  /*if (type & T_MASK) kernel_textures=mask_kernel8_16;
              else kernel_textures=kernel8_16;
              */
              rotation_mask=12;
              break;
    case 32:  /*if (type & T_MASK) kernel_textures=mask_kernel8_32;
              else kernel_textures=kernel8_32;
              */
              rotation_mask=11;
              break;
    case 64:  /*if (type & T_MASK) kernel_textures=mask_kernel8_64;
              else kernel_textures=kernel8_64;
              */
              rotation_mask=10;
              break;
    case 128: /*if (type & T_MASK) kernel_textures=mask_kernel8_128;
              else kernel_textures=kernel8_128;
              */rotation_mask=9;
              break;
    case 256: /*if (type & T_MASK) kernel_textures=mask_kernel8_256;
              else kernel_textures=kernel8_256;
              */
              rotation_mask=8;
              break;
  }

  //Sort the vertices, v1 the topmost and v3 the bottommost

  if (v1->fpy > v2->fpy) { vertex=v1; v1=v2; v2=vertex; }
  if (v1->fpy > v3->fpy) { vertex=v1; v1=v3; v3=vertex; }
  if (v2->fpy > v3->fpy) { vertex=v2; v2=v3; v3=vertex; }

  if (type & T_TEXTURE) {
    if (type & T_PERSPECTIVE) {
      ru1=65536.0*v1->u*widtht/v1->fpz; ru2=65536.0*v2->u*widtht/v2->fpz; ru3=65536.0*v3->u*widtht/v3->fpz;
      rv1=65536.0*v1->v*heightt/v1->fpz; rv2=65536.0*v2->v*heightt/v2->fpz; rv3=65536.0*v3->v*heightt/v3->fpz;
    }
    else {
      rv1=65536.0*v1->v*heightt; rv2=65536.0*v2->v*heightt; rv3=65536.0*v3->v*heightt;
      ru1=65536.0*v1->u*widtht; ru2=65536.0*v2->u*widtht; ru3=65536.0*v3->u*widtht;
    }
  }
 
#ifndef NOTYET

  if (type & T_GOURAUD) {
    v1_alpha=v1->intensity*31;
    v2_alpha=v2->intensity*31;
    v3_alpha=v3->intensity*31;

    v1_r=v1->r*63;
    v1_g=v1->g*63;
    v1_b=v1->b*63;

    v2_r=v2->r*63;
    v2_g=v2->g*63;
    v2_b=v2->b*63;

    v3_r=v3->r*63;
    v3_g=v3->g*63;
    v3_b=v3->b*63;
  }

  //Compute the length of the longest line
  tri_height=v3->py-v1->py;
  if ((v3->fpy-v1->fpy)<0) return;
//  if (tri_height<=0) return;

  fheight=1.0/(v3->fpy-v1->fpy);
  temp=(v2->fpy-v1->fpy)*fheight;
  length=temp*(v3->fpx-v1->fpx)+(v1->fpx-v2->fpx);
  if (length==0) return;

  if (length<0) {
    right_section=2;
    left_section=1;
    left_section_height=tri_height;
    dxdy=(v3->fpx-v1->fpx)*fheight;
    dx_left=schop(dxdy*65536.0);
    prestep=(float)v1->py-v1->fpy;
    x_left=schop((v1->fpx+prestep*dxdy)*65536.0);
    //Computation of the deltas
    if (type & T_TEXTURE) {
      if (type & T_PERSPECTIVE) {
        duz_left=(ru3-ru1)*fheight;
        uz_left=ru1+duz_left*prestep;
        dvz_left=(rv3-rv1)*fheight;
        vz_left=rv1+dvz_left*prestep;
        dunoz_left=(1/v3->fpz-1/v1->fpz)*fheight;
        unoz_left=1/v1->fpz+dunoz_left*prestep;
      }
      else {
        du_left=schop((ru3-ru1)*fheight);
        u_left=schop(ru1+(float)du_left*prestep);
        dv_left=schop((rv3-rv1)*fheight);
        v_left=schop(rv1+(float)dv_left*prestep);
      }
    }
    if (type & T_GOURAUD) {
      dr_left=schop(((v3_r-v1_r)*256.0)*fheight);
      r_left=schop(v1_r*256.0+(float)dr_left*prestep);
      dg_left=schop(((v3_g-v1_g)*256.0)*fheight);
      g_left=schop(v1_g*256.0+(float)dg_left*prestep);
      db_left=schop(((v3_b-v1_b)*256.0)*fheight);
      b_left=schop(v1_b*256.0+(float)db_left*prestep);
      dintensity_left=schop(((v3_alpha-v1_alpha)*256.0)*fheight);
      intensity_left=schop(v1_alpha*256.0+(float)dintensity_left*prestep);
    }
    right_section_height=v2->py-v1->py;
    if (right_section_height<=0) {
      right_section_height=v3->py-v2->py;
      if (right_section_height<=0) return;
      else {
        //Computation of the deltas
        dx_right=schop(((v3->fpx-v2->fpx)*65536.0)/(v3->fpy-v2->fpy));
        fheight=1.0/(v3->fpy-v2->fpy);
        dxdy=(v3->fpx-v2->fpx)*fheight;
        prestep=(float)v2->py-v2->fpy;
        x_right=schop((v2->fpx+prestep*dxdy)*65536.0);
        if ((type & T_TEXTURE) && (type & T_PERSPECTIVE)) {
          duz_right=(ru3-ru2)*fheight;
          uz_right=ru2+duz_right*prestep;
          dvz_right=(rv3-rv2)*fheight;
          vz_right=rv2+dvz_right*prestep;
          dunoz_right=(1/v3->fpz-1/v2->fpz)*fheight;
          unoz_right=1/v2->fpz+dunoz_right*prestep;
        }
      }
    }
    else {
      //Computation of the deltas
      dx_right=schop(((v2->fpx-v1->fpx)*65536.0)/(v2->fpy-v1->fpy));
      fheight=1.0/(v2->fpy-v1->fpy);
      dxdy=(v2->fpx-v1->fpx)*fheight;
      prestep=(float)v1->py-v1->fpy;
      x_right=schop((v1->fpx+prestep*dxdy)*65536.0);
      if ((type & T_TEXTURE) && (type & T_PERSPECTIVE)) {
        duz_right=(ru2-ru1)*fheight;
        uz_right=ru1+duz_right*prestep;
        dvz_right=(rv2-rv1)*fheight;
        vz_right=rv1+dvz_right*prestep;
        dunoz_right=(1/v2->fpz-1/v1->fpz)*fheight;
        unoz_right=1/v1->fpz+dunoz_right*prestep;
      }
    }
  }
  else {
    left_section=2;
    right_section=1;
    right_section_height=tri_height;
    dx_right=schop(((v3->fpx-v1->fpx)*65536.0)/(v3->fpy-v1->fpy));
    fheight=1.0/(v3->fpy-v1->fpy);
    dxdy=(v3->fpx-v1->fpx)*fheight;
    prestep=(float)v1->py-v1->fpy;
    x_right=schop((v1->fpx+prestep*dxdy)*65536.0);
    if ((type & T_TEXTURE) && (type & T_PERSPECTIVE)) {
      duz_right=(ru3-ru1)*fheight;
      uz_right=ru1+duz_right*prestep;
      dvz_right=(rv3-rv1)*fheight;
      vz_right=rv1+dvz_right*prestep;
      dunoz_right=(1/v3->fpz-1/v1->fpz)*fheight;
      unoz_right=1/v1->fpz+dunoz_right*prestep;
    }
    left_section_height=v2->py-v1->py;
    if (left_section_height<=0) {
      left_section--;
      left_section_height=v3->py-v2->py;
      if (left_section_height<=0)
        return;
      else {
        fheight=1.0/(v3->fpy-v2->fpy);
        dxdy=(v3->fpx-v2->fpx)*fheight;
        dx_left=schop(dxdy*65536.0);
        prestep=(float)v2->py-v2->fpy;
        x_left=schop((v2->fpx+prestep*dxdy)*65536.0);
        //Computation of the deltas
        if (type & T_TEXTURE) {
          if (type & T_PERSPECTIVE) {
            duz_left=(ru3-ru2)*fheight;
            uz_left=ru2+duz_left*prestep;
            dvz_left=(rv3-rv2)*fheight;
            vz_left=rv2+dvz_left*prestep;
            dunoz_left=(1/v3->fpz-1/v2->fpz)*fheight;
            unoz_left=1/v2->fpz+dunoz_left*prestep;
          }
          else {
            du_left=schop((ru3-ru2)*fheight);
            u_left=schop(ru2+(float)du_left*prestep);
            dv_left=schop((rv3-rv2)*fheight);
            v_left=schop(rv2+(float)dv_left*prestep);
          }
        }
        if (type & T_GOURAUD) {
          dr_left=schop(((v3_r-v2_r)*256.0)*fheight);
          r_left=schop(v2_r*256.0+(float)dr_left*prestep);
          dg_left=schop(((v3_g-v2_g)*256.0)*fheight);
          g_left=schop(v2_g*256.0+(float)dg_left*prestep);
          db_left=schop(((v3_b-v2_b)*256.0)*fheight);
          b_left=schop(v2_b*256.0+(float)db_left*prestep);
          dintensity_left=schop(((v3_alpha-v2_alpha)*256.0)*fheight);
          intensity_left=schop(v2_alpha*256.0+(float)dintensity_left*prestep);
        }
      }
    }
    else {
      fheight=1.0/(v2->fpy-v1->fpy);
      dxdy=(v2->fpx-v1->fpx)*fheight;
      dx_left=schop(dxdy*65536.0);
      prestep=(float)v1->py-v1->fpy;
      x_left=schop((v1->fpx+prestep*dxdy)*65536.0);
      //Computation of the deltas
      if (type & T_TEXTURE) {
        if (type & T_PERSPECTIVE) {
          duz_left=(ru2-ru1)*fheight;
          uz_left=ru1+duz_left*prestep;
          dvz_left=(rv2-rv1)*fheight;
          vz_left=rv1+dvz_left*prestep;
          dunoz_left=(1/v2->fpz-1/v1->fpz)*fheight;
          unoz_left=1/v1->fpz+dunoz_left*prestep;
        }
        else {
          du_left=schop((ru2-ru1)*fheight);
          u_left=schop(ru1+(float)du_left*prestep);
          dv_left=schop((rv2-rv1)*fheight);
          v_left=schop(rv1+(float)dv_left*prestep);
        }
      }
      if (type & T_GOURAUD) {
        dr_left=schop(((v2_r-v1_r)*256.0)*fheight);
        r_left=schop(v1_r*256.0+(float)dr_left*prestep);
        dg_left=schop(((v2_g-v1_g)*256.0)*fheight);
        g_left=schop(v1_g*256.0+(float)dg_left*prestep);
        db_left=schop(((v2_b-v1_b)*256.0)*fheight);
        b_left=schop(v1_b*256.0+(float)db_left*prestep);
        dintensity_left=schop(((v2_alpha-v1_alpha)*256.0)*fheight);
        intensity_left=schop(v1_alpha*256.0+(float)dintensity_left*prestep);
      }
    }
  }

  if ((type & T_TEXTURE) && !(type & T_PERSPECTIVE)) {
    dudx=schop((temp*(ru3-ru1)+(ru1-ru2))/length);
    dvdx=schop((temp*(rv3-rv1)+(rv1-rv2))/length);
  }

  if (type & T_GOURAUD) {
    drdx=schop( 256.0*((temp*(v3_r-v1_r)+(v1_r-v2_r))/length) );
    dgdx=schop( 256.0*((temp*(v3_g-v1_g)+(v1_g-v2_g))/length) );
    dbdx=schop( 256.0*((temp*(v3_b-v1_b)+(v1_b-v2_b))/length) );
    dintensitydx=schop( 256.0*((temp*(v3_alpha-v1_alpha)+(v1_alpha-v2_alpha))/length) );
  }

  screen_ptr=(short *)(llrender_struct->copy_buffer+(v1->py)*llrender_struct->vwidth);

  for (;;) { //Loop that goes through the polygon from top to bottom
    x1=adjust_integer(x_left);
    width=adjust_integer(x_right)-x1;

    if (type & T_TEXTURE) {
      if (type & T_PERSPECTIVE) {
        widtht=16.0/(float)width;

        start_division(&unoz_left);
        destination=screen_ptr+x1;

        if (width>16) {
          preincrement=(x1<<16)-x_left;
//          fu=uz_left+(float)preincrement*duz_left/65536.0;
//          fv=vz_left+(float)preincrement*dvz_left/65536.0;
          fu=uz_left;
          fv=vz_left;
          end_division();
          fdudx=(uz_right-uz_left)*widtht;
          fdvdx=(vz_right-vz_left)*widtht;
          dunoz_aux=(unoz_right-unoz_left)*widtht;
          unoz_aux=unoz_left+dunoz_aux;
          assignment_u_v();
          start_division(&unoz_aux);
          u=u_0;
          v=v_0;
          fu+=fdudx;
          fv+=fdvdx;
          end_division();
          assignment_u_v();
          do {
          	dudx=(u_0-u)>>4;
          	dvdx=(v_0-v)>>4;
          	fu+=fdudx;
          	fv+=fdvdx;
            unoz_aux+=dunoz_aux;
            start_division(&unoz_aux);
            kernel_textures(v,u,dudx,dvdx,16,destination);
            end_division();
            u=u_0;
            v=v_0;
            assignment_u_v();
            destination+=16;
          	width-=16;
          } while (width>16);
          if (width>0) {
            dudx=(u_0-u)>>4;
            dvdx=(v_0-v)>>4;
            kernel_textures(v,u,dudx,dvdx,width,destination);
          }
        }
        else if (width>0) {
//          fu=uz_left+(float)preincrement*duz_left/65536.0;
//          fv=vz_left+(float)preincrement*dvz_left/65536.0;
          fu=uz_left;
          fv=vz_left;
          end_division();
          assignment_u_v();
          start_division(&unoz_right);
          u=u_0;
          v=v_0;
          fu=uz_right;
          fv=vz_right;
          end_division();
          assignment_u_v();
          if (width>1) {
            dudx=(u_0-u)/width;
            dvdx=(v_0-v)/width;
          }
          else {
            dudx=u_0-u;
            dvdx=v_0-v;
          }
          kernel_textures(v,u,dudx,dvdx,width,destination);
        }
        else end_division();
      }
      else if (type & T_GOURAUD) {
        if (type & T_MASK) {
          if (width>0) {
            destination=screen_ptr+x1;
            preincrement=(x1<<16)-x_left;
            u=u_left+((preincrement*dudx)>>16);
            v=v_left+((preincrement*dvdx)>>16);
            mr=r_left; mg=g_left; mb=b_left;
            intensity=intensity_left;
            do {
              alpha=intensity>>8;
    	        if ((color_b=dir_texture[((v&file_mask)>>rotation_mask)+((u&file_mask)>>16)])!=0) {

                kernel1(); kernel2();
              }
    	        mr+=drdx; mg+=dgdx; mb+=dbdx;
              intensity+=dintensitydx;
            	destination++;
            	u+=dudx;
            	v+=dvdx;
            } while (--width);
          }
        }
        else {
          if (width>0) {
            destination=screen_ptr+x1;
            preincrement=(x1<<16)-x_left;
            u=u_left+((preincrement*dudx)>>16);
            v=v_left+((preincrement*dvdx)>>16);
            mr=r_left; mg=g_left; mb=b_left;
            intensity=intensity_left;
            do {
              alpha=intensity>>8;
    	        color_b=dir_texture[((v&file_mask)>>rotation_mask)+((u&file_mask)>>16)];
              kernel1(); kernel2();
    	        mr+=drdx; mg+=dgdx; mb+=dbdx;
              intensity+=dintensitydx;
            	destination++;
            	u+=dudx;
            	v+=dvdx;
            } while (--width);
          }
        }
      }
/*
      else if (type & T_TRANSLUCENT) {
        if (type & T_MASK) {
          if (width>0) {
            destination=screen_ptr+x1;
            preincrement=(x1<<16)-x_left;
            u=u_left+((preincrement*dudx)>>16);
            v=v_left+((preincrement*dvdx)>>16);
            do {
      	      if (color=dir_texture[((v&file_mask)>>rotation_mask)+((u&file_mask)>>16)]) {
                _asm {
                  pushad
                  mov edi,destino
                  mov ebx,color
                  mov edx,ebx
                  and ebx,0f81fh
                  and edx,07e0h

                  mov ax,[edi]
                  and eax,0f81fh
                  add eax,ebx
                  shr eax,1
                  and eax,0f81fh
                  mov bp,[edi]
                  and ebp,07e0h
                  add ebp,edx
                  shr ebp,1
                  and ebp,07e0h
                  add eax,ebp
                  mov [edi],ax
                  popad
                }
              }
          	  destination++;
            	u+=dudx;
            	v+=dvdx;
            } while (--ancho);
          }
        }
        else {
          if (width>0) {
            destination=screen_ptr+x1;
            preincrement=(x1<<16)-x_left;
            u=u_left+((preincrement*dudx)>>16);
            v=v_left+((preincrement*dvdx)>>16);
            do {
    	        color=dir_texture[((v&file_mask)>>rotation_mask)+((u&file_mask)>>16)];
              _asm {
                pushad
                mov edi,destino
                mov ebx,color
                mov edx,ebx
                and ebx,0f81fh
                and edx,07e0h

                mov ax,[edi]
                and eax,0f81fh
                add eax,ebx
                shr eax,1
                and eax,0f81fh
                mov bp,[edi]
                and ebp,07e0h
                add ebp,edx
                shr ebp,1
                and ebp,07e0h
                add eax,ebp
                mov [edi],ax
                popad
              }
            	destination++;
          	  u+=dudx;
            	v+=dvdx;
            } while (--ancho);
          }
        }
      }
*/
      else {
        if (width>0) {
          destination=screen_ptr+x1;
          kernel_textures(v_left,u_left,dudx,dvdx,width,destination);
        }
      }
    }
/*
    else {
      if (type & T_TRANSLUCENT) {
        if (width>0) {
          destination=screen_ptr+x1;
          _asm {
            pushad
            mov edi,destino
            mov ebx,color
            mov ecx,ancho
            mov edx,ebx
            and ebx,0f81fh
            and edx,07e0h
          bucle1:
            mov ax,[edi]
            and eax,0f81fh
            add eax,ebx
            shr eax,1
            and eax,0f81fh
            mov bp,[edi]
            and ebp,07e0h
            add ebp,edx
            shr ebp,1
            and ebp,07e0h
            add eax,ebp
            mov [edi],ax
            add edi,2
            dec ecx
            jnz bucle1
            popad
          }
        }
      }
      else if (type & T_GOURAUD) {
        if (width>0) {
          destination=screen_ptr+x1;
          mr=r_left; mg=g_left; mb=b_left;
          do {
            _asm {
              pushad
              mov edi,destino
              mov ebx,mr
              cmp ebx,10000h
              jbe label1
              mov ebx,0f800h
            label1:
              and ebx,0f800h
              mov edx,mg
              cmp edx,10000h
              jbe label2
              mov edx,0fc00h
            label2:
              and edx,0fc00h
              mov esi,mb
              cmp esi,10000h
              jbe label3
              mov esi,0f800h
            label3:
              and esi,0f800h
              mov eax,ebx
              shr edx,5
              add eax,edx
              shr esi,11
              add eax,esi
              mov [edi],ax
              popad
            }
            destination++;
	          mr+=drdx; mg+=dgdx; mb+=dbdx;
          } while (--ancho);
        }
      }
      else {
        if (width>0) {
          destination=screen_ptr+x1;
          lf_set(destination,color,width);
        }
      }
    }
*/
    screen_ptr+=llrender_struct->screen_width;

    if (--left_section_height<=0) {
      if (--left_section<=0) return;
      left_section_height=v3->py-v2->py;
      if (left_section_height<=0)
        return;
      else {
        fheight=1.0/(v3->fpy-v2->fpy);
        dxdy=(v3->fpx-v2->fpx)*fheight;
        dx_left=schop(dxdy*65536.0);
        prestep=(float)v2->py-v2->fpy;
        x_left=schop((v2->fpx+prestep*dxdy)*65536.0);
        //Computation of the deltas
        if (type & T_TEXTURE) {
          if (type & T_PERSPECTIVE) {
            duz_left=(ru3-ru2)*fheight;
            uz_left=ru2+duz_left*prestep;
            dvz_left=(rv3-rv2)*fheight;
            vz_left=rv2+dvz_left*prestep;
            dunoz_left=(1/v3->fpz-1/v2->fpz)*fheight;
            unoz_left=1/v2->fpz+dunoz_left*prestep;
          }
          else {
            du_left=schop((ru3-ru2)*fheight);
            u_left=schop(ru2+(float)du_left*prestep);
            dv_left=schop((rv3-rv2)*fheight);
            v_left=schop(rv2+(float)dv_left*prestep);
          }
        }
        if (type & T_GOURAUD) {
          dr_left=schop(((v3_r-v2_r)*256.0)*fheight);
          r_left=schop(v2_r*256.0+(float)dr_left*prestep);
          dg_left=schop(((v3_g-v2_g)*256.0)*fheight);
          g_left=schop(v2_g*256.0+(float)dg_left*prestep);
          db_left=schop(((v3_b-v2_b)*256.0)*fheight);
          b_left=schop(v2_b*256.0+(float)db_left*prestep);
          dintensity_left=schop(((v3_alpha-v2_alpha)*256.0)*fheight);
          intensity_left=schop(v2_alpha*256.0+(float)dintensity_left*prestep);
        }
      }
    }
    else {
      x_left+=dx_left;
      if (type & T_TEXTURE) {
        if (type & T_PERSPECTIVE) {
          uz_left+=duz_left;
          vz_left+=dvz_left;
          unoz_left+=dunoz_left;
        }
        else {
          u_left+=du_left;
          v_left+=dv_left;
        }
      }
      if (type & T_GOURAUD) {
        r_left+=dr_left;
        g_left+=dg_left;
        b_left+=db_left;
        intensity_left+=dintensity_left;
      }
    }

    if (--right_section_height<=0) {
      if (--right_section<=0) return;
      right_section_height=v3->py-v2->py;
      if (right_section_height<=0) return;
      else {
        //Computation of the deltas
        dx_right=schop(((v3->fpx-v2->fpx)*65536.0)/(v3->fpy-v2->fpy));
        fheight=1.0/(v3->fpy-v2->fpy);
        dxdy=(v3->fpx-v2->fpx)*fheight;
        prestep=(float)v2->py-v2->fpy;
        x_right=schop((v2->fpx+prestep*dxdy)*65536.0);
        if ((type & T_TEXTURE) && (type & T_PERSPECTIVE)) {
          duz_right=(ru3-ru2)*fheight;
          uz_right=ru2+duz_right*prestep;
          dvz_right=(rv3-rv2)*fheight;
          vz_right=rv2+dvz_right*prestep;
          dunoz_right=(1/v3->fpz-1/v2->fpz)*fheight;
          unoz_right=1/v2->fpz+dunoz_right*prestep;
        }
      }
    }
    else {
      x_right+=dx_right;
      if ((type & T_TEXTURE) && (type & T_PERSPECTIVE)) {
        uz_right+=duz_right;
        vz_right+=dvz_right;
        unoz_right+=dunoz_right;
      }
    }
  }
#endif
}

