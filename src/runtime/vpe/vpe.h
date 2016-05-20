/****************************************************************************
                         VIRTUAL PRESENCE ENGINE
*****************************************************************************/

#ifndef _VPE_H_
#define _VPE_H_

#include <stdio.h>
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
  VPEWord  ScrWidth;     // _Screen_ width
  VPEWord  ScrHeight;    // _Screen_ height
  VPEFixed ScrConst;     // Constant (depends on aspect ratio)
  VPEFixed TimeRate;     // Timer rate (ticks per second)
  VPELong  Ticks;      // Ticks during last frame
  VPEFixed FPS;      // Drawing speed in frames per sec
  VPEFixed IFPS;     // 1/fps
  VPEFixed TimeUnit;     // How long in (1/100 sec) frame takes
  VPEFixed MaxStairs;    // Max height of a step in stairs
  VPEShort PickX,PickY;    // Scr coords to check for pick
  VPEWord  PickType;     // Type of picked thing
  void *PickPtr;      // Ptr of the picked thing
  VPEFixed PickDist;     // Distance to picked thing
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
  VPEFixed X, Y, Z, T;
};

struct Point {      // Point structure
  VPEDword Type;     // Type
  VPEFixed x,y;      // Current coords
  VPEFixed nx,ny;      // Transformed coords
  VPEFixed ld,rd,a;      // Renderer data
  VPEShort Coord;      // Current coord (in PATH)
  VPEFixed Count;      // Counter (for PATH coord change)
  struct Point *link;   // Ptr to the next link
  VPELong  Stamp;      // Time-stamp of last transform
};

struct Region {     // Region structure
  VPEDword Type;     // Type
  VPEFixed FloorH, CeilH;    // Height of floor and ceiling
  struct TexCon   FloorTC, CeilTC;// Textures
  struct Region  *Below, *Above;  // Region(s) above/below
  VPEShort Fade;     // Light control
  VPEShort Tag;      // Tag control
  struct Move     Resist;   // Local physics
  struct Object  *First;    // First object in the Region list
  struct Wall    **WallPtrs;  // Ptr to Wall pointers array
  VPELong  NumWallPtrs;    // Number of wall pointers
  VPELong  Stamp;      // Time-stamp of last render
  VPEShort Idx;      // Region's index (for REJECT)
};
struct Wall {     // Wall structure
  VPEDword  Type;      // Type
  struct Point   *p1, *p2;  // Points
  struct TexCon   TopTC;    // Top texture
  struct TexCon   MidTC;    // Mid texture
  struct TexCon   BotTC;    // Bottom texture
  struct Region  *Front, *Back; // Front and Back regions
  VPEFixed  Length, Mass;    // Length and (optional) mass
  VPEFixed  TexX, TexY;    // Texture adjust
  VPEShort  Fade;      // Light control
  VPEShort  Tag;     // Tag control
  VPELong Stamp;     // Time-stamp of last render
};
struct Object {     // Object structure
  VPEDword  Type;      // Object's type
  struct Point   *pp;   // Coords
  VPEDword  Event;     // Event to/from engine
  VPEShort  Angle;     // Direction
  VPEFixed  H,RH;      // World Z coord, Render Z Coord
  VPEFixed  Height, Radius, Step;    // Obj dimensions
  VPEFixed  Mass;      // Obj mass
  VPEShort  Fade;      // Light control
  VPEShort  wall_number;   // Para el control de colisiones
  VPEShort  region_number; // Para el control de colisiones
  VPEShort  nextregion_number; // Para el control de colisiones
  struct TexCon     TC;   // Texture
  struct Move       MSpeed; // Max speed
  struct Move       Acc;    // Obj acceleration
  struct Move       Speed;  // Current speed
  struct Region    *pRegion;  // Region where obj is
  struct Object    *Next;   // Next object in the Region's list
};
struct View {     // View structure
  VPEShort  ScrX, ScrY;    // Top-left corner of window
  VPEShort  Width, Height;   // Width and height of the window
  VPEByte  *Buffer;      // Buffer to draw to
  VPEByte **BufScan;     // Table of ptrs to scanlines
  VPEDword  BufWidth;    // Width of the output buffer
  VPEDword  Size;      // Size of buffer (to check realloc)
  struct Object *pObject;   // Object which carries the camera
  VPEFixed  ObjHeight;   // Height above object
  VPEFixed  H;           // Z coord of camera
  VPEFixed  FIni;        // Visibility
  VPEFixed  FLen;        // Visibility
  VPEShort  Table;       // Fading table index
  VPEShort  HAngle;      // Direction relative to obj dir
  VPEShort  VAngle;      // Ver. angle (-127 to 128)
  VPEShort  Horizon;     // For tilting view
  struct VLine  *StartClip; // Start clip array ptr
  VPEFixed  dBackX,dBackY;   // Background texel size
  VPEFixed  BackH;     // View height for the texel size
  VPEFixed  dPicX,dPicY;   // Overlay texel size
  VPEFixed  ConstHDist, ConstVDist;  // Constants
  VPEFixed  ConstHSlope;   // One more constant
  VPEFixed  tc1,ts1,tc2,ts2;   // Variables for floor stuff
};
struct General {    // General info
  char Title[24];     // Zone title
  struct Move GlobalForce;  // Global force
  struct TexCon ScrTC;    // Screen texture
  struct TexCon BackTC;   // Back texture
  VPEShort  BackAngle;   // Angle, back covers
  VPEFixed  BackConst;   // Background constant
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

extern VPELong TotalFrames;
extern int VPE_fog;

//=============================
// Virtual Presence Engine API
//=============================
void  VPE_Init(void);     // Init VPE
void  VPE_Config(int,int);    // Configure VPE (scr size)
void  VPE_Start(void);      // Start rendering process
void  VPE_Stop(void);     // Stop rendering process
void  VPE_Shut(void);     // Shut down the engine
void  VPE_SetRate(VPEFixed);   // Set Engine speed rate
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
struct Region  *FindRegion(VPEFixed,VPEFixed,VPEFixed,int);// Find region from coords
void            SetObjRegion(struct Object *,struct Region *);// Set r. for o.
void            ClearObjRegion(struct Object *);// Clear reg. from an obj.
void SetViewSize(struct View *,int,int,VPEByte *,int);// Set view size
void SetViewDir(struct View *,int,int);   // Set view direction
void SetActiveView(int altura,int objeto,int w,int h,VPEByte *buf_ptr,int buf_width);

//================
// Math functions
//================
VPEFixed FixSin(VPEShort);      // SIN of an angle given in units
VPEFixed FixCos(VPEShort);      // COS
VPEFixed FixITan(VPEFixed,VPEFixed);   // Angle in units
VPEFixed FixDist(VPEFixed,VPEFixed,VPEFixed,VPEFixed); // Fast approx for distance
VPELong  LongSqrt(VPELong);     // Pretty fast sqrt

//=================
// Misc. functions
//=================
void *CacheAlloc(VPELong);               // Alloc. mem. freeing some cache
void *MemRealloc(int *, int *, int);  // Alloc. mem. freeing some cache
void *AddEntry(struct Table *);       // Add entry to a table
void *DelEntry(struct Table *, int);  // Del entry from a table by idx
VPEFixed GetSPF(VPEFixed);                  // Get speed per frame
void DrawOverlay(struct TexCon *,int,int,int); // Draw overlay on screen
void SetPickWatch(int,int);          // Set pick watch for scr coords
VPEWord GetPickWatch(void *,VPEFixed *);   // Get result of pick watching

#define min(a,b)             (((a) < (b)) ? (a) : (b)) // min: Choose smaller of two scalars.

//==================
//  Funciones de movimiento
//==================

void _object_avance(int num_object,int angulo,int velocidad);
void _vpe_inicio(char *fichero,char *buffer,int ancho,int alto);
void _vpe_fin();
void InitGraph(char *buffer,int ancho,int alto);

#endif
