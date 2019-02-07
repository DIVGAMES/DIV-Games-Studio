//#include <mem.h>
#include <math.h>
//#include <graph.h>
#include "global.hpp"
#include "llrender.hpp"
#include "stdint.h"
#include "t.h"

long complex_x_ini, complex_x_fin;
long complex_y_ini, complex_y_fin;

short *tabla_rgba;
long num_llrenders = 0;

float LMax = 1.8; // 1.8
float LMin = 0.7; // 0.7

// extern "C" {
// extern
int direccion_textura;
extern int clock1, clock2;
//}

#define PI ((float)3.141592)

int u_1, v_1, u_0, v_0;
float fu, fv, inversa;

//-----------------------------------------------------------------------------
// Datos globales
//-----------------------------------------------------------------------------

int seccion_izquierda, seccion_derecha;
int altura_seccion_izquierda, altura_seccion_derecha;
int drdx, dgdx, dbdx, dintensidaddx;
int u_izq, du_izq, v_izq, dv_izq;
int x_izq, dx_izq, x_der, dx_der;
int r_izq, dr_izq, g_izq, dg_izq, b_izq, db_izq;
int intensidad_izq, dintensidad_izq;
float duz_izq, dvz_izq, unoz_izq, dunoz_izq;
float duz_der, dvz_der, unoz_der, dunoz_der;
float uz_izq, vz_izq, uz_der, vz_der;

//-----------------------------------------------------------------------------

long ajuste_flotante(float x) // inline
{
	return ((schop(x * 65536.0) + 0xffff) >> 16);
}

long ajuste_entero(long x) // inline
{
	x += 0xffff;
	return (x >> 16);
}

void (*nucleo_texturas)(int v, int u, int du, int dv, int ancho,
			short *destino);

int clocks();
#ifdef DOS
#pragma aux clocks = "push edx"                                                \
		     "db 0fh,31h"                                              \
		     "pop edx" value[eax]

#endif

void inicio_division(float *);
#ifdef DOS

#pragma aux inicio_division = "fld1"                                           \
			      "fdiv dword ptr[EAX]" parm caller[EAX]

#endif

void fin_division();

#ifdef DOS

#pragma aux fin_division = "fstp dword ptr[inversa]"

#endif

void asignacion_u_v();

#ifdef DOS

#pragma aux asignacion_u_v = "fld dword ptr[inversa]"                          \
			     "fld st(0)"                                       \
			     "fmul dword ptr[fu]"                              \
			     "fistp dword ptr[u_0]"                            \
			     "fmul dword ptr[fv]"                              \
			     "fistp dword ptr[v_0]"

#endif
void lf_set(void *, long, long);

#ifdef DOS

#pragma aux lf_set =                                                           \
    "rep stosw" parm caller[edi][eax][ecx] modify exact[edi eax ecx]

#endif
//=============================================================================

short *destino;
short color_b;
int mr, mg, mb, intensidad, alfa;

void nucleo1();

#ifdef DOS

#pragma aux nucleo1 = "pushad"                                                 \
		      "mov edi,destino"                                        \
		      "mov esi,tabla_rgba"                                     \
		      "xor edx,edx"                                            \
		      "mov ebx,mr"                                             \
		      "shr ebx,3"                                              \
		      "and ebx,7e0h"                                           \
		      "mov ax,color_b"                                         \
		      "and eax,0f800h"                                         \
		      "add eax,ebx"                                            \
		      "add eax,alfa"                                           \
		      "mov dx,[esi+eax*2]"                                     \
		      "shl edx,5"                                              \
		      "mov ebx,mg"                                             \
		      "shr ebx,3"                                              \
		      "and ebx,7e0h"

#endif

void nucleo2();

#ifdef DOS
#pragma aux nucleo2 = "mov ax,color_b"                                         \
		      "and eax,07c0h"                                          \
		      "shl eax,5"                                              \
		      "add eax,ebx"                                            \
		      "add eax,alfa"                                           \
		      "add dx,[esi+eax*2]"                                     \
		      "shl edx,6"                                              \
		      "mov ebx,mb"                                             \
		      "shr ebx,3"                                              \
		      "and ebx,7e0h"                                           \
		      "mov ax,color_b"                                         \
		      "and eax,1fh"                                            \
		      "shl eax,11"                                             \
		      "add eax,ebx"                                            \
		      "add eax,alfa"                                           \
		      "add dx,[esi+eax*2]"                                     \
		      "mov [edi],dx"                                           \
		      "popad"

#endif

//=============================================================================

void llrender_create(llrender *llrender_struct, sprite3d *newcamera)
{
	short alfa, color, gouraud;
	int i, c;
	float t;

	llrender_struct->dist_max = 27000;
	//  video=vid;
	llrender_struct->mmdist[0] = llrender_struct->dist_max * 0.33;
	llrender_struct->mmdist[1] = llrender_struct->dist_max * 0.66;
	for (i = 0; i < 256; i++)
		// Datos cambiantes
		llrender_struct->ancho_pantalla =
		    newcamera->ancho;			  // video->Ancho;
	llrender_struct->alto_pantalla = newcamera->alto; // video->Alto;
	llrender_struct->fancho_pantalla = llrender_struct->ancho_pantalla;
	llrender_struct->falto_pantalla = llrender_struct->alto_pantalla;
	llrender_struct->copia = newcamera->buffer; // video->Buffer;
	llrender_struct->vancho = newcamera->ancho * 2;
	llrender_struct->fin_copia =
	    llrender_struct->copia +
	    llrender_struct->vancho * llrender_struct->alto_pantalla;

	if (num_llrenders == 0) {

		tabla_rgba = (short *)lf_malloc(65536 * 2);

		for (i = 0; i < 65536; i++) {

			alfa = (short)(i & 0x1f); // 0-32 Nivel de iluminacion
			color =
			    (short)((i >> 10) & 0x3e); // 0-64 Color de textura
			gouraud = (short)((i >> 5) &
					  0x3f); // 0-64 Color de iluminacion

			t = (LMax - LMin) * ((float)alfa / 32.0) + LMin;
			c = (int)(t * (float)color);
			tabla_rgba[i] = (short)(c > 63 ? 31 : c >> 1);

			//  tabla_rgba[i]=(((i>>10)&0x3e)*(31-alfa)+((i>>5)&0x3f)*alfa)/64;
		}
	}
	num_llrenders++;
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_destroy(llrender *llrender_struct)
{
	num_llrenders--;
	if (num_llrenders == 0) {
		lf_free(tabla_rgba);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

int compara(const void *op1, const void *op2)
{
	return ((*((const lptface *)op1))->v1->fpz <
		(*((const lptface *)op2))->v1->fpz);
}

/*
int compara(const void *op1, const void *op2)
{
  const lptface p1= *((const lptface *)op1);
  const lptface p2= *((const lptface *)op2);
  return (p1->v1->fpz<p2->v1->fpz);
}
*/

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_render(llrender *llrender_struct, lptface faces, int nfaces)
{
	int i;
	lptface *caras;

	if ((caras = (lptface *)lf_malloc(sizeof(lptface) * nfaces)) == NULL)
		return;

	for (i = 0; i < nfaces; i++)
		caras[i] = &faces[i];

	qsort(caras, nfaces, sizeof(lptface *), compara);

	i = 0;
	do {
		llrender_clipping_3d_near(llrender_struct, caras[i]);
		//    llrender_clipping_3d_near(llrender_struct,&faces[i]);
	} while (++i < nfaces);

	lf_free(caras);
}

//─────────────────────────────────────────────────────────────────────────────
//  Pinta un triangulo en el buffer de video
//─────────────────────────────────────────────────────────────────────────────

void llrender_pintar_triangulo(llrender *llrender_struct, lptface face)
{
	int mmlvl;
	int zmed;
	// int color;

	if (face->tipo_map == -1)
		return;

	if (face->v1->estado == 0) {
		face->v1->px = ajuste_flotante(face->v1->fpx);
		face->v1->py = ajuste_flotante(face->v1->fpy);
		face->v1->estado = 1;
	}

	if (face->v2->estado == 0) {
		face->v2->px = ajuste_flotante(face->v2->fpx);
		face->v2->py = ajuste_flotante(face->v2->fpy);
		face->v2->estado = 1;
	}

	if (face->v3->estado == 0) {
		face->v3->px = ajuste_flotante(face->v3->fpx);
		face->v3->py = ajuste_flotante(face->v3->fpy);
		face->v3->estado = 1;
	}

	//---------------------------------------------------------------------------
	//  Calculo la bounding box del objeto en pantalla
	//---------------------------------------------------------------------------

	if (face->v1->px < complex_x_ini)
		complex_x_ini = face->v1->px;
	if (face->v2->px < complex_x_ini)
		complex_x_ini = face->v2->px;
	if (face->v3->px < complex_x_ini)
		complex_x_ini = face->v3->px;
	if (face->v1->px > complex_x_fin)
		complex_x_fin = face->v1->px + 1;
	if (face->v2->px > complex_x_fin)
		complex_x_fin = face->v2->px + 1;
	if (face->v3->px > complex_x_fin)
		complex_x_fin = face->v3->px + 1;
	if (face->v1->py < complex_y_ini)
		complex_y_ini = face->v1->py;
	if (face->v2->py < complex_y_ini)
		complex_y_ini = face->v2->py;
	if (face->v3->py < complex_y_ini)
		complex_y_ini = face->v3->py;
	if (face->v1->py > complex_y_fin)
		complex_y_fin = face->v1->py + 1;
	if (face->v2->py > complex_y_fin)
		complex_y_fin = face->v2->py + 1;
	if (face->v3->py > complex_y_fin)
		complex_y_fin = face->v3->py + 1;

	// Si el poligono no apunta a la cámara no se ve y por tanto no se pinta
	if (llrender_Cara_Oculta(llrender_struct, face))
		return;
	/*
  #ifdef DEBUG
    line(face->v1->px, face->v1->py, face->v2->px, face->v2->py, 0xFFFF);
    line(face->v2->px, face->v2->py, face->v3->px, face->v3->py, 0xFFFF);
    line(face->v3->px, face->v3->py, face->v1->px, face->v1->py, 0xFFFF);
  #else

  if (face->tipo_map==WIRE) {
    color=(schop(face->material->dr*65536.0)&0xf800)+
	  (schop(face->material->dg*2048.0)&0x07e0)+
	  (schop(face->material->db*32.0)&0x1f);
    line(face->v1->px, face->v1->py, face->v2->px, face->v2->py, color);
    line(face->v2->px, face->v2->py, face->v3->px, face->v3->py, color);
    line(face->v3->px, face->v3->py, face->v1->px, face->v1->py, color);
    return;
  }
*/
	zmed = schop(face->v1->fpz + face->v2->fpz + face->v3->fpz) >> 2;

	//  Calculo el Mipmap asociado a la textura
	if (zmed < llrender_struct->mmdist[0])
		mmlvl = 0;
	else if (zmed < llrender_struct->mmdist[1])
		mmlvl = 1;
	else if (zmed < llrender_struct->dist_max)
		mmlvl = 2;
	else
		mmlvl = 3;

	switch (face->tipo_map) {
	case FLAT:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl, T_FLAT);
		break;
	case ANTIALIAS:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl, T_FLAT);
		break;
	case GOURAUD:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_GOURAUD);
		break;
	case TEXTURA:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA);
		break;
	case TEXT_GOUR:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_GOURAUD);
		break;
	case TEXT_ILUM:
		//      Pinta_Text_Ilum(face,mmlvl);
		//      llrender_Pinta_Triangulo(llrender_struct,face,mmlvl,T_TEXTURA
		//      | T_TRANSLUCIDO);
		break;
	case TEXT_PER:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_PERSPECTIVA);
		break;
	case TEXT_PER_TRANS:
		//      Pinta_Text_Persp_Translucido(face,mmlvl);
		break;
	case TEXT_TRANS:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_TRANSLUCIDO);
		break;
	case SOMBRA:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl, T_FLAT);
		break;
	case TRANS_PER:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_PERSPECTIVA | T_MASCARA);
		break;
	case TEXT_BILINEAR:
		//      Pinta_Text_Persp_Bilinear(face,mmlvl);
		break;
	case TRANSPARENTE:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_MASCARA);
		break;
	case TRANSPARENTE_MASCARA:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_TRANSLUCIDO | T_MASCARA);
		break;
	case FLAT_TRANS:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_FLAT | T_TRANSLUCIDO);
		break;
	case GOUR_MASK:
		llrender_Pinta_Triangulo(llrender_struct, face, mmlvl,
					 T_TEXTURA | T_MASCARA | T_GOURAUD);
		break;
	}

	//  #endif
}

//─────────────────────────────────────────────────────────────────────────────
//  Comprueba si una cara está oculta
//─────────────────────────────────────────────────────────────────────────────

int llrender_Cara_Oculta(llrender *llrender_struct, lptface face)
{
	float producto_escalar;

	producto_escalar =
	    (face->v2->fpx - face->v1->fpx) * (face->v3->fpy - face->v1->fpy) -
	    (face->v2->fpy - face->v1->fpy) * (face->v3->fpx - face->v1->fpx);

	if (producto_escalar < 0)
		return (0);

	return 1;
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_near(llrender *llrender_struct, lptface face_in)
{
	long i = 0;
	tface face;
	tvertex vertex[4];
	float fwork;

	//  Calculamos cuantos puntos están situados detras del plano de
	//  clipping
	i = face_in->v1->sutherland_cohen + face_in->v2->sutherland_cohen +
	    face_in->v3->sutherland_cohen;
	if (!i) {
		llrender_pintar_triangulo(llrender_struct, face_in);
		return;
	}
	i = (i >> 8) & 3;
	if (i == 3)
		return; // El polígono está fuera de pantalla
	if (i == 0) {
		llrender_clipping_3d_left(
		    llrender_struct,
		    face_in); // El polígono está dentro de pantalla
		return;
	}

	memcpy(&face, face_in, sizeof(tface));
	face.v1->estado = 0;
	face.v2->estado = 0;
	face.v3->estado = 0;

	if (i == 2) { // El resultado del corte es un triángulo
		// Partimos de un punto situado delante del plano de clipping
		if (face.v1->fpz >= 1.0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpz >= 1.0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		// Corte 1 con el plano de clipping
		fwork = (1.0 - vertex[0].fpz) / (vertex[1].fpz - vertex[0].fpz);
		vertex[1].sx =
		    (vertex[1].sx - vertex[0].sx) * fwork + vertex[0].sx;
		vertex[1].sy =
		    (vertex[1].sy - vertex[0].sy) * fwork + vertex[0].sy;
		vertex[1].u = (vertex[1].u - vertex[0].u) * fwork + vertex[0].u;
		vertex[1].v = (vertex[1].v - vertex[0].v) * fwork + vertex[0].v;
		vertex[1].r = (vertex[1].r - vertex[0].r) * fwork + vertex[0].r;
		vertex[1].g = (vertex[1].g - vertex[0].g) * fwork + vertex[0].g;
		vertex[1].b = (vertex[1].b - vertex[0].b) * fwork + vertex[0].b;
		vertex[1].intensidad =
		    (vertex[1].intensidad - vertex[0].intensidad) * fwork +
		    vertex[0].intensidad;
		// Proyectar
		vertex[1].fpx = (llrender_struct->fancho_pantalla / 2) +
				(vertex[1].sx * llrender_struct->proyeccion_x);
		vertex[1].fpy = (llrender_struct->falto_pantalla / 2) -
				(vertex[1].sy * llrender_struct->proyeccion_y);
		vertex[1].fpz = 1.0;
		vertex[1].sutherland_cohen = 0;
		if (vertex[1].fpx < 0.0)
			vertex[1].sutherland_cohen += 1;
		if (vertex[1].fpx > llrender_struct->fancho_pantalla)
			vertex[1].sutherland_cohen += 4;
		if (vertex[1].fpy < 0.0)
			vertex[1].sutherland_cohen += 16;
		if (vertex[1].fpy > llrender_struct->falto_pantalla)
			vertex[1].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = (1.0 - vertex[0].fpz) / (vertex[2].fpz - vertex[0].fpz);
		vertex[2].sx =
		    (vertex[2].sx - vertex[0].sx) * fwork + vertex[0].sx;
		vertex[2].sy =
		    (vertex[2].sy - vertex[0].sy) * fwork + vertex[0].sy;
		vertex[2].u = (vertex[2].u - vertex[0].u) * fwork + vertex[0].u;
		vertex[2].v = (vertex[2].v - vertex[0].v) * fwork + vertex[0].v;
		vertex[2].r = (vertex[2].r - vertex[0].r) * fwork + vertex[0].r;
		vertex[2].g = (vertex[2].g - vertex[0].g) * fwork + vertex[0].g;
		vertex[2].b = (vertex[2].b - vertex[0].b) * fwork + vertex[0].b;
		vertex[2].intensidad =
		    (vertex[2].intensidad - vertex[0].intensidad) * fwork +
		    vertex[0].intensidad;
		// Proyectar
		vertex[2].fpx = (llrender_struct->fancho_pantalla / 2) +
				(vertex[2].sx * llrender_struct->proyeccion_x);
		vertex[2].fpy = (llrender_struct->falto_pantalla / 2) -
				(vertex[2].sy * llrender_struct->proyeccion_y);
		vertex[2].fpz = 1.0;
		vertex[2].sutherland_cohen = 0;
		if (vertex[2].fpx < 0.0)
			vertex[2].sutherland_cohen += 1;
		if (vertex[2].fpx > llrender_struct->fancho_pantalla)
			vertex[2].sutherland_cohen += 4;
		if (vertex[2].fpy < 0.0)
			vertex[2].sutherland_cohen += 16;
		if (vertex[2].fpy > llrender_struct->falto_pantalla)
			vertex[2].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_left(llrender_struct, &face);
	} else { // El resultado del corte son dos triángulos
		// Partimos de un punto situado detras del plano de clipping
		if (face.v1->fpz < 1.0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpz < 1.0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		// Corte 1 con el plano de clipping
		fwork = (1.0 - vertex[0].fpz) / (vertex[2].fpz - vertex[0].fpz);
		vertex[3].sx =
		    (vertex[2].sx - vertex[0].sx) * fwork + vertex[0].sx;
		vertex[3].sy =
		    (vertex[2].sy - vertex[0].sy) * fwork + vertex[0].sy;
		vertex[3].u = (vertex[2].u - vertex[0].u) * fwork + vertex[0].u;
		vertex[3].v = (vertex[2].v - vertex[0].v) * fwork + vertex[0].v;
		vertex[3].r = (vertex[2].r - vertex[0].r) * fwork + vertex[0].r;
		vertex[3].g = (vertex[2].g - vertex[0].g) * fwork + vertex[0].g;
		vertex[3].b = (vertex[2].b - vertex[0].b) * fwork + vertex[0].b;
		vertex[3].intensidad =
		    (vertex[2].intensidad - vertex[0].intensidad) * fwork +
		    vertex[0].intensidad;
		// Proyectar
		vertex[3].fpx = (llrender_struct->fancho_pantalla / 2) +
				(vertex[3].sx * llrender_struct->proyeccion_x);
		vertex[3].fpy = (llrender_struct->falto_pantalla / 2) -
				(vertex[3].sy * llrender_struct->proyeccion_y);
		vertex[3].fpz = 1.0;
		vertex[3].sutherland_cohen = 0;
		if (vertex[3].fpx < 0.0)
			vertex[3].sutherland_cohen += 1;
		if (vertex[3].fpx > llrender_struct->fancho_pantalla)
			vertex[3].sutherland_cohen += 4;
		if (vertex[3].fpy < 0.0)
			vertex[3].sutherland_cohen += 16;
		if (vertex[3].fpy > llrender_struct->falto_pantalla)
			vertex[3].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = (1.0 - vertex[0].fpz) / (vertex[1].fpz - vertex[0].fpz);
		vertex[0].sx =
		    (vertex[1].sx - vertex[0].sx) * fwork + vertex[0].sx;
		vertex[0].sy =
		    (vertex[1].sy - vertex[0].sy) * fwork + vertex[0].sy;
		vertex[0].u = (vertex[1].u - vertex[0].u) * fwork + vertex[0].u;
		vertex[0].v = (vertex[1].v - vertex[0].v) * fwork + vertex[0].v;
		vertex[0].r = (vertex[1].r - vertex[0].r) * fwork + vertex[0].r;
		vertex[0].g = (vertex[1].g - vertex[0].g) * fwork + vertex[0].g;
		vertex[0].b = (vertex[1].b - vertex[0].b) * fwork + vertex[0].b;
		vertex[0].intensidad =
		    (vertex[1].intensidad - vertex[0].intensidad) * fwork +
		    vertex[0].intensidad;
		// Proyectar
		vertex[0].fpx = (llrender_struct->fancho_pantalla / 2) +
				(vertex[0].sx * llrender_struct->proyeccion_x);
		vertex[0].fpy = (llrender_struct->falto_pantalla / 2) -
				(vertex[0].sy * llrender_struct->proyeccion_y);
		vertex[0].fpz = 1.0;
		vertex[0].sutherland_cohen = 0;
		if (vertex[0].fpx < 0.0)
			vertex[0].sutherland_cohen += 1;
		if (vertex[0].fpx > llrender_struct->fancho_pantalla)
			vertex[0].sutherland_cohen += 4;
		if (vertex[0].fpy < 0.0)
			vertex[0].sutherland_cohen += 16;
		if (vertex[0].fpy > llrender_struct->falto_pantalla)
			vertex[0].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_left(llrender_struct, &face);
		face.v1 = &vertex[0];
		face.v2 = &vertex[2];
		face.v3 = &vertex[3];
		face.v3->estado = 0;
		llrender_clipping_3d_left(llrender_struct, &face);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_left(llrender *llrender_struct, lptface face_in)
{
	long i = 0;
	tface face;
	tvertex vertex[4];
	float fwork;
	float cero_z, uno_z, dos_z;

	//  Calculamos cuantos puntos están situados fuera de la pantalla
	i = face_in->v1->sutherland_cohen + face_in->v2->sutherland_cohen +
	    face_in->v3->sutherland_cohen;
	if (!i) {
		llrender_pintar_triangulo(llrender_struct, face_in);
		return;
	}
	i = i & 3;
	if (i == 3)
		return; // El polígono está fuera de pantalla
	if (i == 0) {
		llrender_clipping_3d_right(
		    llrender_struct,
		    face_in); // El polígono está dentro de pantalla
		return;
	}

	memcpy(&face, face_in, sizeof(tface));
	face.v1->estado = 0;
	face.v2->estado = 0;
	face.v3->estado = 0;

	if (i == 1) { // El resultado del corte son dos triángulos
		// Partimos de un punto situado encima de la pantalla
		if (face.v1->fpx < 0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpx < 0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el plano de clipping
		fwork = -vertex[0].fpx / (vertex[2].fpx - vertex[0].fpx);
		vertex[3].fpx = 0;
		vertex[3].fpy =
		    (vertex[2].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[3].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].sutherland_cohen = 0;
		if (vertex[3].fpx > llrender_struct->fancho_pantalla)
			vertex[3].sutherland_cohen += 4;
		if (vertex[3].fpy < 0.0)
			vertex[3].sutherland_cohen += 16;
		if (vertex[3].fpy > llrender_struct->falto_pantalla)
			vertex[3].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = -vertex[0].fpx / (vertex[1].fpx - vertex[0].fpx);
		vertex[0].fpx = 0;
		vertex[0].fpy =
		    (vertex[1].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[0].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].sutherland_cohen = 0;
		if (vertex[0].fpx > llrender_struct->fancho_pantalla)
			vertex[0].sutherland_cohen += 4;
		if (vertex[0].fpy < 0.0)
			vertex[0].sutherland_cohen += 16;
		if (vertex[0].fpy > llrender_struct->falto_pantalla)
			vertex[0].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_right(llrender_struct, &face);
		face.v1 = &vertex[0];
		face.v2 = &vertex[2];
		face.v3 = &vertex[3];
		face.v3->estado = 0;
		llrender_clipping_3d_right(llrender_struct, &face);
	} else { // El resultado del corte es un triángulo
		// Partimos de un punto situado debajo del margen superior de
		// pantalla
		if (face.v1->fpx >= 0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpx >= 0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el margen superior de la pantalla
		fwork = -vertex[0].fpx / (vertex[1].fpx - vertex[0].fpx);
		vertex[1].fpx = 0;
		vertex[1].fpy =
		    (vertex[1].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[1].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].sutherland_cohen = 0;
		if (vertex[1].fpx > llrender_struct->fancho_pantalla)
			vertex[1].sutherland_cohen += 4;
		if (vertex[1].fpy < 0.0)
			vertex[1].sutherland_cohen += 16;
		if (vertex[1].fpy > llrender_struct->falto_pantalla)
			vertex[1].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = -vertex[0].fpx / (vertex[2].fpx - vertex[0].fpx);
		vertex[2].fpx = 0;
		vertex[2].fpy =
		    (vertex[2].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[2].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].sutherland_cohen = 0;
		if (vertex[2].fpx > llrender_struct->fancho_pantalla)
			vertex[2].sutherland_cohen += 4;
		if (vertex[2].fpy < 0.0)
			vertex[2].sutherland_cohen += 16;
		if (vertex[2].fpy > llrender_struct->falto_pantalla)
			vertex[2].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_right(llrender_struct, &face);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_right(llrender *llrender_struct, lptface face_in)
{
	long i = 0;
	tface face;
	tvertex vertex[4];
	float fwork;
	float cero_z, uno_z, dos_z;

	//  Calculamos cuantos puntos están situados fuera de la pantalla
	i = face_in->v1->sutherland_cohen + face_in->v2->sutherland_cohen +
	    face_in->v3->sutherland_cohen;
	if (!i) {
		llrender_pintar_triangulo(llrender_struct, face_in);
		return;
	}
	i = (i >> 2) & 3;
	if (i == 3)
		return; // El polígono está fuera de pantalla
	if (i == 0) {
		llrender_clipping_3d_up(
		    llrender_struct,
		    face_in); // El polígono está dentro de pantalla
		return;
	}

	memcpy(&face, face_in, sizeof(tface));
	face.v1->estado = 0;
	face.v2->estado = 0;
	face.v3->estado = 0;

	if (i == 1) { // El resultado del corte son dos triángulos
		// Partimos de un punto situado encima de la pantalla
		if (face.v1->fpx > llrender_struct->fancho_pantalla) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpx > llrender_struct->fancho_pantalla) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el plano de clipping
		fwork = (llrender_struct->fancho_pantalla - vertex[0].fpx) /
			(vertex[2].fpx - vertex[0].fpx);
		vertex[3].fpx = llrender_struct->fancho_pantalla;
		vertex[3].fpy =
		    (vertex[2].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[3].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].sutherland_cohen = 0;
		if (vertex[3].fpy < 0.0)
			vertex[3].sutherland_cohen += 16;
		if (vertex[3].fpy > llrender_struct->falto_pantalla)
			vertex[3].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = (llrender_struct->fancho_pantalla - vertex[0].fpx) /
			(vertex[1].fpx - vertex[0].fpx);
		vertex[0].fpx = llrender_struct->fancho_pantalla;
		vertex[0].fpy =
		    (vertex[1].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[0].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].sutherland_cohen = 0;
		if (vertex[0].fpy < 0.0)
			vertex[0].sutherland_cohen += 16;
		if (vertex[0].fpy > llrender_struct->falto_pantalla)
			vertex[0].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_up(llrender_struct, &face);
		face.v1 = &vertex[0];
		face.v2 = &vertex[2];
		face.v3 = &vertex[3];
		face.v3->estado = 0;
		llrender_clipping_3d_up(llrender_struct, &face);
	} else { // El resultado del corte es un triángulo
		// Partimos de un punto situado debajo del margen superior de
		// pantalla
		if (face.v1->fpx <= llrender_struct->fancho_pantalla) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpx <= llrender_struct->fancho_pantalla) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el margen superior de la pantalla
		fwork = (llrender_struct->fancho_pantalla - vertex[0].fpx) /
			(vertex[1].fpx - vertex[0].fpx);
		vertex[1].fpx = llrender_struct->fancho_pantalla;
		vertex[1].fpy =
		    (vertex[1].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[1].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].sutherland_cohen = 0;
		if (vertex[1].fpy < 0.0)
			vertex[1].sutherland_cohen += 16;
		if (vertex[1].fpy > llrender_struct->falto_pantalla)
			vertex[1].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = (llrender_struct->fancho_pantalla - vertex[0].fpx) /
			(vertex[2].fpx - vertex[0].fpx);
		vertex[2].fpx = llrender_struct->fancho_pantalla;
		vertex[2].fpy =
		    (vertex[2].fpy - vertex[0].fpy) * fwork + vertex[0].fpy;
		vertex[2].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].sutherland_cohen = 0;
		if (vertex[2].fpy < 0.0)
			vertex[2].sutherland_cohen += 16;
		if (vertex[2].fpy > llrender_struct->falto_pantalla)
			vertex[2].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_up(llrender_struct, &face);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_up(llrender *llrender_struct, lptface face_in)
{
	long i = 0;
	tface face;
	tvertex vertex[4];
	float fwork;
	float cero_z, uno_z, dos_z;

	//  Calculamos cuantos puntos están situados fuera de la pantalla
	i = face_in->v1->sutherland_cohen + face_in->v2->sutherland_cohen +
	    face_in->v3->sutherland_cohen;
	if (!i) {
		llrender_pintar_triangulo(llrender_struct, face_in);
		return;
	}
	i = (i >> 4) & 3;
	if (i == 3)
		return; // El polígono está fuera de pantalla
	if (i == 0) {
		llrender_clipping_3d_down(
		    llrender_struct,
		    face_in); // El polígono está dentro de pantalla
		return;
	}

	memcpy(&face, face_in, sizeof(tface));
	face.v1->estado = 0;
	face.v2->estado = 0;
	face.v3->estado = 0;

	if (i == 1) { // El resultado del corte son dos triángulos
		// Partimos de un punto situado encima de la pantalla
		if (face.v1->fpy < 0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpy < 0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el plano de clipping
		fwork = -vertex[0].fpy / (vertex[2].fpy - vertex[0].fpy);
		vertex[3].fpx =
		    (vertex[2].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[3].fpy = 0;
		vertex[3].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].sutherland_cohen = 0;
		if (vertex[3].fpy > llrender_struct->falto_pantalla)
			vertex[3].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = -vertex[0].fpy / (vertex[1].fpy - vertex[0].fpy);
		vertex[0].fpx =
		    (vertex[1].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[0].fpy = 0;
		vertex[0].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].sutherland_cohen = 0;
		if (vertex[0].fpy > llrender_struct->falto_pantalla)
			vertex[0].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_down(llrender_struct, &face);
		face.v1 = &vertex[0];
		face.v2 = &vertex[2];
		face.v3 = &vertex[3];
		face.v3->estado = 0;
		llrender_clipping_3d_down(llrender_struct, &face);
	} else { // El resultado del corte es un triángulo
		// Partimos de un punto situado debajo del margen superior de
		// pantalla
		if (face.v1->fpy >= 0) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpy >= 0) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el margen superior de la pantalla
		fwork = -vertex[0].fpy / (vertex[1].fpy - vertex[0].fpy);
		vertex[1].fpx =
		    (vertex[1].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[1].fpy = 0;
		vertex[1].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].sutherland_cohen = 0;
		if (vertex[1].fpy > llrender_struct->falto_pantalla)
			vertex[1].sutherland_cohen += 64;

		// Corte 2 con el plano de clipping
		fwork = -vertex[0].fpy / (vertex[2].fpy - vertex[0].fpy);
		vertex[2].fpx =
		    (vertex[2].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[2].fpy = 0;
		vertex[2].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].sutherland_cohen = 0;
		if (vertex[2].fpy > llrender_struct->falto_pantalla)
			vertex[2].sutherland_cohen += 64;

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_clipping_3d_down(llrender_struct, &face);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//
//─────────────────────────────────────────────────────────────────────────────

void llrender_clipping_3d_down(llrender *llrender_struct, lptface face_in)
{
	long i = 0;
	tface face;
	tvertex vertex[4];
	float fwork;
	float cero_z, uno_z, dos_z;

	//  Calculamos cuantos puntos están situados fuera de la pantalla
	i = face_in->v1->sutherland_cohen + face_in->v2->sutherland_cohen +
	    face_in->v3->sutherland_cohen;
	if (!i) {
		llrender_pintar_triangulo(llrender_struct, face_in);
		return;
	}
	i = (i >> 6) & 3;
	if (i == 3)
		return; // El polígono está fuera de pantalla
	if (i == 0) {
		llrender_pintar_triangulo(
		    llrender_struct,
		    face_in); // El polígono está dentro de pantalla
		return;
	}

	memcpy(&face, face_in, sizeof(tface));
	face.v1->estado = 0;
	face.v2->estado = 0;
	face.v3->estado = 0;

	if (i == 1) { // El resultado del corte son dos triángulos
		// Partimos de un punto situado encima de la pantalla

		if (face.v1->fpy > llrender_struct->falto_pantalla) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpy > llrender_struct->falto_pantalla) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el plano de clipping
		fwork = (llrender_struct->falto_pantalla - vertex[0].fpy) /
			(vertex[2].fpy - vertex[0].fpy);
		vertex[3].fpx =
		    (vertex[2].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[3].fpy = llrender_struct->falto_pantalla;
		vertex[3].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[3].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);

		// Corte 2 con el plano de clipping
		fwork = (llrender_struct->falto_pantalla - vertex[0].fpy) /
			(vertex[1].fpy - vertex[0].fpy);
		vertex[0].fpx =
		    (vertex[1].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[0].fpy = llrender_struct->falto_pantalla;
		vertex[0].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[0].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_pintar_triangulo(llrender_struct, &face);
		face.v1 = &vertex[0];
		face.v2 = &vertex[2];
		face.v3 = &vertex[3];
		face.v3->estado = 0;
		llrender_pintar_triangulo(llrender_struct, &face);
	} else { // El resultado del corte es un triángulo
		// Partimos de un punto situado debajo del margen superior de
		// pantalla
		if (face.v1->fpy <= llrender_struct->falto_pantalla) {
			memcpy(&vertex[0], face.v1, sizeof(tvertex));
			memcpy(&vertex[1], face.v2, sizeof(tvertex));
			memcpy(&vertex[2], face.v3, sizeof(tvertex));
		} else if (face.v2->fpy <= llrender_struct->falto_pantalla) {
			memcpy(&vertex[0], face.v2, sizeof(tvertex));
			memcpy(&vertex[1], face.v3, sizeof(tvertex));
			memcpy(&vertex[2], face.v1, sizeof(tvertex));
		} else {
			memcpy(&vertex[0], face.v3, sizeof(tvertex));
			memcpy(&vertex[1], face.v1, sizeof(tvertex));
			memcpy(&vertex[2], face.v2, sizeof(tvertex));
		}
		cero_z = 1.0 / vertex[0].fpz;
		uno_z = 1.0 / vertex[1].fpz;
		dos_z = 1.0 / vertex[2].fpz;
		// Corte 1 con el margen superior de la pantalla
		fwork = (llrender_struct->falto_pantalla - vertex[0].fpy) /
			(vertex[1].fpy - vertex[0].fpy);
		vertex[1].fpx =
		    (vertex[1].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[1].fpy = llrender_struct->falto_pantalla;
		vertex[1].u =
		    ((vertex[1].u * uno_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].v =
		    ((vertex[1].v * uno_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].r =
		    ((vertex[1].r * uno_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].g =
		    ((vertex[1].g * uno_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].b =
		    ((vertex[1].b * uno_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].intensidad = ((vertex[1].intensidad * uno_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((uno_z - cero_z) * fwork + cero_z);
		vertex[1].fpz = 1 / ((uno_z - cero_z) * fwork + cero_z);

		// Corte 2 con el plano de clipping
		fwork = (llrender_struct->falto_pantalla - vertex[0].fpy) /
			(vertex[2].fpy - vertex[0].fpy);
		vertex[2].fpx =
		    (vertex[2].fpx - vertex[0].fpx) * fwork + vertex[0].fpx;
		vertex[2].fpy = llrender_struct->falto_pantalla;
		vertex[2].u =
		    ((vertex[2].u * dos_z - vertex[0].u * cero_z) * fwork +
		     vertex[0].u * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].v =
		    ((vertex[2].v * dos_z - vertex[0].v * cero_z) * fwork +
		     vertex[0].v * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].r =
		    ((vertex[2].r * dos_z - vertex[0].r * cero_z) * fwork +
		     vertex[0].r * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].g =
		    ((vertex[2].g * dos_z - vertex[0].g * cero_z) * fwork +
		     vertex[0].g * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].b =
		    ((vertex[2].b * dos_z - vertex[0].b * cero_z) * fwork +
		     vertex[0].b * cero_z) /
		    ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].intensidad = ((vertex[2].intensidad * dos_z -
					 vertex[0].intensidad * cero_z) *
					    fwork +
					vertex[0].intensidad * cero_z) /
				       ((dos_z - cero_z) * fwork + cero_z);
		vertex[2].fpz = 1 / ((dos_z - cero_z) * fwork + cero_z);

		// Pintar
		face.v1 = &vertex[0];
		face.v2 = &vertex[1];
		face.v3 = &vertex[2];
		llrender_pintar_triangulo(llrender_struct, &face);
	}
}

//─────────────────────────────────────────────────────────────────────────────
//  Maps a texture on a triangle with gouraud
//─────────────────────────────────────────────────────────────────────────────

int dp = 0;
float fids[255];

void inicio_division(float *fp)
{

	fids[dp] = *fp;
	dp++;
}

void fin_division()
{
	inversa = (float)fids[dp];
	dp--;
}
void asignacion_u_v(void)
{
	u_0 = (uint32_t)(inversa * fu);
	v_0 = (uint32_t)(inversa * fv);
}

#ifndef UINT16_C
#define UINT16_C(value) (uint16_t)(value)
//__CONCAT(value,U)
#endif
static inline void nucleo_combinado(void)
{
	uint16_t output_pixel = 0;
	uint16_t mask = (mr >> 3) & UINT16_C(0x7E0);
	uint16_t index = (color_b & UINT16_C(0xF800)) + mask + alfa;
	output_pixel = tabla_rgba[index];
	output_pixel <<= 5;
	mask = (mg >> 3) & UINT16_C(0x7E0);
	index = ((color_b & UINT16_C(0x7C0)) << 5) + mask + alfa;
	output_pixel += tabla_rgba[index];
	output_pixel <<= 6;
	mask = (mb >> 3) & UINT16_C(0x7E0);
	index = ((color_b & UINT16_C(0x1F)) << 11) + mask + alfa;
	output_pixel += tabla_rgba[index];
	*destino = output_pixel;
	//    printf("output pixel: %d\n",output_pixel);
}

void nucleo1(void) { nucleo_combinado(); }

void nucleo2(void) {}

void llrender_Pinta_Triangulo(llrender *llrender_struct, lptface face,
			      int mmlevel, int tipo)
{
	int altura;
	float faltura, temp, largo;
	int x1, u, v, dudx, dvdx;
	short *ptr_pantalla;
	///////////////////////////////////////////////////////
	lptmaterial mat = face->material;
	int ancho = mat->textura->Ancho[mmlevel];
	int alto = mat->textura->Alto[mmlevel];
	short *dir_textura = (short *)mat->textura->Levels[mmlevel];
	///////////////////////////////////////////////////////
	float anchot = (float)ancho;
	float altot = (float)ancho;
	long mascara = (ancho - 1) * 65536;
	long rotacion_mascara;
	lptvertex v1 = face->v1;
	lptvertex v2 = face->v2;
	lptvertex v3 = face->v3;
	lptvertex vertice;
	float unoz_aux, dunoz_aux, fdudx, fdvdx;
	float dxdy, prestep;
	int preincremento;
	long v1_r, v1_g, v1_b;
	long v2_r, v2_g, v2_b;
	long v3_r, v3_g, v3_b;
	long v1_alpha, v2_alpha, v3_alpha;
	long col_r, col_g, col_b, color;
	float ru1, ru2, ru3, rv1, rv2, rv3;

	direccion_textura = (long)mat->textura->Levels[mmlevel];

	if (!(tipo & T_TEXTURA)) {
		col_r = schop(face->material->dr * 65536.0) & 0xf800;
		col_g = schop(face->material->dg * 2048.0) & 0x07e0;
		col_b = schop(face->material->db * 32.0) & 0x1f;
		color = col_r + col_g + col_b;
	}
	// rotacion_mascara=10;

	// printf("ancho: %d\n",ancho);

	switch (ancho) {
	case 8: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_8;
      else nucleo_texturas=nucleo8_8;
      * */
		rotacion_mascara = 13;
		break;
	case 16: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_16;
      else nucleo_texturas=nucleo8_16;
      */
		rotacion_mascara = 12;
		break;
	case 32: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_32;
      else nucleo_texturas=nucleo8_32;
      */
		rotacion_mascara = 11;
		break;
	case 64: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_64;
      else nucleo_texturas=nucleo8_64;
      */
		rotacion_mascara = 10;
		break;
	case 128: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_128;
      else nucleo_texturas=nucleo8_128;
      */
		rotacion_mascara = 9;
		break;
	case 256: /*if (tipo & T_MASCARA) nucleo_texturas=mask_nucleo8_256;
      else nucleo_texturas=nucleo8_256;
      */
		rotacion_mascara = 8;
		break;
	}

	// Ordenamos los vertices, v1 el de más arriba y v3 el de más abajo

	if (v1->fpy > v2->fpy) {
		vertice = v1;
		v1 = v2;
		v2 = vertice;
	}
	if (v1->fpy > v3->fpy) {
		vertice = v1;
		v1 = v3;
		v3 = vertice;
	}
	if (v2->fpy > v3->fpy) {
		vertice = v2;
		v2 = v3;
		v3 = vertice;
	}

	if (tipo & T_TEXTURA) {
		if (tipo & T_PERSPECTIVA) {
			ru1 = 65536.0 * v1->u * anchot / v1->fpz;
			ru2 = 65536.0 * v2->u * anchot / v2->fpz;
			ru3 = 65536.0 * v3->u * anchot / v3->fpz;
			rv1 = 65536.0 * v1->v * altot / v1->fpz;
			rv2 = 65536.0 * v2->v * altot / v2->fpz;
			rv3 = 65536.0 * v3->v * altot / v3->fpz;
		} else {
			rv1 = 65536.0 * v1->v * altot;
			rv2 = 65536.0 * v2->v * altot;
			rv3 = 65536.0 * v3->v * altot;
			ru1 = 65536.0 * v1->u * anchot;
			ru2 = 65536.0 * v2->u * anchot;
			ru3 = 65536.0 * v3->u * anchot;
		}
	}

#ifndef NOTYET

	if (tipo & T_GOURAUD) {
		v1_alpha = v1->intensidad * 31;
		v2_alpha = v2->intensidad * 31;
		v3_alpha = v3->intensidad * 31;

		v1_r = v1->r * 63;
		v1_g = v1->g * 63;
		v1_b = v1->b * 63;

		v2_r = v2->r * 63;
		v2_g = v2->g * 63;
		v2_b = v2->b * 63;

		v3_r = v3->r * 63;
		v3_g = v3->g * 63;
		v3_b = v3->b * 63;
	}

	// Calculamos la longitud de la linea más larga
	altura = v3->py - v1->py;
	if ((v3->fpy - v1->fpy) < 0)
		return;
	//  if (altura<=0) return;

	faltura = 1.0 / (v3->fpy - v1->fpy);
	temp = (v2->fpy - v1->fpy) * faltura;
	largo = temp * (v3->fpx - v1->fpx) + (v1->fpx - v2->fpx);
	if (largo == 0)
		return;

	if (largo < 0) {
		seccion_derecha = 2;
		seccion_izquierda = 1;
		altura_seccion_izquierda = altura;
		dxdy = (v3->fpx - v1->fpx) * faltura;
		dx_izq = schop(dxdy * 65536.0);
		prestep = (float)v1->py - v1->fpy;
		x_izq = schop((v1->fpx + prestep * dxdy) * 65536.0);
		// Calculo de las deltas
		if (tipo & T_TEXTURA) {
			if (tipo & T_PERSPECTIVA) {
				duz_izq = (ru3 - ru1) * faltura;
				uz_izq = ru1 + duz_izq * prestep;
				dvz_izq = (rv3 - rv1) * faltura;
				vz_izq = rv1 + dvz_izq * prestep;
				dunoz_izq =
				    (1 / v3->fpz - 1 / v1->fpz) * faltura;
				unoz_izq = 1 / v1->fpz + dunoz_izq * prestep;
			} else {
				du_izq = schop((ru3 - ru1) * faltura);
				u_izq = schop(ru1 + (float)du_izq * prestep);
				dv_izq = schop((rv3 - rv1) * faltura);
				v_izq = schop(rv1 + (float)dv_izq * prestep);
			}
		}
		if (tipo & T_GOURAUD) {
			dr_izq = schop(((v3_r - v1_r) * 256.0) * faltura);
			r_izq = schop(v1_r * 256.0 + (float)dr_izq * prestep);
			dg_izq = schop(((v3_g - v1_g) * 256.0) * faltura);
			g_izq = schop(v1_g * 256.0 + (float)dg_izq * prestep);
			db_izq = schop(((v3_b - v1_b) * 256.0) * faltura);
			b_izq = schop(v1_b * 256.0 + (float)db_izq * prestep);
			dintensidad_izq =
			    schop(((v3_alpha - v1_alpha) * 256.0) * faltura);
			intensidad_izq =
			    schop(v1_alpha * 256.0 +
				  (float)dintensidad_izq * prestep);
		}
		altura_seccion_derecha = v2->py - v1->py;
		if (altura_seccion_derecha <= 0) {
			altura_seccion_derecha = v3->py - v2->py;
			if (altura_seccion_derecha <= 0)
				return;
			else {
				// Calculo de las deltas
				dx_der = schop(((v3->fpx - v2->fpx) * 65536.0) /
					       (v3->fpy - v2->fpy));
				faltura = 1.0 / (v3->fpy - v2->fpy);
				dxdy = (v3->fpx - v2->fpx) * faltura;
				prestep = (float)v2->py - v2->fpy;
				x_der =
				    schop((v2->fpx + prestep * dxdy) * 65536.0);
				if ((tipo & T_TEXTURA) &&
				    (tipo & T_PERSPECTIVA)) {
					duz_der = (ru3 - ru2) * faltura;
					uz_der = ru2 + duz_der * prestep;
					dvz_der = (rv3 - rv2) * faltura;
					vz_der = rv2 + dvz_der * prestep;
					dunoz_der =
					    (1 / v3->fpz - 1 / v2->fpz) *
					    faltura;
					unoz_der =
					    1 / v2->fpz + dunoz_der * prestep;
				}
			}
		} else {
			// Calculo de las deltas
			dx_der = schop(((v2->fpx - v1->fpx) * 65536.0) /
				       (v2->fpy - v1->fpy));
			faltura = 1.0 / (v2->fpy - v1->fpy);
			dxdy = (v2->fpx - v1->fpx) * faltura;
			prestep = (float)v1->py - v1->fpy;
			x_der = schop((v1->fpx + prestep * dxdy) * 65536.0);
			if ((tipo & T_TEXTURA) && (tipo & T_PERSPECTIVA)) {
				duz_der = (ru2 - ru1) * faltura;
				uz_der = ru1 + duz_der * prestep;
				dvz_der = (rv2 - rv1) * faltura;
				vz_der = rv1 + dvz_der * prestep;
				dunoz_der =
				    (1 / v2->fpz - 1 / v1->fpz) * faltura;
				unoz_der = 1 / v1->fpz + dunoz_der * prestep;
			}
		}
	} else {
		seccion_izquierda = 2;
		seccion_derecha = 1;
		altura_seccion_derecha = altura;
		dx_der = schop(((v3->fpx - v1->fpx) * 65536.0) /
			       (v3->fpy - v1->fpy));
		faltura = 1.0 / (v3->fpy - v1->fpy);
		dxdy = (v3->fpx - v1->fpx) * faltura;
		prestep = (float)v1->py - v1->fpy;
		x_der = schop((v1->fpx + prestep * dxdy) * 65536.0);
		if ((tipo & T_TEXTURA) && (tipo & T_PERSPECTIVA)) {
			duz_der = (ru3 - ru1) * faltura;
			uz_der = ru1 + duz_der * prestep;
			dvz_der = (rv3 - rv1) * faltura;
			vz_der = rv1 + dvz_der * prestep;
			dunoz_der = (1 / v3->fpz - 1 / v1->fpz) * faltura;
			unoz_der = 1 / v1->fpz + dunoz_der * prestep;
		}
		altura_seccion_izquierda = v2->py - v1->py;
		if (altura_seccion_izquierda <= 0) {
			seccion_izquierda--;
			altura_seccion_izquierda = v3->py - v2->py;
			if (altura_seccion_izquierda <= 0)
				return;
			else {
				faltura = 1.0 / (v3->fpy - v2->fpy);
				dxdy = (v3->fpx - v2->fpx) * faltura;
				dx_izq = schop(dxdy * 65536.0);
				prestep = (float)v2->py - v2->fpy;
				x_izq =
				    schop((v2->fpx + prestep * dxdy) * 65536.0);
				// Calculo de las deltas
				if (tipo & T_TEXTURA) {
					if (tipo & T_PERSPECTIVA) {
						duz_izq = (ru3 - ru2) * faltura;
						uz_izq =
						    ru2 + duz_izq * prestep;
						dvz_izq = (rv3 - rv2) * faltura;
						vz_izq =
						    rv2 + dvz_izq * prestep;
						dunoz_izq = (1 / v3->fpz -
							     1 / v2->fpz) *
							    faltura;
						unoz_izq = 1 / v2->fpz +
							   dunoz_izq * prestep;
					} else {
						du_izq = schop((ru3 - ru2) *
							       faltura);
						u_izq =
						    schop(ru2 + (float)du_izq *
								    prestep);
						dv_izq = schop((rv3 - rv2) *
							       faltura);
						v_izq =
						    schop(rv2 + (float)dv_izq *
								    prestep);
					}
				}
				if (tipo & T_GOURAUD) {
					dr_izq = schop(((v3_r - v2_r) * 256.0) *
						       faltura);
					r_izq = schop(v2_r * 256.0 +
						      (float)dr_izq * prestep);
					dg_izq = schop(((v3_g - v2_g) * 256.0) *
						       faltura);
					g_izq = schop(v2_g * 256.0 +
						      (float)dg_izq * prestep);
					db_izq = schop(((v3_b - v2_b) * 256.0) *
						       faltura);
					b_izq = schop(v2_b * 256.0 +
						      (float)db_izq * prestep);
					dintensidad_izq = schop(
					    ((v3_alpha - v2_alpha) * 256.0) *
					    faltura);
					intensidad_izq = schop(
					    v2_alpha * 256.0 +
					    (float)dintensidad_izq * prestep);
				}
			}
		} else {
			faltura = 1.0 / (v2->fpy - v1->fpy);
			dxdy = (v2->fpx - v1->fpx) * faltura;
			dx_izq = schop(dxdy * 65536.0);
			prestep = (float)v1->py - v1->fpy;
			x_izq = schop((v1->fpx + prestep * dxdy) * 65536.0);
			// Calculo de las deltas
			if (tipo & T_TEXTURA) {
				if (tipo & T_PERSPECTIVA) {
					duz_izq = (ru2 - ru1) * faltura;
					uz_izq = ru1 + duz_izq * prestep;
					dvz_izq = (rv2 - rv1) * faltura;
					vz_izq = rv1 + dvz_izq * prestep;
					dunoz_izq =
					    (1 / v2->fpz - 1 / v1->fpz) *
					    faltura;
					unoz_izq =
					    1 / v1->fpz + dunoz_izq * prestep;
				} else {
					du_izq = schop((ru2 - ru1) * faltura);
					u_izq = schop(ru1 +
						      (float)du_izq * prestep);
					dv_izq = schop((rv2 - rv1) * faltura);
					v_izq = schop(rv1 +
						      (float)dv_izq * prestep);
				}
			}
			if (tipo & T_GOURAUD) {
				dr_izq =
				    schop(((v2_r - v1_r) * 256.0) * faltura);
				r_izq = schop(v1_r * 256.0 +
					      (float)dr_izq * prestep);
				dg_izq =
				    schop(((v2_g - v1_g) * 256.0) * faltura);
				g_izq = schop(v1_g * 256.0 +
					      (float)dg_izq * prestep);
				db_izq =
				    schop(((v2_b - v1_b) * 256.0) * faltura);
				b_izq = schop(v1_b * 256.0 +
					      (float)db_izq * prestep);
				dintensidad_izq = schop(
				    ((v2_alpha - v1_alpha) * 256.0) * faltura);
				intensidad_izq =
				    schop(v1_alpha * 256.0 +
					  (float)dintensidad_izq * prestep);
			}
		}
	}

	if ((tipo & T_TEXTURA) && !(tipo & T_PERSPECTIVA)) {
		dudx = schop((temp * (ru3 - ru1) + (ru1 - ru2)) / largo);
		dvdx = schop((temp * (rv3 - rv1) + (rv1 - rv2)) / largo);
	}

	if (tipo & T_GOURAUD) {
		drdx = schop(256.0 *
			     ((temp * (v3_r - v1_r) + (v1_r - v2_r)) / largo));
		dgdx = schop(256.0 *
			     ((temp * (v3_g - v1_g) + (v1_g - v2_g)) / largo));
		dbdx = schop(256.0 *
			     ((temp * (v3_b - v1_b) + (v1_b - v2_b)) / largo));
		dintensidaddx = schop(256.0 * ((temp * (v3_alpha - v1_alpha) +
						(v1_alpha - v2_alpha)) /
					       largo));
	}

	ptr_pantalla = (short *)(llrender_struct->copia +
				 (v1->py) * llrender_struct->vancho);

	for (;;) { // Bucle que recorre el polígono de arriba a abajo
		x1 = ajuste_entero(x_izq);
		ancho = ajuste_entero(x_der) - x1;

		if (tipo & T_TEXTURA) {
			if (tipo & T_PERSPECTIVA) {
				anchot = 16.0 / (float)ancho;

				inicio_division(&unoz_izq);
				destino = ptr_pantalla + x1;

				if (ancho > 16) {
					preincremento = (x1 << 16) - x_izq;
					//          fu=uz_izq+(float)preincremento*duz_izq/65536.0;
					//          fv=vz_izq+(float)preincremento*dvz_izq/65536.0;
					fu = uz_izq;
					fv = vz_izq;
					fin_division();
					fdudx = (uz_der - uz_izq) * anchot;
					fdvdx = (vz_der - vz_izq) * anchot;
					dunoz_aux =
					    (unoz_der - unoz_izq) * anchot;
					unoz_aux = unoz_izq + dunoz_aux;
					asignacion_u_v();
					inicio_division(&unoz_aux);
					u = u_0;
					v = v_0;
					fu += fdudx;
					fv += fdvdx;
					fin_division();
					asignacion_u_v();
					do {
						dudx = (u_0 - u) >> 4;
						dvdx = (v_0 - v) >> 4;
						fu += fdudx;
						fv += fdvdx;
						unoz_aux += dunoz_aux;
						inicio_division(&unoz_aux);
						nucleo_texturas(v, u, dudx,
								dvdx, 16,
								destino);
						fin_division();
						u = u_0;
						v = v_0;
						asignacion_u_v();
						destino += 16;
						ancho -= 16;
					} while (ancho > 16);
					if (ancho > 0) {
						dudx = (u_0 - u) >> 4;
						dvdx = (v_0 - v) >> 4;
						nucleo_texturas(v, u, dudx,
								dvdx, ancho,
								destino);
					}
				} else if (ancho > 0) {
					//          fu=uz_izq+(float)preincremento*duz_izq/65536.0;
					//          fv=vz_izq+(float)preincremento*dvz_izq/65536.0;
					fu = uz_izq;
					fv = vz_izq;
					fin_division();
					asignacion_u_v();
					inicio_division(&unoz_der);
					u = u_0;
					v = v_0;
					fu = uz_der;
					fv = vz_der;
					fin_division();
					asignacion_u_v();
					if (ancho > 1) {
						dudx = (u_0 - u) / ancho;
						dvdx = (v_0 - v) / ancho;
					} else {
						dudx = u_0 - u;
						dvdx = v_0 - v;
					}
					nucleo_texturas(v, u, dudx, dvdx, ancho,
							destino);
				} else
					fin_division();
			} else if (tipo & T_GOURAUD) {
				if (tipo & T_MASCARA) {
					if (ancho > 0) {
						destino = ptr_pantalla + x1;
						preincremento =
						    (x1 << 16) - x_izq;
						u = u_izq +
						    ((preincremento * dudx) >>
						     16);
						v = v_izq +
						    ((preincremento * dvdx) >>
						     16);
						mr = r_izq;
						mg = g_izq;
						mb = b_izq;
						intensidad = intensidad_izq;
						do {
							alfa = intensidad >> 8;
							if ((color_b = dir_textura
								 [((v &
								    mascara) >>
								   rotacion_mascara) +
								  ((u &
								    mascara) >>
								   16)]) != 0) {

								nucleo1();
								nucleo2();
							}
							mr += drdx;
							mg += dgdx;
							mb += dbdx;
							intensidad +=
							    dintensidaddx;
							destino++;
							u += dudx;
							v += dvdx;
						} while (--ancho);
					}
				} else {
					if (ancho > 0) {
						destino = ptr_pantalla + x1;
						preincremento =
						    (x1 << 16) - x_izq;
						u = u_izq +
						    ((preincremento * dudx) >>
						     16);
						v = v_izq +
						    ((preincremento * dvdx) >>
						     16);
						mr = r_izq;
						mg = g_izq;
						mb = b_izq;
						intensidad = intensidad_izq;
						do {
							alfa = intensidad >> 8;
							color_b = dir_textura
							    [((v & mascara) >>
							      rotacion_mascara) +
							     ((u & mascara) >>
							      16)];
							nucleo1();
							nucleo2();
							mr += drdx;
							mg += dgdx;
							mb += dbdx;
							intensidad +=
							    dintensidaddx;
							destino++;
							u += dudx;
							v += dvdx;
						} while (--ancho);
					}
				}
			}
			/*
      else if (tipo & T_TRANSLUCIDO) {
	if (tipo & T_MASCARA) {
	  if (ancho>0) {
	    destino=ptr_pantalla+x1;
	    preincremento=(x1<<16)-x_izq;
	    u=u_izq+((preincremento*dudx)>>16);
	    v=v_izq+((preincremento*dvdx)>>16);
	    do {
	      if
      (color=dir_textura[((v&mascara)>>rotacion_mascara)+((u&mascara)>>16)]) {
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
		  destino++;
		u+=dudx;
		v+=dvdx;
	    } while (--ancho);
	  }
	}
	else {
	  if (ancho>0) {
	    destino=ptr_pantalla+x1;
	    preincremento=(x1<<16)-x_izq;
	    u=u_izq+((preincremento*dudx)>>16);
	    v=v_izq+((preincremento*dvdx)>>16);
	    do {
		color=dir_textura[((v&mascara)>>rotacion_mascara)+((u&mascara)>>16)];
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
		destino++;
		  u+=dudx;
		v+=dvdx;
	    } while (--ancho);
	  }
	}
      }
*/
			else {
				if (ancho > 0) {
					destino = ptr_pantalla + x1;
					nucleo_texturas(v_izq, u_izq, dudx,
							dvdx, ancho, destino);
				}
			}
		}
		/*
    else {
      if (tipo & T_TRANSLUCIDO) {
	if (ancho>0) {
	  destino=ptr_pantalla+x1;
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
      else if (tipo & T_GOURAUD) {
	if (ancho>0) {
	  destino=ptr_pantalla+x1;
	  mr=r_izq; mg=g_izq; mb=b_izq;
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
	    destino++;
		  mr+=drdx; mg+=dgdx; mb+=dbdx;
	  } while (--ancho);
	}
      }
      else {
	if (ancho>0) {
	  destino=ptr_pantalla+x1;
	  lf_set(destino,color,ancho);
	}
      }
    }
*/
		ptr_pantalla += llrender_struct->ancho_pantalla;

		if (--altura_seccion_izquierda <= 0) {
			if (--seccion_izquierda <= 0)
				return;
			altura_seccion_izquierda = v3->py - v2->py;
			if (altura_seccion_izquierda <= 0)
				return;
			else {
				faltura = 1.0 / (v3->fpy - v2->fpy);
				dxdy = (v3->fpx - v2->fpx) * faltura;
				dx_izq = schop(dxdy * 65536.0);
				prestep = (float)v2->py - v2->fpy;
				x_izq =
				    schop((v2->fpx + prestep * dxdy) * 65536.0);
				// Calculo de las deltas
				if (tipo & T_TEXTURA) {
					if (tipo & T_PERSPECTIVA) {
						duz_izq = (ru3 - ru2) * faltura;
						uz_izq =
						    ru2 + duz_izq * prestep;
						dvz_izq = (rv3 - rv2) * faltura;
						vz_izq =
						    rv2 + dvz_izq * prestep;
						dunoz_izq = (1 / v3->fpz -
							     1 / v2->fpz) *
							    faltura;
						unoz_izq = 1 / v2->fpz +
							   dunoz_izq * prestep;
					} else {
						du_izq = schop((ru3 - ru2) *
							       faltura);
						u_izq =
						    schop(ru2 + (float)du_izq *
								    prestep);
						dv_izq = schop((rv3 - rv2) *
							       faltura);
						v_izq =
						    schop(rv2 + (float)dv_izq *
								    prestep);
					}
				}
				if (tipo & T_GOURAUD) {
					dr_izq = schop(((v3_r - v2_r) * 256.0) *
						       faltura);
					r_izq = schop(v2_r * 256.0 +
						      (float)dr_izq * prestep);
					dg_izq = schop(((v3_g - v2_g) * 256.0) *
						       faltura);
					g_izq = schop(v2_g * 256.0 +
						      (float)dg_izq * prestep);
					db_izq = schop(((v3_b - v2_b) * 256.0) *
						       faltura);
					b_izq = schop(v2_b * 256.0 +
						      (float)db_izq * prestep);
					dintensidad_izq = schop(
					    ((v3_alpha - v2_alpha) * 256.0) *
					    faltura);
					intensidad_izq = schop(
					    v2_alpha * 256.0 +
					    (float)dintensidad_izq * prestep);
				}
			}
		} else {
			x_izq += dx_izq;
			if (tipo & T_TEXTURA) {
				if (tipo & T_PERSPECTIVA) {
					uz_izq += duz_izq;
					vz_izq += dvz_izq;
					unoz_izq += dunoz_izq;
				} else {
					u_izq += du_izq;
					v_izq += dv_izq;
				}
			}
			if (tipo & T_GOURAUD) {
				r_izq += dr_izq;
				g_izq += dg_izq;
				b_izq += db_izq;
				intensidad_izq += dintensidad_izq;
			}
		}

		if (--altura_seccion_derecha <= 0) {
			if (--seccion_derecha <= 0)
				return;
			altura_seccion_derecha = v3->py - v2->py;
			if (altura_seccion_derecha <= 0)
				return;
			else {
				// Calculo de las deltas
				dx_der = schop(((v3->fpx - v2->fpx) * 65536.0) /
					       (v3->fpy - v2->fpy));
				faltura = 1.0 / (v3->fpy - v2->fpy);
				dxdy = (v3->fpx - v2->fpx) * faltura;
				prestep = (float)v2->py - v2->fpy;
				x_der =
				    schop((v2->fpx + prestep * dxdy) * 65536.0);
				if ((tipo & T_TEXTURA) &&
				    (tipo & T_PERSPECTIVA)) {
					duz_der = (ru3 - ru2) * faltura;
					uz_der = ru2 + duz_der * prestep;
					dvz_der = (rv3 - rv2) * faltura;
					vz_der = rv2 + dvz_der * prestep;
					dunoz_der =
					    (1 / v3->fpz - 1 / v2->fpz) *
					    faltura;
					unoz_der =
					    1 / v2->fpz + dunoz_der * prestep;
				}
			}
		} else {
			x_der += dx_der;
			if ((tipo & T_TEXTURA) && (tipo & T_PERSPECTIVA)) {
				uz_der += duz_der;
				vz_der += dvz_der;
				unoz_der += dunoz_der;
			}
		}
	}
#endif
}
