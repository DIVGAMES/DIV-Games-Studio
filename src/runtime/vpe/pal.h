/*****************************************************************************
 *                                                                           *
 *                           PALETTE FILE STUFF                              *
 *                                                                           *
 *                   ------------------------------------                    *
 *                  |      Virtual Presence Engine       |                   *
 *****************************************************************************/



#define PAL_ID		"PL3"		// Palette file ID
#define MAX_TABLES	32		// Max number of tables


struct PalHeader {		// Header of a palette file
	VPEByte   IDStr[4];		// ID string == PL1
	VPEWord   NumTables;		// Number of tables
	VPEWord   NumShades;		// Number of shades in tables
};

struct Palette {		// Main Palette structure
	struct PalHeader  PH;		// File header
	VPEByte   VGAPal[768];		// VGA palette
	VPEByte   BackFade[MAX_TABLES];	// Background shades for each table
	VPEByte  *Tables[MAX_TABLES];	// Shade tables' offsets
	VPEByte  *Trans;			// Translucency table offset
	VPEByte  *MemTables;		// Tables raw ptr
};



/*************************************************************
			Palette file format
--------------------------------------------------------------

	Header		- PalHeader
	Raw palette	- VPEByte * 256 * 3
	Background	- VPEByte * 32
	Shade tables	- VPEByte * 256 * NumShades * NumTables
	Trans table     - VPEByte * 256 * 256

**************************************************************/
