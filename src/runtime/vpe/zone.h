/*****************************************************************************
 *                                                                           *
 *                             ZONE FILE STUFF                               *
 *                                                                           *
 *                   ------------------------------------                    *
 *                  |      Virtual Presence Engine       |                   *
 *****************************************************************************/


#define PATH_LEN  32    // Number of path steps for a point

#define P_XMOVE   0x00000001  // Free move along X coord
#define P_YMOVE   0x00000002  // Free move along Y coord
#define P_PMOVE   0x00000004  // Point's path on

#define R_MASK    0x00000001    // Masked region (ceil and/or floor)
#define R_RELTEX  0x00000002    // Texturing starts relative to wall
#define R_TRANS   0x00000008    // Trans. region (ceil and/or floor)
#define R_FLOORHOLE 0x00000010  // Fall thru floor
#define R_CEILHOLE  0x00000020  // Fly thru ceiling
#define R_LOCAL   0x00000100    // Use local physics for objects

#define W_MASK    0x00000001  // Masked wall (drawn from back)
#define W_SOLID   0x00000002  // Col. det. works
#define W_MOVABLE 0x00000004  // Wall is movable
#define W_TRANS   0x00000008  // Translucent wall (drawn from back)

#define O_THING   0x00000001  // No speed calcs
#define O_FLYER   0x00000002  // Gravity does not apply to the object
#define O_NOFRIC  0x00000004  // No friction. Move until collision
#define O_NOSLIDE 0x00000008  // Object does not slide along a wall
#define O_WALLCOL 0x00000010  // Check wall collisions
#define O_OBJCOL  0x00000020  // Check obj collisions
#define O_STRUCT  0x00000040  // Special 'structure' collisions
#define O_TRANS   0x00000100  // Translucent object
#define O_PROJECTILE  0x00001000  // Projectile object
#define O_INSTANT 0x00002000  // Object has infinite speed (bullet)
#define O_INFRONT 0x00010000  // Overlay object, slightly closer
#define O_BEHIND  0x00020000  // Overlay object, slightly farther


struct ZF_Header {    // Zone file header
  char  IDStr[4];     // Zone file ID string
  SHORT NumPoints;    // Number of points
  SHORT NumRegions;   // Number of floors
  SHORT NumWalls;     // Number of walls
  SHORT NumFlags;     // Number of flags
};

struct ZF_Move {    // Used for movement data
  SHORT x,y,z,t;      // x,y,z and torque
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


struct ZF_Flag {   // Point struture
  LONG  x,y;      // Point coordinates
  int   number;
};

struct ZF_Point {   // Point struture
  DWORD Type;     // Type
  LONG  x,y;      // Point coordinates
  SHORT path;     // Path index
  SHORT link;     // Index of the next point in the link
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
  SHORT Fade;       // Luminosidad de la region 0-65535
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
  SHORT Fade;       // Luminosidad de la pared 0-65535
  SHORT TexX, TexY; // Texture adjusting
  SHORT Mass;       // Mass for movable walls
  SHORT Tag;        // Tag ID
};


/************************************************************
      ZONE FILE FORMAT
-------------------------------------------------------------

  Header    - ZF_Header
  Points data - ZF_Point     * NumPoints
  Regions data  - ZF_Region    * NumRegions
  Walls data  - ZF_Wall      * NumWalls
  General info  - ZF_General

*************************************************************/
