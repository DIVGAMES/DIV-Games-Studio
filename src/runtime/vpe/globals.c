#include "internal.h"

struct EngineCFG Engine;
struct Palette Pal;
struct General Gen;

struct Table Pics;
struct Table Points;
struct Table Regions;
struct Table Walls;
struct Table Objects;
struct Table Views;
struct Wall **WallPtrs;
struct View *ActView;
struct View *CurView;
struct VDraw *VDraws;
struct WLine *MLines;
struct FLine *FLines;
struct VLine *VLines;
struct Level Levels[MAX_LEVELS];
struct Level *CurLevel;
struct VDraw LeftVDraw, RightVDraw;
VPEFixed *SinTable, *CosTable, *ITanTable, *FPSTable;

VPEFixed ViewX, ViewY, ViewCos, ViewSin;
VPELong CurStamp, TotalFrames;
int NumWallPtrs, NumRTE, NumVDraws, NumMLines, NumLevels;
int PickVX, PickVY, PickVFlag;
