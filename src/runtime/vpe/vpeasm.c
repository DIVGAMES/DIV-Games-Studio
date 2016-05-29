#include <stdbool.h>
#include <stdint.h>

#include "internal.h"
#include "osdep.h"

extern memptrsize ScrBase;
extern int ScrWidth;
extern int ScrHeight;

/* ------------------------------ *** VPE.INC *** ------------------------------ */

//struct FLine {
//	VPEByte  *RawPtr, *PalPtr, *PixPtr;
//	VPEDword  Coord, Delta;
//	VPEShort  LeftCol, Count, Width2;
//	struct Region *pRegion;
//};
//struct WLine {
//	VPEByte  *RawPtr, *PalPtr, *PixPtr;
//	VPEFixed  Coord, Delta;
//	VPEShort  ViewWidth, Count, Mask;
//};

#define RawPtr_     0 // Tex ptr
#define PalPtr_     4 // Pal table ptr
#define PixPtr_     8 // Buffer ptr
#define Coord_     12 // Coord data
#define Delta_     16 // Delta
#define LeftCol_   20 // LeftCol (FLine)
#define ViewWidth_ 20 // View Width (WLine)
#define Count_     22 // Size
#define Width2_    24 // log2 Width (FLine)
#define WMask_     24 // Mask (WLine)


/* ------------------------------ *** FLOOP.INC   *** ------------------------------ */
/* ------------------------------ *** DRAW_FA.ASM *** ------------------------------ */

#define FLOOP_LEN  32
#define FLOOP_SIZE 19


int32_t FLoopOffSet[] = {
	0x0260,0x024D,0x023A,0x0227,0x0214,0x0201,0x01EE,0x01DB,
	0x01C8,0x01B5,0x01A2,0x018F,0x017C,0x0169,0x0156,0x0143,
	0x0130,0x011D,0x010A,0x00F7,0x00E4,0x00D1,0x00BE,0x00AB,
	0x0098,0x0085,0x0072,0x005F,0x004C,0x0039,0x0026,0x0013
};

int32_t LoopCount = 0;

//****************************************************************************
// Draws a horizontal floor span
//				EAX (EBP+8)
//	void DrawFSpan(struct HLine *pHLine)
//****************************************************************************
void DrawFSpan(struct FLine *pFLine) {
/*
	_asm {
		pushad
		mov ebx, dword ptr [pFLine]
		movzx eax, word ptr [ebx+Count_] ; EAX=pixels_to_draw
		mov dword ptr [LoopCount], eax		; LoopCount=EAX
		and eax, FLOOP_LEN-1		; EAX=pixels_to_draw%LOOP_LEN
		mov esi, offset FLoopOffSet	; Address of the offset table
		mov eax, dword ptr [esi+eax*4]		; EAX=relative offset of the cell
		add eax, offset FLoopStart	; EAX=absolute address of start cell
		push eax

		mov cl, byte ptr [ebx+Width2_]		; get log 2 of tex width
		movzx eax, word ptr [ebx+LeftCol_] ; put cur col to eax
		mov ebp, dword ptr [ebx+Delta_]		; get delta's
		mul ebp				; make cur coords
		mov edx, dword ptr [ebx+Coord_]		; get start coords
		add edx, eax;			; edx contains full coords
		mov esi, dword ptr [ebx+RawPtr_]		; Tex pointer
		mov edi, dword ptr [ebx+PixPtr_]		; Buffer pointer
		mov ebx, dword ptr [ebx+PalPtr_]		; Pal buffer pointer

		ret				; Jump to the start loop cell

	FLoopStart:				; Start of unrolled loop
		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		xor eax, eax
		shld eax, edx, cl
		shld ax, dx, cl
		mov bl, [esi+eax]
		mov bl, [ebx]
		mov [edi], bl
		inc edi
		add edx, ebp

		sub dword ptr [LoopCount], FLOOP_LEN
		jae FLoopStart

		popad
	}
*/

///*
	VPEDword  LeftCol, Width2, N32MWidth2, N16MWidth2;
	VPEDword  Coord, Delta;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	VPEDword  Raw;

	LoopCount     = pFLine->Count;              // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
	if(LoopCount == 0) {
		return;
	}

	Width2        = pFLine->Width2;             // mov cl, [ebx+Width2]		; get log 2 of tex width
	N32MWidth2    = 32 - Width2;
	N16MWidth2    = 16 - Width2;
	LeftCol       = pFLine->LeftCol;            // movzx eax, word ptr [ebx+LeftCol] ; put cur col to eax
	Delta         = pFLine->Delta;              // mov ebp, [ebx+Delta]		; get delta's
	LeftCol      *= Delta;                      // mul ebp				; make cur coords
	Coord         = pFLine->Coord + LeftCol;    // mov edx, [ebx+Coord]		; get start coords
	                                            // add edx, eax;			; edx contains full coords
	RawPtr        = pFLine->RawPtr;             // mov esi, [ebx+RawPtr]		; Tex pointer
	PixPtr        = pFLine->PixPtr;             // mov edi, [ebx+PixPtr]		; Buffer pointer
	PalPtr        = pFLine->PalPtr;             // mov ebx, [ebx+PalPtr]		; Pal buffer pointer

	// ecx = Width2
	// edx = Coord
	// ebp = Delta
	do {                                        // FLoopStart:				; Start of unrolled loop
		                                          // xor eax, eax
		Raw = ((Coord >> N32MWidth2) << Width2) | // shld eax, edx, cl
		      ((Coord & 0xFFFF) >> N16MWidth2);   // shld ax, dx, cl
		if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
			PixPtr[0] = PalPtr[RawPtr[Raw]];          // mov bl, [esi+eax]
		                                          // mov bl, [ebx]
		                                          // mov [edi], bl
		PixPtr += 1;                              // inc edi
		Coord  += Delta;                          // add edx, ebp

		LoopCount -= 1;                           // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                      // jae FLoopStart

//*/
}


/* ------------------------------ *** WLOOP.INC   *** ------------------------------ */
/* ------------------------------ *** WMLOOP.INC  *** ------------------------------ */
/* ------------------------------ *** WTOOP.INC   *** ------------------------------ */
/* ------------------------------ *** DRAW_WA.ASM *** ------------------------------ */

#define WLOOP_LEN   32
#define WLOOP_SIZE  15
#define WMLOOP_LEN  32
#define WMLOOP_SIZE 19
#define WTLOOP_LEN  32
#define WTLOOP_SIZE 20

int32_t WLoopOffset[] = {
	0x01E0,0x01D1,0x01C2,0x01B3,0x01A4,0x0195,0x0186,0x0177,
	0x0168,0x0159,0x014A,0x013B,0x012C,0x011D,0x010E,0x00FF,
	0x00F0,0x00E1,0x00D2,0x00C3,0x00B4,0x00A5,0x0096,0x0087,
	0x0078,0x0069,0x005A,0x004B,0x003C,0x002D,0x001E,0x000F
};

int32_t WMLoopOffset[] = {
	0x0260,0x024D,0x023A,0x0227,0x0214,0x0201,0x01EE,0x01DB,
	0x01C8,0x01B5,0x01A2,0x018F,0x017C,0x0169,0x0156,0x0143,
	0x0130,0x011D,0x010A,0x00F7,0x00E4,0x00D1,0x00BE,0x00AB,
	0x0098,0x0085,0x0072,0x005F,0x004C,0x0039,0x0026,0x0013
};

int32_t WTLoopOffset[] = {
	0x0280,0x026C,0x0258,0x0244,0x0230,0x021C,0x0208,0x01F4,
	0x01E0,0x01CC,0x01B8,0x01A4,0x0190,0x017C,0x0168,0x0154,
	0x0140,0x012C,0x0118,0x0104,0x00F0,0x00DC,0x00C8,0x00B4,
	0x00A0,0x008C,0x0078,0x0064,0x0050,0x003C,0x0028,0x0014
};

//****************************************************************************
// Draws a vertical wall span
//					EAX
//	void DrawWSpan(struct WLine *PWline)
//****************************************************************************
void DrawWSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

		mov ebx, dword ptr [PWline]

		movzx eax, word ptr [ebx+Count_]
		mov dword ptr [LoopCount], eax
		and eax, WLOOP_LEN-1
		mov esi, offset WLoopOffset
		mov eax, [esi+eax*4]
		add eax, offset WLoopStart
		push eax

		mov esi, [ebx+RawPtr_]
		mov ecx, [ebx+Delta_-2]
		xor cx, cx
		mov eax, [ebx+Coord_-2]
		mov al, [ebx+Delta_+2]
		mov ah, [ebx+WMask_]
		xor edx, edx
		mov dl, [ebx+Coord_+2]
		movzx ebp, word ptr [ebx+ViewWidth_]
		mov edi, [ebx+PixPtr_]
		mov ebx, [ebx+PalPtr_]

		ret

WLoopStart:
		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		and dl, ah
		mov bl, [esi+edx]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc dl, al

		sub dword ptr [LoopCount], WLOOP_LEN
		jae WLoopStart

		popad				; restore all registers
	}
*/

///*
	VPEDword  ViewWidth;
	VPEDword  Coord, CoordM2, DeltaM2;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	LoopCount = PWline->Count;                                  // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
	                                                            // mov LoopCount, eax		; LoopCount=EAX
	if(LoopCount == 0) {
		return;
	}

	DeltaM2   = (PWline->Delta & 0x0000FFFF) << 16;             // mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	                                                            // xor cx, cx			; clear lower ecx

	CoordM2   = ((PWline->Coord & 0x0000FFFF) << 16) |          // mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	            ((PWline->Delta & 0x00FF0000) >> 16) |          // mov al, [ebx+Delta_+2]
	            ((PWline->Mask  & 0x000000FF) <<  8);           // mov ah, [ebx+WMask_]

	Coord     = ((PWline->Coord & 0x00FF0000) >> 16);           // xor edx, edx			; int Coord in low edx
	                                                            // mov dl, [ebx+Coord_+2]

	ViewWidth = PWline->BufWidth;                               // movzx ebp, word ptr [ebx+ViewWidth_]
	RawPtr    = PWline->RawPtr;                                 // mov esi, [ebx+RawPtr_]		; raw ptr in esi
	PixPtr    = PWline->PixPtr;                                 // mov edi, [ebx+PixPtr_]
	PalPtr    = PWline->PalPtr;                                 // mov ebx, [ebx+PalPtr_]

	// eax = CoordM2
	// ecx = DeltaM2
	// edx = Coord
	// ebp = ViewWidth
	do {                                                         // WLoopStart:				; Start of unrolled loop

		Coord &= ((CoordM2 & 0xFF00) >> 8);                        // and dl, ah
		if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
			PixPtr[0] = PalPtr[RawPtr[Coord]];                         // mov bl, [esi+edx]
		                                                           // mov bl, [ebx]
		                                                           // mov [edi], bl
		PixPtr  += ViewWidth;                                      // add edi, ebp
		CoordM2 += DeltaM2;                                        // add eax, ecx
		Coord    = (Coord & 0xFFFFFF00) |
		        ((((Coord & 0xFF)       +
		         (CoordM2 & 0xFF)) & 0xFF) + (CoordM2 < DeltaM2)); // adc dl, al

		LoopCount -= 1;                                            // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                                       // jae WLoopStart
//*/
}

//****************************************************************************
// Draws a vertical masked wall span (any Tex width)
//					EAX
//	void DrawMaskWSpan(struct WLine *PWline)
//****************************************************************************
void DrawMaskWSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

	mov ebx, dword ptr [PWline]

	movzx eax, word ptr [ebx+Count_]
	mov dword ptr [LoopCount], eax
	and eax, WMLOOP_LEN-1
	mov esi, offset WMLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset WMLoopStart
	push eax

	mov esi, [ebx+RawPtr_]
	mov ecx, [ebx+Delta_-2]
	xor cx, cx
	mov eax, [ebx+Coord_-2]
	mov al, [ebx+Delta_+2]
	mov ah, [ebx+WMask_]
	xor edx, edx
	mov dl, [ebx+Coord_+2]
	movzx ebp, word ptr [ebx+ViewWidth_]
	mov edi, [ebx+PixPtr_]
	mov ebx, [ebx+PalPtr_]

	ret

WMLoopStart:
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [esi+edx]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc dl, al

	sub dword ptr [LoopCount], WMLOOP_LEN
	jae WMLoopStart

		popad
	}
*/
}

//****************************************************************************
// Draws a vertical translucent wall span (any Tex width)
//					EAX
//	void DrawTransWSpan(struct WLine *PWline)
//****************************************************************************
void DrawTransWSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

	mov ebx, eax

	movzx eax, word ptr [ebx+Count_]
	mov dword ptr [LoopCount], eax
	and eax, WTLOOP_LEN-1
	mov esi, offset WTLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset WTLoopStart
	push eax

	mov esi, [ebx+RawPtr_]
	mov ecx, [ebx+Delta_-2]
	xor cx, cx
	mov eax, [ebx+Coord_-2]
	mov al, [ebx+Delta_+2]
	mov ah, [ebx+WMask_]
	xor edx, edx
	mov dl, [ebx+Coord_+2]
	movzx ebp, word ptr [ebx+ViewWidth_]
	mov edi, [ebx+PixPtr_]
	mov ebx, [ebx+PalPtr_]
	sub ebx, ecx			; different from others!

	ret

WTLoopStart:
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al
         
	and dl, ah
	mov bl, [edi]
	mov ch, [esi+edx]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	xor ch, ch
	add eax, ecx
	adc dl, al

	sub dword ptr [LoopCount], WTLOOP_LEN
	jae WTLoopStart

		popad
	}
*/
		VPEDword  ViewWidth;
	VPEDword  Coord, CoordM2, DeltaM2;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	LoopCount = PWline->Count;                                  // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
	                                                            // mov LoopCount, eax		; LoopCount=EAX
	if(LoopCount == 0) {
		return;
	}

	DeltaM2   = (PWline->Delta & 0x0000FFFF) << 16;             // mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	                                                            // xor cx, cx			; clear lower ecx

	CoordM2   = ((PWline->Coord & 0x0000FFFF) << 16) |          // mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	            ((PWline->Delta & 0x00FF0000) >> 16) |          // mov al, [ebx+Delta_+2]
	            ((PWline->Mask  & 0x000000FF) <<  8);           // mov ah, [ebx+WMask_]

	Coord     = ((PWline->Coord & 0x00FF0000) >> 16);           // xor edx, edx			; int Coord in low edx
	                                                            // mov dl, [ebx+Coord_+2]

	ViewWidth = PWline->BufWidth;                               // movzx ebp, word ptr [ebx+ViewWidth_]
	RawPtr    = PWline->RawPtr;                                 // mov esi, [ebx+RawPtr_]		; raw ptr in esi
	PixPtr    = PWline->PixPtr;                                 // mov edi, [ebx+PixPtr_]
	PalPtr    = PWline->PalPtr;                                 // mov ebx, [ebx+PalPtr_]

	// eax = CoordM2
	// ecx = DeltaM2
	// edx = Coord
	// ebp = ViewWidth
	do { 
		Coord = (Coord & 0xFFFFFF00) |
		       ((Coord & 0xFF)       &
		     ((CoordM2 & 0xFF00) >> 8));                           // and dl, ah
		
		if(PalPtr[RawPtr[Coord]]>0)                                                        // WLoopStart:				; Start of unrolled loop
			if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
				PixPtr[0] = PalPtr[RawPtr[Coord]];                         // mov bl, [esi+edx]
		                                                           // mov bl, [ebx]
		                                                           // mov [edi], bl
		PixPtr  += ViewWidth;                                      // add edi, ebp
		CoordM2 += DeltaM2;                                        // add eax, ecx
		Coord    = (Coord & 0xFFFFFF00) |
		        ((((Coord & 0xFF)       +
		         (CoordM2 & 0xFF)) & 0xFF) + (CoordM2 < DeltaM2)); // adc dl, al

		LoopCount -= 1;                                            // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                                       // jae WLoopStart
//*/
}



#define OLOOP_LEN   32
#define OLOOP_SIZE  12
#define OMLOOP_LEN  32
#define OMLOOP_SIZE 16
#define OTLOOP_LEN  32
#define OTLOOP_SIZE 15

int32_t OLoopOffset[] = {
	0x0180,0x0174,0x0168,0x015C,0x0150,0x0144,0x0138,0x012C,
	0x0120,0x0114,0x0108,0x00FC,0x00F0,0x00E4,0x00D8,0x00CC,
	0x00C0,0x00B4,0x00A8,0x009C,0x0090,0x0084,0x0078,0x006C,
	0x0060,0x0054,0x0048,0x003C,0x0030,0x0024,0x0018,0x000C
};

int32_t OMLoopOffset[] = {
	0x0200,0x01F0,0x01E0,0x01D0,0x01C0,0x01B0,0x01A0,0x0190,
	0x0180,0x0170,0x0160,0x0150,0x0140,0x0130,0x0120,0x0110,
	0x0100,0x00F0,0x00E0,0x00D0,0x00C0,0x00B0,0x00A0,0x0090,
	0x0080,0x0070,0x0060,0x0050,0x0040,0x0030,0x0020,0x0010
};

int32_t OTLoopOffset[] = {
	0x01E0,0x01D1,0x01C2,0x01B3,0x01A4,0x0195,0x0186,0x0177,
	0x0168,0x0159,0x014A,0x013B,0x012C,0x011D,0x010E,0x00FF,
	0x00F0,0x00E1,0x00D2,0x00C3,0x00B4,0x00A5,0x0096,0x0087,
	0x0078,0x0069,0x005A,0x004B,0x003C,0x002D,0x001E,0x000F
};

//****************************************************************************
// Draws a vertical object span (any Tex width)
//					EAX
//	void DrawOSpan(struct WLine *PWline)
//****************************************************************************
void DrawOSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

		mov ebx, dword ptr [PWline]

		movzx eax, word ptr [ebx+Count_]
		mov dword ptr [LoopCount], eax
		and eax, OLOOP_LEN-1
		mov esi, offset OLoopOffset
		mov eax, [esi+eax*4]
		add eax, offset OLoopStart
		push eax

		mov esi, [ebx+RawPtr_]		; raw ptr in esi

		mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
		xor ax, ax

		mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
		xor cx, cx			; clear lower ecx

		xor edx, edx			; int Coord in low edx
		mov dx, [ebx+Coord_+2]
		add esi, edx			; Advance raw ptr

		xor edx, edx			; int Delta in low edx
		mov dx, [ebx+Delta_+2]

		movzx ebp, word ptr [ebx+ViewWidth_]

		mov edi, [ebx+PixPtr_]
		mov ebx, [ebx+PalPtr_]

		ret

OLoopStart:
		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		mov bl, [esi]
		mov bl, [ebx]
		mov [edi], bl
		add edi, ebp
		add eax, ecx
		adc esi, edx

		sub dword ptr [LoopCount], OLOOP_LEN
		jae OLoopStart

		popad
	}
*/
///*
//struct WLine {
//	VPEByte  *RawPtr, *PalPtr, *PixPtr;
//	VPEFixed  Coord, Delta;
//	VPEShort  ViewWidth, Count, Mask;
//};
//#define RawPtr_     0 // Tex ptr
//#define PalPtr_     4 // Pal table ptr
//#define PixPtr_     8 // Buffer ptr
//#define Coord_     12 // Coord data
//#define Delta_     16 // Delta
//#define LeftCol_   20 // LeftCol (FLine)
//#define ViewWidth_ 20 // View Width (WLine)
//#define Count_     22 // Size
//#define Width2_    24 // log2 Width (FLine)
//#define WMask_     24 // Mask (WLine)

	VPEDword  ViewWidth;
	VPEDword  CoordM2, Delta, DeltaM2;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	LoopCount = PWline->Count;                      // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
                                                  // mov LoopCount, eax		; LoopCount=EAX
	if(LoopCount == 0) {
		return;
	}

	CoordM2   = (PWline->Coord & 0x0000FFFF) << 16; // mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	                                                // xor ax, ax
	DeltaM2   = (PWline->Delta & 0x0000FFFF) << 16; // mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	                                                // xor cx, cx			; clear lower ecx
	Delta     = (PWline->Delta & 0xFFFF0000) >> 16; // xor edx, edx			; int Delta in low edx
	                                                // mov dx, [ebx+Delta_+2]
	ViewWidth = PWline->BufWidth;                   // movzx ebp, word ptr [ebx+ViewWidth_]
	RawPtr    = PWline->RawPtr +                    // mov esi, [ebx+RawPtr_]		; raw ptr in esi
	          ((PWline->Coord & 0xFFFF0000) >> 16); // xor edx, edx			; int Coord in low edx
	                                                // mov dx, [ebx+Coord_+2]
	                                                // add esi, edx			; Advance raw ptr
	PixPtr    = PWline->PixPtr;                     // mov edi, [ebx+PixPtr_]
	PalPtr    = PWline->PalPtr;                     // mov ebx, [ebx+PalPtr_]

	do {                      
	if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
		PixPtr[0] = PalPtr[RawPtr[0]];                // mov bl, [esi]
		                                              // mov bl, [ebx]
		                                              // mov [edi], bl
		PixPtr  += ViewWidth;                         // add edi, ebp
		CoordM2 += DeltaM2;                           // add eax, ecx
		RawPtr  += Delta + (CoordM2 < DeltaM2);       // adc esi, edx

		LoopCount -= 1;                               // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                          // jae OLoopStart
//*/
}

//****************************************************************************
// Draws a vertical masked object span (any Tex width)
//					EAX
//	void DrawMaskOSpan(struct WLine *PWline)
//****************************************************************************
void DrawMaskOSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

	mov ebx, dword ptr [PWline]

	movzx eax, word ptr [ebx+Count_]
	mov dword ptr [LoopCount], eax
	and eax, OMLOOP_LEN-1
	mov esi, offset OMLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset OMLoopStart
	push eax

	mov esi, [ebx+RawPtr_]		; raw ptr in esi

	mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord_+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta_+2]

	movzx ebp, word ptr [ebx+ViewWidth_]

	mov edi, [ebx+PixPtr_]
	mov ebx, [ebx+PalPtr_]

	ret

OMLoopStart:
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
        
	mov bl, [esi]
	or bl, bl
	je $+6
	mov bl, [ebx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx

	sub dword ptr [LoopCount], OMLOOP_LEN
	jae OMLoopStart

		popad
	}
*/


	VPEDword  ViewWidth;
	VPEDword  CoordM2, Delta, DeltaM2;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	LoopCount = PWline->Count;                      // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
                                                  // mov LoopCount, eax		; LoopCount=EAX
	if(LoopCount == 0) {
		return;
	}

	CoordM2   = (PWline->Coord & 0x0000FFFF) << 16; // mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	                                                // xor ax, ax
	DeltaM2   = (PWline->Delta & 0x0000FFFF) << 16; // mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	                                                // xor cx, cx			; clear lower ecx
	Delta     = (PWline->Delta & 0xFFFF0000) >> 16; // xor edx, edx			; int Delta in low edx
	                                                // mov dx, [ebx+Delta_+2]
	ViewWidth = PWline->BufWidth;                   // movzx ebp, word ptr [ebx+ViewWidth_]
	RawPtr    = PWline->RawPtr +                    // mov esi, [ebx+RawPtr_]		; raw ptr in esi
	          ((PWline->Coord & 0xFFFF0000) >> 16); // xor edx, edx			; int Coord in low edx
	                                                // mov dx, [ebx+Coord_+2]
	                                                // add esi, edx			; Advance raw ptr
	PixPtr    = PWline->PixPtr;                     // mov edi, [ebx+PixPtr_]
	PalPtr    = PWline->PalPtr;                     // mov ebx, [ebx+PalPtr_]

	do {             
		if(PalPtr[RawPtr[0]]>0)                               // OLoopStart:				; Start of unrolled loop
			if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
				PixPtr[0] = PalPtr[RawPtr[0]];                // mov bl, [esi]
		                                              // mov bl, [ebx]
		                                              // mov [edi], bl
		PixPtr  += ViewWidth;                         // add edi, ebp
		CoordM2 += DeltaM2;                           // add eax, ecx
		RawPtr  += Delta + (CoordM2 < DeltaM2);       // adc esi, edx

		LoopCount -= 1;                               // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                          // jae OLoopStart

}

//****************************************************************************
// Draws a vertical translucent object span (any Tex width)
//					EAX
//	void DrawTransOSpan(struct WLine *PWline)
//****************************************************************************
void DrawTransOSpan(struct WLine *PWline) {
/*
	_asm {
		pushad

	mov ebx, dword ptr [PWline]

	movzx eax, word ptr [ebx+Count_]
	mov dword ptr [LoopCount], eax
	and eax, OTLOOP_LEN-1
	mov esi, offset OTLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset OTLoopStart
	push eax

	mov esi, [ebx+RawPtr_]		; raw ptr in esi

	mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord_+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta_+2]

	movzx ebp, word ptr [ebx+ViewWidth_]

	mov edi, [ebx+PixPtr_]
	mov ebx, [ebx+PalPtr_]

	sub ebx, ecx			; make [ebx+ecx] pointer

	ret

OTLoopStart:
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx
       
	mov bl, [edi]
	mov ch, [esi]
	mov bl, [ebx+ecx]
	mov [edi], bl
	add edi, ebp
	add eax, ecx
	adc esi, edx

	sub dword ptr [LoopCount], OTLOOP_LEN
	jae OTLoopStart

		popad
	}
*/


	VPEDword  ViewWidth;
	VPEDword  CoordM2, Delta, DeltaM2;
	VPEByte  *RawPtr, *PalPtr, *PixPtr;

	LoopCount = PWline->Count;                      // movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
                                                  // mov LoopCount, eax		; LoopCount=EAX
	if(LoopCount == 0) {
		return;
	}

	CoordM2   = (PWline->Coord & 0x0000FFFF) << 16; // mov eax, [ebx+Coord_-2]		; frac Coord in hi eax
	                                                // xor ax, ax
	DeltaM2   = (PWline->Delta & 0x0000FFFF) << 16; // mov ecx, [ebx+Delta_-2]		; frac Delta in hi ecx
	                                                // xor cx, cx			; clear lower ecx
	Delta     = (PWline->Delta & 0xFFFF0000) >> 16; // xor edx, edx			; int Delta in low edx
	                                                // mov dx, [ebx+Delta_+2]
	ViewWidth = PWline->BufWidth;                   // movzx ebp, word ptr [ebx+ViewWidth_]
	RawPtr    = PWline->RawPtr +                    // mov esi, [ebx+RawPtr_]		; raw ptr in esi
	          ((PWline->Coord & 0xFFFF0000) >> 16); // xor edx, edx			; int Coord in low edx
	                                                // mov dx, [ebx+Coord_+2]
	                                                // add esi, edx			; Advance raw ptr
	PixPtr    = PWline->PixPtr;                     // mov edi, [ebx+PixPtr_]
	PalPtr    = PWline->PalPtr;                     // mov ebx, [ebx+PalPtr_]

	do {     
		if(PalPtr[RawPtr[0]]>0)
			if(PixPtr>=ScrBase && PixPtr<ScrBase+ScrWidth*ScrHeight)                      // OLoopStart:				; Start of unrolled loop
				PixPtr[0] = PalPtr[RawPtr[0]+256*PixPtr[0]];                // mov bl, [esi]
		                                              // mov bl, [ebx]
		                                              // mov [edi], bl
		PixPtr  += ViewWidth;                         // add edi, ebp
		CoordM2 += DeltaM2;                           // add eax, ecx
		RawPtr  += Delta + (CoordM2 < DeltaM2);       // adc esi, edx

		LoopCount -= 1;                               // sub dword ptr LoopCount, 1
	}while(LoopCount > 0);                          // jae OLoopStart
}



//extern VPint