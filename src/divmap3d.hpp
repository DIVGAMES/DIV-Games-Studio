#ifndef __DIVMAP3d_HPP_
#define __DIVMAP3d_HPP_

#define MAX_POINTS  8192
#define MAX_WALLS   8192
#define MAX_REGIONS 4096
#define MAX_FLAGS   1000

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Definicion de tipos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

typedef struct type_flag {
  int32_t active;
  int32_t x,y;
  int32_t number;
} tflag, *lptflag;

typedef struct type_point {
  int32_t active;
  int32_t x,y;
  int32_t links;
} tpoint, *lptpoint;

typedef struct type_wall {
  int32_t active;
  int32_t type;
  int32_t p1,p2;
  int32_t front_region,back_region;
  int32_t texture;
  int32_t texture_top;
  int32_t texture_bot;
  int32_t fade;       // Valor de la luz de 0 a 16
} twall, *lptwall;

typedef struct type_region {
  int32_t active;
  int32_t type;
  int32_t floor_height,ceil_height;
  int32_t floor_tex;
  int32_t ceil_tex;
  int32_t fade;       // Valor de la luz de 0 a 16
} tregion, *lptregion;

typedef struct type_map {
  int32_t num_points;
  tpoint *points[MAX_POINTS];
  int32_t num_walls;
  twall *walls[MAX_WALLS];
  int32_t num_regions;
  tregion *regions[MAX_REGIONS];
  int32_t num_flags;
  tflag *flags[MAX_FLAGS];
  int32_t bbox_x_ini,bbox_y_ini;
  int32_t bbox_x_fin,bbox_y_fin;
  int32_t fondo;
} tmap, *lptmap;

void *lf_malloc(int32_t size);
void lf_free(void *puntero);
void lf_free_all();

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Definiciones del formato de las VPE
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

typedef int16_t     M8SHORT;
//#ifndef WIN32
typedef int32_t M8DWORD;
//#endif
typedef int32_t      M8LONG;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Definicion de estructuras
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct ZF_Header {  // Zone file header
  char  IDStr[4];   // Zone file ID string
  M8SHORT NumPoints;  // Number of points
  M8SHORT NumRegions; // Number of floors
  M8SHORT NumWalls;   // Number of walls
  M8SHORT NumFlags;   // Number of flags
};

struct ZF_Move {  // Used for movement data
  M8SHORT x,y,z,t;  // x,y,z and torque
};

struct ZF_General {     // General information
  char  Title[24];      // Zone name
  char  Palette[9];     // Palette filename
  int32_t   ScrTex;         // Name of screen texture
  int32_t   BackTex;        // Background texture name
  char  BackEff[9];     // Background Eff program
  M8SHORT BackAngle;      // Angle of view covered by BackTex
  M8SHORT ActView;        // Index of view which gets kbd input
  struct ZF_Move Force; // Global force
};

struct ZF_Flag {    // Flag struture
  M8LONG  x,y;        // Point coordinates
  int32_t   number;     // Numero asociado
};

struct ZF_Point {   // Point struture
  M8DWORD Type;       // Type
  M8LONG  x,y;        // Point coordinates
  M8SHORT path;       // Path index
  M8SHORT link;       // Index of the next point in the link
};

struct ZF_Region {  // Region structure
  M8DWORD Type;       // Type
  M8SHORT FloorH;     // Floor height
  M8SHORT CeilH;      // Ceiling height
  M8SHORT Below;      // Region below
  M8SHORT Above;      // Region above
  int32_t   FloorTex;   // Floor texture name
  int32_t   CeilTex;    // Ceiling texture name
  char  Eff[9];     // Eff program
  M8SHORT Fade;       // Effect's param
  M8SHORT Tag;        // Tag ID
};

struct ZF_Wall {    // Wall structure
  M8DWORD Type;       // Wall type
  M8SHORT p1, p2;     // Point indeces
  M8SHORT Front;      // Front Region
  M8SHORT Back;       // Back Region
  int32_t   TopTex;     // Top texture name
  int32_t   MidTex;     // Middle texture name
  int32_t   BotTex;     // Bottom texture name
  char  Eff[9];     // Eff program
  M8SHORT Fade;       // Effect's param
  M8SHORT TexX, TexY; // Texture adjusting
  M8SHORT Mass;       // Mass for movable walls
  M8SHORT Tag;        // Tag ID
};

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Formato
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

//  Header        - ZF_Header
//  Points data   - ZF_Point     * NumPoints
//  Regions data  - ZF_Region    * NumRegions
//  Walls data    - ZF_Wall      * NumWalls
//  General info  - ZF_General

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Declaracion de funciones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void map_saveedit(FILE *fichero, lptmap map_aux);
void map_readedit(FILE *fichero, lptmap map_aux);

#endif
