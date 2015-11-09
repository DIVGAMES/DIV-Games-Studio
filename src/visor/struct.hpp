#ifndef __STRUCT_HPP_
#define __STRUCT_HPP_

#include "resource.hpp"

/********************************************************/
/* Estructuras basicas para la engine 3D                */
/********************************************************/

/* MATRIZ ***********************************************/

typedef struct _matrix{
    float   _00,_01,_02,_03;
    float   _10,_11,_12,_13;
    float   _20,_21,_22,_23;
    float   _30,_31,_32,_33;
}tmatrix,*lptmatrix;

/* TEXTURA **********************************************/

typedef struct _textura{
  int           ancho,alto; // Tama¤o de la textura.
  char          *buffer;    // Buffer de texturas.
// MipMapping
  char          *Levels[4]; // Puntero a los niveles.
  int           Ancho[4];   // Puntero a los niveles.
  int           Alto[4];    // Puntero a los niveles.
}ttextura,*lpttextura;

/* MATERIAL *********************************************/

#define T_FLAT        0
#define T_TEXTURA     1
#define T_GOURAUD     2
#define T_PERSPECTIVA 4
#define T_TRANSLUCIDO 8
#define T_MASCARA     16
#define T_SOMBRA      32
#define T_ANTIALIAS   64

// Tipos de mapeado
#define FLAT          0
#define GOURAUD       1
#define TRANSLUCIDO   2
#define TEXTURA       3
#define GOURAUD_TEXT  4
#define TRANSPARENTE  5
#define GHOST         6

#define TEXTURA_Z_CONST 7

#define FLAT_TRANS    8
#define TEXT_TRANS    9
#define TEXT_PER_TRANS 10
#define GOURAUD_PER   11
#define TRANSPARENTE_MASCARA 12
#define TEXT_PER      13
#define GOUR_TEXT_PER 14
#define TRANS_PER     15
#define TEXT_BILINEAR 16
#define TEXT_GOUR     17
#define TEXT_ILUM     18
#define SOMBRA        19
#define GOUR_MASK     20
#define WIRE          21
#define ANTIALIAS     22

typedef struct _material{
  float       dr,dg,db;     // Color de la luz difusa que afecta al objeto.
  char        dac_difusa;   // Indice sobre el dac del color difuso
  float       ar,ag,ab;     // Color de la luz ambiente que afecta al objeto.
  char        dac_ambient;  // Indice sobre el dac del color ambiente
  float       sr,sg,sb;     // Color de la luz especular que afecta al objeto.
  char        dac_especular;// Indice sobre el dac del color especular
  lpttextura  textura;      // Puntero al buffer de textura.
  int         tipo_map;     // Tipo de mapeado
}tmaterial,*lptmaterial;

/* VERTICE **********************************************/

typedef struct _vertex{
  float       rx,ry,rz;     // Coordenadas reales.
  float       sx,sy,sz;     // Coordenadas espaciales.
  float       fpx,fpy,fpz;  // Coordenadas de pantalla en float.
  int         px,py,pz;     // Coordenadas de pantalla.

  float       rnx,rny,rnz;  // Normales del vertice reales.
  float       snx,sny,snz;  // Normales del vertice espaciales.
  float       r,g,b;        // Valores rgb del vertice.
  float       intensidad;   // Valores de intensidad.

  float       u,v;          // Coordenadas de textura.
  float       ru,rv;        // Coordenadas de textura.
  int         estado;       // Para evitar tratarlo varias veces
  int         sutherland_cohen; // Para el clipping 3D
  int         basura[4];
}tvertex,*lptvertex;

/* CARA *************************************************/

typedef struct _face{
  lptvertex   v1;           // Puntero al vertice 1.
  lptvertex   v2;           // Puntero al vertice 2.
  lptvertex   v3;           // Puntero al vertice 3.
  float       rnx,rny,rnz;  // Normal de la cara reales.
  float       snx,sny,snz;  // Normal de la cara espaciales.
  float       co;
  lptmaterial material;     // Puntero al material de esta cara.
  int         tipo_map;
//  int         bas[4];
}tface,*lptface;

/* BOUNDING BOX *****************************************/

typedef struct _bbox{
  float       x1,y1,z1;     // Esquina superior izquierda.
  float       x2,y2,z2;     // Esquina inferior derecha.
}tbbox,*lptbbox;

/* CINTA AISLANTE *****************************************/

typedef struct _tape{
  int         Obj1,Vert1;
  int         Obj2,Vert2;
  int         LinkMode;
}ttape,*lpttape;

/* OBJETO ***********************************************/

typedef struct _object{
  short       nVertex;      // Numero de vertices del objeto.
  short       nFaces;       // Numero de caras del objeto.
  lptmaterial Material;     // Puntero al material que afecta al objeto.
  tbbox       Bbox;         // Bounding Box del objeto.
  tvertex     Centro;       // Buffer de vertices.
  unsigned int z;           // z para ordenar.
  lptvertex   Vertices;     // Buffer de vertices.
  lptface     Faces;        // Buffer de caras.
  lptmatrix   matrix;       // Matriz de transformaci¢n.
  char        name[20];     // Nombre del objeto
  int         Shadow;       // Indica si se trata como sombra
  float       lambiente;    // Luz ambiente del objeto
  int         activo;
}tobject,*lptobject;

typedef struct _vector{
    float x,y,z;
}tvector,*lptvector;

typedef struct _texturebuffer{
    int         users;
    char        name[13];
    Resource    *res;
    lpttextura  pointer;
}texturebuffer,*lptexturebuffer;

#endif

