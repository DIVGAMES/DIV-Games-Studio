#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#include "vpe.h"

//===========
// Constants
//===========
/********************   60 degrees   **************************************/
//#define VIEW_ANGLE  341     // FOV (60 deg)
//#define F_FLOOR_CONST DOUBLE_FIX(1.1547)  // 1/Cos(30) (floor drawing)
//#define F_VIEW_CONST  DOUBLE_FIX(1.7321)  // 1/(Tan(30))

/********************   72 degrees   **************************************/
#define VIEW_ANGLE  410     // FOV (72 deg)
#define F_FLOOR_CONST DOUBLE_FIX(1.2361)  // 1/Cos(36) (floor drawing)
#define F_VIEW_CONST  DOUBLE_FIX(1.3764)  // 1/(Tan(36))

/********************   90 degrees   **************************************/
//#define VIEW_ANGLE  512     // FOV (90 deg)
//#define F_FLOOR_CONST DOUBLE_FIX(1.4142)  // 1/Cos(45) (floor drawing)
//#define F_VIEW_CONST  DOUBLE_FIX(1.0000)  // 1/(Tan(45))



#define F_MIN_DIST  INT_FIX(8)    // Default clipping distance 
#define MAX_VDRAWS  1000      // Number of VDraws to alloc
#define MAX_MLINES  320*6     // Number of MLines to alloc
#define MAX_LEVELS  64        // Number of levels
#define FPSTABLE_SIZE 1024      // Size of FPS table
#define FPSTABLE_NUM  20      // Number of FPS tables
#define TEX_EMPTY -1      // All PICs are empty
#define TEX_OLD   -2      // Leftover from prev zone


//=======================
// Attributes for VDraws
//=======================
#define VD_OBJECT 0x10000000    // VDraw is an object
#define VD_COMPLEX  0x20000000    // Complex wall
#define VD_WALLBACK 0x40000000    // Wall is turned


//========================
// Nice and useful macros
//========================

struct PicInfo *GetPic2(int piccode,int num_fpg);
void LoadPic2(struct PicInfo *pic);

#define TexCheck(ptc) {if ((ptc)->pPic) { \
      LoadPic((ptc)->pPic);\
      }}

#define TexCheck2(ptc)  {if ((ptc)->pPic) { \
      LoadPic2((ptc)->pPic);\
      }}


//=======================
// Internal data structs
//=======================
struct VLine {
  VPEShort Top, Bot;
};
struct FLine {
  VPEByte  *RawPtr, *PalPtr, *PixPtr;
  VPEDword  Coord, Delta;
  VPEShort  LeftCol, Count, Width2;
  struct Region *pRegion;
};
struct WLine {
  VPEByte  *RawPtr, *PalPtr, *PixPtr;
  VPEFixed  Coord, Delta;
  VPEShort  BufWidth, Count, Mask;
  VPEByte   IsTrans, IsTile;
};
struct VDraw {
  VPEDword  Type;
  void  *ptr;
  struct VDraw  *Prev, *Next;
  VPEShort LeftCol,RightCol;
  VPEFixed LD,RD,dLD,dRD;
  VPEFixed XStart,XLen;
  VPEFixed px1,px2;
};
struct Level {
  struct Region *Start;
  struct Region *From;
  struct VLine *Clip;
  int ClipCount;
  VPEFixed MinDist;
};
struct RTEffect {
  struct Effect *Eff;
  void *data;
};

extern struct Wall    **WallPtrs; // Wall ptr pool
extern struct RTEffect *RTE;    // Real-time effects (EFF_FRAME)
extern struct View     *CurView;  // Current view (during rendering)
extern struct VDraw    *VDraws;   // VDraws
extern struct WLine    *MLines;   // MLines (for masked/trans data)
extern struct FLine    *FLines;   // FLines (for floor buffering)
extern struct VLine    *VLines;   // VLines (for clipping)
extern struct Level     Levels[MAX_LEVELS]; // Levels (for 3d regions)
extern struct Level    *CurLevel; // Current level during rendering
extern struct VDraw     LeftVDraw,RightVDraw;
extern VPEFixed           *SinTable, *CosTable, *ITanTable, *FPSTable;
extern VPEByte            *Reject;

extern VPELong CurStamp;
extern VPEFixed ViewX,ViewY,ViewCos,ViewSin;
extern int NumWallPtrs, NumRTE, NumVDraws, NumMLines, NumLevels;
extern int PickVX, PickVY, PickVFlag;


void DrawView(struct View *);   // Draw view into buffer

void ScanLevel(struct Level *);   // Scan level's data
void ScanRegion(struct Region *); // Scan region data

void UpdateFloor(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);
void UpdateAboveHole(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);
void UpdateBelowHole(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);
void UpdateBack(VPEShort,VPEShort,VPEShort,struct VLine *,struct PicInfo *,struct Region *);

void DrawSimpleWall(struct VDraw *);  // Render simple wall
void DrawComplexWall(struct VDraw *); // Render cplx wall
void DrawObject(struct VDraw *);  // Render object
void DrawBackCol(int,int,int);    // Draw back vcol
void DrawMasked(void);      // Draw all masked stuff

void DrawFSpan(struct FLine *);   // Draw hor tex line
void DrawWSpan(struct WLine *);   // Draw ver tex line
void DrawMaskWSpan(struct WLine *); // Draw masked ver tex line (wall)
void DrawTransWSpan(struct WLine *);  // Draw translucent ver tex line (wall)
void DrawOSpan(struct WLine *);   // Draw ver tex line (obj)
void DrawMaskOSpan(struct WLine *); // Draw masked ver tex line (obj)
void DrawTransOSpan(struct WLine *);  // Draw translucent ver tex line (obj)


void InsertVDraw(struct VDraw *); // Insert VDraw to the list
struct VDraw *FindVDraw(void);    // Get VDraw to render
struct VDraw *RemoveVDraw(struct VDraw *);// Remove VDraw from list

void SetFPS(int);     // Sets FPS value (param - ticks)

void InitMemory(void);
void ShutMemory(void);

void LoadMath(void);
void LoadPalette(char *);
void InitReject(VPEByte *);

struct PicInfo *GetPic(int,int num_fpg);
void LoadPic(struct PicInfo *);

void UpdateZone(void);
void UpdateZoneViews(void);
void ClearZone(void);
void MoveWall(struct Wall *, VPEFixed, VPEFixed, VPEFixed, VPEFixed);
void MovePoint(struct Point *, VPEFixed, VPEFixed);

void InitEffects(void);
void SetupEffects(VPEDword);
void AddRTEffect(struct Effect *, void *);
void DelRTEffect(void *);

#endif
