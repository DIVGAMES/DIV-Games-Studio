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
	BYTE   IDStr[4];		// ID string == PL1
	WORD   NumTables;		// Number of tables
	WORD   NumShades;		// Number of shades in tables
};

struct Palette {		// Main Palette structure
	struct PalHeader  PH;		// File header
	BYTE   VGAPal[768];		// VGA palette
	BYTE   BackFade[MAX_TABLES];	// Background shades for each table
	BYTE  *Tables[MAX_TABLES];	// Shade tables' offsets
	BYTE  *Trans;			// Translucency table offset
	BYTE  *MemTables;		// Tables raw ptr
};



/*************************************************************
			Palette file format
--------------------------------------------------------------

	Header		- PalHeader
	Raw palette	- BYTE * 256 * 3
	Background	- BYTE * 32
	Shade tables	- BYTE * 256 * NumShades * NumTables
	Trans table     - BYTE * 256 * 256

**************************************************************/
