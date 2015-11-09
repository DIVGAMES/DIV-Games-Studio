; struct FLine {
;	BYTE  *RawPtr, *PalPtr, *PixPtr;
;	DWORD  Coord, Delta;
;	SHORT  LeftCol, Count, Width2;
;	struct Region *pRegion;
; };
; struct WLine {
;	BYTE  *RawPtr, *PalPtr, *PixPtr;
;	FIXED  Coord, Delta;
;	SHORT  ViewWidth, Count, Mask;
; };

RawPtr		equ 0			; Tex ptr
PalPtr		equ 4			; Pal table ptr
PixPtr		equ 8			; Buffer ptr
Coord		equ 12			; Coord data
Delta		equ 16			; Delta
LeftCol		equ 20			; LeftCol (FLine)
ViewWidth	equ 20			; View Width (WLine)
Count		equ 22			; Size
Width2		equ 24			; log2 Width (FLine)
WMask		equ 24			; Mask (WLine)

