#ifndef __STRUCT_HPP_
#define __STRUCT_HPP_

#include "resource.hpp"

/********************************************************/
/* Basic structures for the 3D engine                */
/********************************************************/

/* MATRIX ***********************************************/

typedef struct _matrix{
    float   _00,_01,_02,_03;
    float   _10,_11,_12,_13;
    float   _20,_21,_22,_23;
    float   _30,_31,_32,_33;
}tmatrix,*lptmatrix;

/* TEXTURE **********************************************/

typedef struct _texture{
  int           width,height; // Texture size.
  char          *buffer;    // Texture buffer.
// MipMapping
  char          *Levels[4]; // Pointer to the levels.
  int           Width[4];   // Pointer to the levels.
  int           Height[4];    // Pointer to the levels.
}ttexture,*lpttexture;

/* MATERIAL *********************************************/

#define T_FLAT        0
#define T_TEXTURE     1
#define T_GOURAUD     2
#define T_PERSPECTIVE 4
#define T_TRANSLUCENT 8
#define T_MASK     16
#define T_SHADOW      32
#define T_ANTIALIAS   64

// Mapping types
#define FLAT          0
#define GOURAUD       1
#define TRANSLUCENT   2
#define TEXTURE       3
#define GOURAUD_TEXT  4
#define TRANSPARENT  5
#define GHOST         6

#define TEXTURE_Z_CONST 7

#define FLAT_TRANS    8
#define TEXT_TRANS    9
#define TEXT_PER_TRANS 10
#define GOURAUD_PER   11
#define TRANSPARENT_MASK 12
#define TEXT_PER      13
#define GOUR_TEXT_PER 14
#define TRANS_PER     15
#define TEXT_BILINEAR 16
#define TEXT_GOUR     17
#define TEXT_ILLUM     18
#define SHADOW        19
#define GOUR_MASK     20
#define WIRE          21
#define ANTIALIAS     22

typedef struct _material{
  float       dr,dg,db;     // Color of the diffuse light affecting the object.
  char        dac_diffuse;   // Index into the dac of the diffuse color
  float       ar,ag,ab;     // Color of the ambient light affecting the object.
  char        dac_ambient;  // Index into the dac of the ambient color
  float       sr,sg,sb;     // Color of the specular light affecting the object.
  char        dac_specular;// Index into the dac of the specular color
  lpttexture  texture;      // Pointer to the texture buffer.
  int         map_type;     // Mapping type
}tmaterial,*lptmaterial;

/* VERTEX ***********************************************/

typedef struct _vertex{
  float       rx,ry,rz;     // Real coordinates.
  float       sx,sy,sz;     // Spatial coordinates.
  float       fpx,fpy,fpz;  // Screen coordinates in float.
  int         px,py,pz;     // Screen coordinates.

  float       rnx,rny,rnz;  // Real vertex normals.
  float       snx,sny,snz;  // Spatial vertex normals.
  float       r,g,b;        // Rgb values of the vertex.
  float       intensity;   // Intensity values.

  float       u,v;          // Texture coordinates.
  float       ru,rv;        // Texture coordinates.
  int         state;       // To avoid processing it several times
  int         sutherland_cohen; // For 3D clipping
  int         garbage[4];
}tvertex,*lptvertex;

/* CARA *************************************************/

typedef struct _face{
  lptvertex   v1;           // Pointer to vertex 1.
  lptvertex   v2;           // Pointer to vertex 2.
  lptvertex   v3;           // Pointer to vertex 3.
  float       rnx,rny,rnz;  // Real face normal.
  float       snx,sny,snz;  // Spatial face normal.
  float       co;
  lptmaterial material;     // Pointer to the material of this face.
  int         map_type;
//  int         bas[4];
}tface,*lptface;

/* BOUNDING BOX *****************************************/

typedef struct _bbox{
  float       x1,y1,z1;     // Top left corner.
  float       x2,y2,z2;     // Bottom right corner.
}tbbox,*lptbbox;

/* DUCT TAPE *****************************************/

typedef struct _tape{
  int         Obj1,Vert1;
  int         Obj2,Vert2;
  int         LinkMode;
}ttape,*lpttape;

/* OBJECT ***********************************************/

typedef struct _object{
  short       nVertex;      // Number of vertices of the object.
  short       nFaces;       // Number of faces of the object.
  lptmaterial Material;     // Pointer to the material affecting the object.
  tbbox       Bbox;         // Bounding Box of the object.
  tvertex     Center;       // Vertex buffer.
  unsigned int z;           // z for sorting.
  lptvertex   Vertices;     // Vertex buffer.
  lptface     Faces;        // Face buffer.
  lptmatrix   matrix;       // Transformation matrix.
  char        name[20];     // Object name
  int         Shadow;       // Tells whether it is treated as a shadow
  float       lambient;    // Ambient light of the object
  int         active;
}tobject,*lptobject;

typedef struct _vector{
    float x,y,z;
}tvector,*lptvector;

typedef struct _texturebuffer{
    int         users;
    char        name[13];
    Resource    *res;
    lpttexture  pointer;
}texturebuffer,*lptexturebuffer;

#endif

