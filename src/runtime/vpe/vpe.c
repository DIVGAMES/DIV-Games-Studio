#include "gfx.h"
#include "internal.h"

int VPE_Working = FALSE;
int VPE_fog = 0;

//═════════════════════════════════════════════════════════════════════════════
//  Inicializa la libreria
//═════════════════════════════════════════════════════════════════════════════

void _vpe_inicio(char *fichero, char *buffer, int ancho, int alto)
{
	InitGraph(buffer, ancho, alto); // Init gfx system
	VPE_Init();			// Init VPE
	VPE_Config(ancho, alto);	// Configure VPE

	LoadZone(fichero); // Load zone
	VPE_Start();       // Start the engine
}

//═════════════════════════════════════════════════════════════════════════════
//  Finaliza la libreria
//═════════════════════════════════════════════════════════════════════════════

void _vpe_fin()
{
	VPE_Stop();  // Stop the engine
	VPE_Shut();  // Shut down VPE
	ShutGraph(); // Shut gfx system
}

//═════════════════════════════════════════════════════════════════════════════
//═════════════════════════════════════════════════════════════════════════════

void VPE_Init(void)
{
	InitMemory(); // Init mem.manager and get some mem
	LoadMath();   // Load math
	LeftVDraw.LeftCol = LeftVDraw.RightCol = -10000;  // Init list
	RightVDraw.LeftCol = RightVDraw.RightCol = 10000; // Init list
	TotalFrames = 0;				  // Total frames drawn
	VPE_Working = FALSE;				  // Clear working flag
}

void VPE_Config(int Width, int Height)
{
	VPEFixed t1, t2;
	int i;

	Engine.ScrWidth = Width;		      // Set screen width
	Engine.ScrHeight = Height;		      // Set screen height
	t1 = FixDiv(INT_FIX(320), INT_FIX(240));      // Ratio for square pixels
	t2 = FixDiv(INT_FIX(Width), INT_FIX(Height)); // Our ratio
	Engine.ScrConst = FixDiv(t1, t2);	     // Make ScrConst

	//  if (FLines) MemFree(FLines);  // Free FLines if necessary
	FLines = (struct FLine *)CacheAlloc(Height * sizeof(struct FLine));
	if (!FLines)
		FatalError(ER_MEMORY, "FLines");
	//  if (VLines) MemFree(VLines);  // Free VLines if necessary
	VLines = (struct VLine *)CacheAlloc(MAX_LEVELS * Width *
					    sizeof(struct VLine));
	if (!VLines)
		FatalError(ER_MEMORY, "VLines");
	for (i = 0; i < MAX_LEVELS * Width; i++) // Init VLines
		VLines[i].Top = VLines[i].Bot = 0;
	for (i = 0; i < MAX_LEVELS; i++) // Init Clip in Levels
		Levels[i].Clip = VLines + Width * i;
	Engine.PickX = -1; // Init Pick X coord
	Engine.PickY = -1; // Init Pick Y coord
}

void VPE_Start(void)
{
	VPE_Working = TRUE; // VPE is working
}

void VPE_Update(void)
{
	//  VPELong last;

	if (!VPE_Working)
		return;
	// Update the zone
	UpdateZone();
}

void VPE_Render(void)
{
	int i;
	struct View *pView;

	UpdateZoneViews();

	// Prepare pick checking
	Engine.PickPtr = NULL;
	Engine.PickType = PICK_NULL;
	Engine.PickDist = 0;
	for (i = 0; i < Views.Number; i++) {
		// Create general stamp for this frame for current view
		CurStamp = (TotalFrames << 7) + (i << 4);
		pView = (struct View *)Views.ptr[i];
		DrawView(pView);
	}
	TotalFrames++;
}

void VPE_Stop(void)
{
	VPE_Working = FALSE; // VPE is not working
}

void VPE_Shut(void)
{
	ShutMemory(); // Shut down memory manager
}

void SetPickWatch(int x, int y)
{
	Engine.PickX = x;
	Engine.PickY = y;
}

VPEWord GetPickWatch(void *data, VPEFixed *dist)
{
	data = Engine.PickPtr;
	*dist = Engine.PickDist;
	return (Engine.PickType);
}
