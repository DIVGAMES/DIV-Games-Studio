/*****************************************************************************
 *                                                                           *
 *                           TEXTURE FILE STUFF                              *
 *                                                                           *
 *                   ------------------------------------                    *
 *                  |      Virtual Presence Engine       |                   *
 *****************************************************************************/

struct PicInfo {        // Information about a picture
  int    code;          // Codigo dentro del fpg
  VPEShort  Width, Height; // Dimensions of a pic
  VPEShort  InsX, InsY;    // Insertion point
  VPEShort  Used;          // Used counter
  VPEShort  Width2;        // log2 Width
  VPEByte  *Raw;           // Pointer to data
  int    fpg;
};

struct TexCon {                   // Texture control
  struct PicInfo *pPic;           // Pointer to current picture
  VPEByte   IsMirror;                // Mirror flag
  VPEByte   reserved_1[3];           // To keep the alignment
  struct PicInfo *pic_render[10]; // Pointer to current picture
  int    IsMirror_render[10];     // Mirror flag
};

