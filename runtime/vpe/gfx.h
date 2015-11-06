/****************************************************************************
                           VPE GRAPHICS SYSTEM
*****************************************************************************/

// These functions can be altered, renamed or whatever
void InitGraph(char *buffer,int ancho,int alto); // Init gfx output device
void ShutGraph(void);		// Shut down gfx
int  GetGraphWidth(void);	// Returns width of output device
int  GetGraphHeight(void);	// Returns height of output device

// The following functions have to be provided
void SetPalette(BYTE *);	// Set palette (all 256 entries)
void DrawBuffer(View *);	// Blitting function
void DrawScreen(void);		// Filling function
