/****************************************************************************
                         VIRTUAL PRESENCE ENGINE
*****************************************************************************/

#ifndef _VPE_H_
#define _VPE_H_

#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "hard.h"
#include "zone.h"
#include "tex.h"
#include "pal.h"

//=================
// General defines
//=================
#define VPE_VERSION "0.67"      // Current version
#define MATH_FILE "MATH.DAT"    // Math table filename
#define NULL_RES  "NO_NAME"   // Null resource
#define ZONE_EXT  ".ZON"      // Def zone extension
#define TEX_EXT   ".TEX"      // Def tex extension
#define PIC_EXT   ".PIC"      // Def picture extension
#define PAL_EXT   ".PAL"      // Def pal extension
#define PCX_EXT   ".PCX"      // Def pal extension


//==================
// Misc VPE defines
//==================
#define RES_NUM   8192      // Maximum number of res.
#define RES_INC     32      // Num. of res. for mem alloc


//===========================
// Events for object control
//===========================
#define EV_ACT1   0x00000001    // Action #1
#define EV_ACT2   0x00000002    // Action #2
#define EV_ACT3   0x00000004    // Action #3
#define EV_ACT4   0x00000008    // Action #4
#define EV_FORWARD  0x00000010    // Move forward
#define EV_BACKWARD 0x00000020    // Move backward
#define EV_LEFT   0x00000040    // Strafe left
#define EV_RIGHT  0x00000080    // Strafe right
#define EV_UP   0x00000100    // Jump (or move up)
#define EV_DOWN   0x00000200    // Move down
#define EV_TURNLEFT 0x00000400    // Turn left
#define EV_TURNRIGHT  0x00000800    // Turn right
#define EV_DESTROY  0x10000000    // Object is to be deleted

//===============================
// Types common to all resources
//===============================
#define T_TAGGED  0x01000000    // Tag has been activated
#define T_SEEN    0x02000000    // Has been seen
#define T_ABSLIGHT  0x04000000    // Fade shows abs. light (type)

//============
// Pick types
//============
#define PICK_NULL 0     // Nothing
#define PICK_TOPWALL  1     // Top part of wall
#define PICK_MIDWALL  2     // Mid part of wall
#define PICK_BOTWALL  3     // Bot part of wall
#define PICK_OBJECT 4     // Object

//========
// Errors
//========
#define ER_GENERAL  -1      // Just an error
#define ER_MEMORY -2      // Not enough memory
#define ER_CACHE0 -3      // Zero cache alloc
#define ER_FOPEN  -4      // Can't open file
#define ER_FORMAT -5      // Bad format
#define ER_FIND   -6      // Couldn't find something
#define ER_RES    -7      // Out of resources


//======================
// Engine configuration
//======================
struct EngineCFG {    // Engine configuration
  WORD  ScrWidth;     // _Screen_ width
  WORD  ScrHeight;    // _Screen_ height
  FIXED ScrConst;     // Constant (depends on aspect ratio)
  FIXED TimeRate;     // Timer rate (ticks per second)
  LONG  Ticks;      // Ticks during last frame
  FIXED FPS;      // Drawing speed in frames per sec
  FIXED IFPS;     // 1/fps
  FIXED TimeUnit;     // How long in (1/100 sec) frame takes
  FIXED MaxStairs;    // Max height of a step in stairs
  SHORT PickX,PickY;    // Scr coords to check for pick
  WORD  PickType;     // Type of picked thing
  void *PickPtr;      // Ptr of the picked thing
  FIXED PickDist;     // Distance to picked thing
};

//================
// VPE structures
//================
struct Table {      // Table structure
  int   Number;     // Number of entries
  int   Allocated;    // Allocated entries
  int   Size;     // Size of an element
  void *ptr[RES_NUM];   // Pointers to data entries
};

//=================
// Zone structures
//=================

struct Move {     // Physics structure
  FIXED X, Y, Z, T;
};

struct Point {      // Point structure
  DWORD Type;     // Type
  FIXED x,y;      // Current coords
  FIXED nx,ny;      // Transformed coords
  FIXED ld,rd,a;      // Renderer data
  SHORT Coord;      // Current coord (in PATH)
  FIXED Count;      // Counter (for PATH coord change)
  struct Point *link;   // Ptr to the next link
  LONG  Stamp;      // Time-stamp of last transform
};

struct Region {     // Region structure
  DWORD Type;     // Type
  FIXED FloorH, CeilH;    // Height of floor and ceiling
  struct TexCon   FloorTC, CeilTC;// Textures
  struct Region  *Below, *Above;  // Region(s) above/below
  SHORT Fade;     // Light control
  SHORT Tag;      // Tag control
  struct Move     Resist;   // Local physics
  struct Object  *First;    // First object in the Region list
  struct Wall    **WallPtrs;  // Ptr to Wall pointers array
  LONG  NumWallPtrs;    // Number of wall pointers
  LONG  Stamp;      // Time-stamp of last render
  SHORT Idx;      // Region's index (for REJECT)
};
struct Wall {     // Wall structure
  DWORD  Type;      // Type
  struct Point   *p1, *p2;  // Points
  struct TexCon   TopTC;    // Top texture
  struct TexCon   MidTC;    // Mid texture
  struct TexCon   BotTC;    // Bottom texture
  struct Region  *Front, *Back; // Front and Back regions
  FIXED  Length, Mass;    // Length and (optional) mass
  FIXED  TexX, TexY;    // Texture adjust
  SHORT  Fade;      // Light control
  SHORT  Tag;     // Tag control
  LONG Stamp;     // Time-stamp of last render
};
struct Object {     // Object structure
  DWORD  Type;      // Object's type
  struct Point   *pp;   // Coords
  DWORD  Event;     // Event to/from engine
  SHORT  Angle;     // Direction
  FIXED  H,RH;      // World Z coord, Render Z Coord
  FIXED  Height, Radius, Step;    // Obj dimensions
  FIXED  Mass;      // Obj mass
  SHORT  Fade;      // Light control
  SHORT  wall_number;   // Para el control de colisiones
  SHORT  region_number; // Para el control de colisiones
  SHORT  nextregion_number; // Para el control de colisiones
  struct TexCon     TC;   // Texture
  struct Move       MSpeed; // Max speed
  struct Move       Acc;    // Obj acceleration
  struct Move       Speed;  // Current speed
  struct Region    *pRegion;  // Region where obj is
  struct Object    *Next;   // Next object in the Region's list
};
struct View {     // View structure
  SHORT  ScrX, ScrY;    // Top-left corner of window
  SHORT  Width, Height;   // Width and height of the window
  BYTE  *Buffer;      // Buffer to draw to
  BYTE **BufScan;     // Table of ptrs to scanlines
  DWORD  BufWidth;    // Width of the output buffer
  DWORD  Size;      // Size of buffer (to check realloc)
  struct Object *pObject;   // Object which carries the camera
  FIXED  ObjHeight;   // Height above object
  FIXED  H;           // Z coord of camera
  FIXED  FIni;        // Visibility
  FIXED  FLen;        // Visibility
  SHORT  Table;       // Fading table index
  SHORT  HAngle;      // Direction relative to obj dir
  SHORT  VAngle;      // Ver. angle (-127 to 128)
  SHORT  Horizon;     // For tilting view
  struct VLine  *StartClip; // Start clip array ptr
  FIXED  dBackX,dBackY;   // Background texel size
  FIXED  BackH;     // View height for the texel size
  FIXED  dPicX,dPicY;   // Overlay texel size
  FIXED  ConstHDist, ConstVDist;  // Constants
  FIXED  ConstHSlope;   // One more constant
  FIXED  tc1,ts1,tc2,ts2;   // Variables for floor stuff
};
struct General {    // General info
  char Title[24];     // Zone title
  struct Move GlobalForce;  // Global force
  struct TexCon ScrTC;    // Screen texture
  struct TexCon BackTC;   // Back texture
  SHORT  BackAngle;   // Angle, back covers
  FIXED  BackConst;   // Background constant
};

extern struct EngineCFG Engine;
extern struct Palette   Pal;
extern struct General   Gen;

extern struct Table     Pics;
extern struct Table     Points;
extern struct Table     Regions;
extern struct Table     Walls;
extern struct Table     Objects;
extern struct Table     Views;
extern struct View     *ActView;

extern LONG TotalFrames;
extern int VPE_fog;

//=============================
// Virtual Presence Engine API
//=============================
void  VPE_Init(void);     // Init VPE
void  VPE_Config(int,int);    // Configure VPE (scr size)
void  VPE_Start(void);      // Start rendering process
void  VPE_Stop(void);     // Stop rendering process
void  VPE_Shut(void);     // Shut down the engine
void  VPE_SetRate(FIXED);   // Set Engine speed rate
void  VPE_Update(void);     // Prepare zone for next frame
void  VPE_Render(void);     // Render screen

//================
// Zone functions
//================
void  LoadZone(char *);     // Load a zone

//===================
// Texture functions
//===================
void  TexAlloc(struct TexCon *,int, int num_fpg); // Allocate/load a texture
void  TexAlloc2(struct TexCon *,int, int num_fpg); // Allocate/load a texture

//=========================
// VPE resource management
//=========================
struct Object  *AddObject(void);    // Add a new object
int             DelObject(struct Object *); // Delete an object
struct Point   *AddPoint(void);     // Add a new point
struct Point   *DelPoint(struct Point *); // Delete a point
struct Region  *FindRegion(FIXED,FIXED,FIXED,int);// Find region from coords
void            SetObjRegion(struct Object *,struct Region *);// Set r. for o.
void            ClearObjRegion(struct Object *);// Clear reg. from an obj.
void SetViewSize(struct View *,int,int,BYTE *,int);// Set view size
void SetViewDir(struct View *,int,int);   // Set view direction
void SetActiveView(int altura,int objeto,int w,int h,BYTE *buf_ptr,int buf_width);

//================
// Math functions
//================
FIXED FixSin(SHORT);      // SIN of an angle given in units
FIXED FixCos(SHORT);      // COS
FIXED FixITan(FIXED,FIXED);   // Angle in units
FIXED FixDist(FIXED,FIXED,FIXED,FIXED); // Fast approx for distance
LONG  LongSqrt(LONG);     // Pretty fast sqrt

//=================
// Misc. functions
//=================
void *CacheAlloc(LONG);               // Alloc. mem. freeing some cache
void *MemRealloc(int *, int *, int);  // Alloc. mem. freeing some cache
void *AddEntry(struct Table *);       // Add entry to a table
void *DelEntry(struct Table *, int);  // Del entry from a table by idx
FIXED GetSPF(FIXED);                  // Get speed per frame
void DrawOverlay(struct TexCon *,int,int,int); // Draw overlay on screen
void SetPickWatch(int,int);          // Set pick watch for scr coords
WORD GetPickWatch(void *,FIXED *);   // Get result of pick watching

//==================
//  Funciones de movimiento
//==================

void _object_avance(int num_object,int angulo,int velocidad);
void _vpe_inicio(char *fichero,char *buffer,int ancho,int alto);
void _vpe_fin();
void InitGraph(char *buffer,int ancho,int alto);

#define  max( a, b ) ( ( a > b) ? a : b ) 

#define  min( a, b ) ( ( a < b) ? a : b ) 


#endif
