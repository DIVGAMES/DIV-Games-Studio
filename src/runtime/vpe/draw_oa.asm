	.386P

include vpe.inc
include oloop.inc
include omloop.inc
include otloop.inc

	public DrawOSpan_
	public DrawMaskOSpan_
	public DrawTransOSpan_

	.CODE

;****************************************************************************
; Draws a vertical object span (any Tex width)
;					EAX
;	void DrawOSpan(struct WLine *pWLine)
;****************************************************************************
DrawOSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, OLOOP_LEN-1
	mov esi, offset OLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset OLoopStart
	push eax

	mov esi, [ebx+RawPtr]		; raw ptr in esi

	mov eax, [ebx+Coord-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta+2]

	movzx ebp, word ptr [ebx+ViewWidth]

	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]

	ret

OLoopStart:
	OLoop				; unrolled loop

	sub dword ptr LoopCount, OLOOP_LEN
	jae OLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawOSpan_ endp


;****************************************************************************
; Draws a vertical masked object span (any Tex width)
;					EAX
;	void DrawMaskOSpan(struct WLine *pWLine)
;****************************************************************************
DrawMaskOSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, OMLOOP_LEN-1
	mov esi, offset OMLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset OMLoopStart
	push eax

	mov esi, [ebx+RawPtr]		; raw ptr in esi

	mov eax, [ebx+Coord-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta+2]

	movzx ebp, word ptr [ebx+ViewWidth]

	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]

	ret

OMLoopStart:
	OMLoop				; unrolled loop

	sub dword ptr LoopCount, OMLOOP_LEN
	jae OMLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawMaskOSpan_ endp


;****************************************************************************
; Draws a vertical translucent object span (any Tex width)
;					EAX
;	void DrawTransOSpan(struct WLine *pWLine)
;****************************************************************************
DrawTransOSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, OTLOOP_LEN-1
	mov esi, offset OTLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset OTLoopStart
	push eax

	mov esi, [ebx+RawPtr]		; raw ptr in esi

	mov eax, [ebx+Coord-2]		; frac Coord in hi eax
	xor ax, ax

	mov ecx, [ebx+Delta-2]		; frac Delta in hi ecx
	xor cx, cx			; clear lower ecx

	xor edx, edx			; int Coord in low edx
	mov dx, [ebx+Coord+2]
	add esi, edx			; Advance raw ptr

	xor edx, edx			; int Delta in low edx
	mov dx, [ebx+Delta+2]

	movzx ebp, word ptr [ebx+ViewWidth]

	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]

	sub ebx, ecx			; make [ebx+ecx] pointer

	ret

OTLoopStart:
	OTLoop				; unrolled loop

	sub dword ptr LoopCount, OTLOOP_LEN
	jae OTLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawTransOSpan_ endp

	.DATA

LoopCount	dd 0

end
