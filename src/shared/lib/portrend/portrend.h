#ifndef PORTREND_H_
#define PORTREND_H_

/* Define window size */
#include "internal.h"

#define W ancho
#define H alto


#define FALSE           0
#define TRUE            1

#define DPMI_INT	0x31

//==========================
// General type definitions
//==========================
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long int        LONG;
typedef short int       SHORT;
typedef long int        FIXED;


//==================
// Math definitions
//==================
#define DEG360		2048		// equals to 360 degrees
#define DEG360_2	11		// DEG360 log2
#define DEG180		1024		// 180 degrees
#define DEG90		512		// 90 degrees
#define ITAN_LEN	1024		// -1 to 1 (1024 steps)
#define F_DEG360	(2048<<16)	// 360 deg FIXED
#define F_DEG180	(1024<<16)	// 180 deg FIXED
#define F_DEG90		(512<<16)	// 90  deg FIXED
#define F_HITAN		(512<<16)	// Half of the ITAN_LEN (FIXED)
#define ITAN_MUL	9		// log2(ITAN_LEN>>1)


//===============
// Useful macros
//===============
#define FIX_INT(x)  (SHORT)((x)>>16)
#define INT_FIX(x)  ((FIXED)(x)<<16)
#define DOUBLE_FIX(x)   ((FIXED)((double)(x)*65536.0))
#define FIX_DOUBLE(x)   ((double)(x)/65536.0)
#define GET_LOW(px)	(WORD)(*px)
#define GET_HI(px)	(*((SHORT *)(px)+1))
#define SET_HI(px,a)    (*((SHORT *)(px)+1)=a
#define SET_LOW(px,a)	(*((WORD *)(px))=a)
#define FixMin(a,b)	((a)<(b)?(a):(b))
#define FixMax(a,b)	((a)>(b)?(a):(b))
#define FixMod(a)	((a)>=0?(a):(-(a)))
#define mod(a)		((a)>0?(a):(-(a)))

typedef short int     SHORT;
typedef unsigned long DWORD;
typedef long int      LONG;

typedef struct _vertxy {
	float x;
	float y;
} vertxy;

typedef struct _vertxyz {
	float x;
	float y;
	float z;
} vertxyz;

typedef struct _sector {
    float floor, ceil;
    vertxy *vertex; // Each vertex has an x and y coordinate
    signed char *neighbors;           // Each edge may have a corresponding neighboring sector
    unsigned npoints;                 // How many vertexes there are
} sector;


/* Player: location */
typedef struct _m8camera
{
    vertxyz where,      // Current position
           velocity;   // Current motion vector
    float angle, anglesin, anglecos, yaw;   // Looking towards (and sin() and cos() thereof)
    unsigned sector;                        // Which sector the player is currently in
} m8camera;




/* Define various vision related constants */
#define EyeHeight  6    // Camera height from floor when standing
#define DuckHeight 2.5  // And when crouching
#define HeadMargin 1    // How much room there is above camera before the head hits the ceiling
#define KneeHeight 2    // How tall obstacles the player can simply walk over without jumping
#define hfov (0.73f*H)  // Affects the horizontal field of vision
#define vfov (.2f*H)    // Affects the vertical field of vision

#define MAX_POINTS  8192
#define MAX_WALLS   8192
#define MAX_REGIONS 4096
#define MAX_FLAGS   1000

#define min(a,b)             (((a) < (b)) ? (a) : (b)) // min: Choose smaller of two scalars.
#define max(a,b)             (((a) > (b)) ? (a) : (b)) // max: Choose greater of two scalars.
#define clamp(a, mi,ma)      min(max(a,mi),ma)         // clamp: Clamp value into set range.
#define vxs(x0,y0, x1,y1)    ((x0)*(y1) - (x1)*(y0))   // vxs: Vector cross product
// Overlap:  Determine whether the two number ranges overlap.
#define Overlap(a0,a1,b0,b1) (min(a0,a1) <= max(b0,b1) && min(b0,b1) <= max(a0,a1))
// IntersectBox: Determine whether two 2D-boxes intersect.
#define IntersectBox(x0,y0, x1,y1, x2,y2, x3,y3) (Overlap(x0,x1,x2,x3) && Overlap(y0,y1,y2,y3))
// PointSide: Determine which side of a line the point is on. Return value: <0, =0 or >0.
#define PointSide(px,py, x0,y0, x1,y1) vxs((x1)-(x0), (y1)-(y0), (px)-(x0), (py)-(y0))
// Intersect: Calculate the point of intersection between two lines.
#define Intersect(x1,y1, x2,y2, x3,y3, x4,y4) ((vertxy) { \
    vxs(vxs(x1,y1, x2,y2), (x1)-(x2), vxs(x3,y3, x4,y4), (x3)-(x4)) / vxs((x1)-(x2), (y1)-(y2), (x3)-(x4), (y3)-(y4)), \
    vxs(vxs(x1,y1, x2,y2), (y1)-(y2), vxs(x3,y3, x4,y4), (y3)-(y4)) / vxs((x1)-(x2), (y1)-(y2), (x3)-(x4), (y3)-(y4)) })



//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//  Definicion de tipos
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

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


typedef struct {
  byte  m3d_path[256];
  byte  m3d_name[16];
  int32_t   numero;
  byte  fpg_path[256];
  byte  fpg_name[16];
  tmap  map;
} M3D_info;

void map_read(M3D_info *m3d_aux, char *full);

void map2port(struct ZF_Header     *Header);

void DrawScreen();
void PortRender(int i);


#endif
