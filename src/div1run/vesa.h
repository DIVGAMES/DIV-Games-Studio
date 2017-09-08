//═════════════════════════════════════════════════════════════════════════════
//      Subrutinas de VESA
//═════════════════════════════════════════════════════════════════════════════
#define TRUE  1
#define FALSE 0

// Estructuras usadas por el driver VESA
typedef struct RealPointer {
	unsigned short Segment;  //The real mode segment (offset is 0).
	unsigned short Selector; //In protected mode, you need to chuck this dude into a segment register and use an offset 0.
} RealPointer;

typedef struct VesaInfo {
	char           Signature[4];
	unsigned short Version;
	unsigned short OEMNameOffset;
	unsigned short OEMNameSegment;        //Pointer to OEM name?
	unsigned char  Capabilities[4];
	unsigned short SupportedModesOffset;
	unsigned short SupportedModesSegment; //Pointer to list of supported VESA and OEM modes (terminated with 0xffff).
	unsigned char  Reserved[238];
} VesaInfo;

typedef struct VesaModeData {
	unsigned short   ModeAttributes;
	unsigned char    WindowAAttributes;
	unsigned char    WindowBAttributes;
	unsigned short   WindowGranularity;
	unsigned short   WindowSize;
	unsigned short   StartSegmentWindowA;
	unsigned short   StartSegmentWindowB;
	void           (*WindowPositioningFunction)(signed long page);
	unsigned short   BytesPerScanLine;

	//Remainder of this structure is optional for VESA modes in v1.0/1.1, needed for OEM modes.
	unsigned short   PixelWidth;
	unsigned short   PixelHeight;
	unsigned char    CharacterCellPixelWidth;
	unsigned char    CharacterCellPixelHeight;
	unsigned char    NumberOfMemoryPlanes;
	unsigned char    BitsPerPixel;
	unsigned char    NumberOfBanks;
	unsigned char    MemoryModelType;
	unsigned char    SizeOfBank;
	unsigned char    NumberOfImagePages;
	unsigned char    Reserved1;

	//VBE v1.2+
	unsigned char    RedMaskSize;
	unsigned char    RedFieldPosition;
	unsigned char    GreenMaskSize;
	unsigned char    GreenFieldPosition;
	unsigned char    BlueMaskSize;
	unsigned char    BlueFieldPosition;
	unsigned char    ReservedMaskSize;
	unsigned char    ReservedFieldPosition;
	unsigned char    DirectColourModeInfo;
	unsigned char    Reserved2[216];
} VesaModeData;

typedef struct RMREGS {
	unsigned long  edi;
	unsigned long  esi;
	unsigned long  ebp;
	unsigned long  reserved;
	unsigned long  ebx;
	unsigned long  edx;
	unsigned long  ecx;
	unsigned long  eax;
	unsigned short flags;
	unsigned short es, ds, fs, gs, ip, cs, sp, ss;
} RMREGS;

void EndVesaMode();
int  SetVesaMode(unsigned short vmode);
void vesa_set_page(signed long vpage);
