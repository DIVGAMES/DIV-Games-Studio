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
  int active;
  int x,y;
  int number;
} tflag, *lptflag;

typedef struct type_point {
  int active;
  int x,y;
  int links;
} tpoint, *lptpoint;

typedef struct type_wall {
  int active;
  int type;
  int p1,p2;
  int front_region,back_region;
  int texture;
  int texture_top;
  int texture_bot;
  int fade;       // Valor de la luz de 0 a 16
} twall, *lptwall;

typedef struct type_region {
  int active;
  int type;
  int floor_height,ceil_height;
  int floor_tex;
  int ceil_tex;
  int fade;       // Valor de la luz de 0 a 16
} tregion, *lptregion;

typedef struct type_map {
  int num_points;
  tpoint *points[MAX_POINTS];
  int num_walls;
  twall *walls[MAX_WALLS];
  int num_regions;
  tregion *regions[MAX_REGIONS];
  int num_flags;
  tflag *flags[MAX_FLAGS];
  int bbox_x_ini,bbox_y_ini;
  int bbox_x_fin,bbox_y_fin;
  int fondo;
} tmap, *lptmap;

void *lf_malloc(int size);
void lf_free(void *puntero);
void lf_free_all();

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Definiciones del formato de las VPE
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

typedef short int     SHORT;
typedef unsigned long DWORD;
typedef long int      LONG;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Definicion de estructuras
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct ZF_Header {  // Zone file header
  char  IDStr[4];   // Zone file ID string
  SHORT NumPoints;  // Number of points
  SHORT NumRegions; // Number of floors
  SHORT NumWalls;   // Number of walls
  SHORT NumFlags;   // Number of flags
};

struct ZF_Move {  // Used for movement data
  SHORT x,y,z,t;  // x,y,z and torque
};

struct ZF_General {     // General information
  char  Title[24];      // Zone name
  char  Palette[9];     // Palette filename
  int   ScrTex;         // Name of screen texture
  int   BackTex;        // Background texture name
  char  BackEff[9];     // Background Eff program
  SHORT BackAngle;      // Angle of view covered by BackTex
  SHORT ActView;        // Index of view which gets kbd input
  struct ZF_Move Force; // Global force
};

struct ZF_Flag {    // Flag struture
  LONG  x,y;        // Point coordinates
  int   number;     // Numero asociado
};

struct ZF_Point {   // Point struture
  DWORD Type;       // Type
  LONG  x,y;        // Point coordinates
  SHORT path;       // Path index
  SHORT link;       // Index of the next point in the link
};

struct ZF_Region {  // Region structure
  DWORD Type;       // Type
  SHORT FloorH;     // Floor height
  SHORT CeilH;      // Ceiling height
  SHORT Below;      // Region below
  SHORT Above;      // Region above
  int   FloorTex;   // Floor texture name
  int   CeilTex;    // Ceiling texture name
  char  Eff[9];     // Eff program
  SHORT Fade;       // Effect's param
  SHORT Tag;        // Tag ID
};

struct ZF_Wall {    // Wall structure
  DWORD Type;       // Wall type
  SHORT p1, p2;     // Point indeces
  SHORT Front;      // Front Region
  SHORT Back;       // Back Region
  int   TopTex;     // Top texture name
  int   MidTex;     // Middle texture name
  int   BotTex;     // Bottom texture name
  char  Eff[9];     // Eff program
  SHORT Fade;       // Effect's param
  SHORT TexX, TexY; // Texture adjusting
  SHORT Mass;       // Mass for movable walls
  SHORT Tag;        // Tag ID
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
