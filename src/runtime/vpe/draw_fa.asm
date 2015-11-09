	.386P

include vpe.inc
include floop.inc

	public DrawFSpan_

	.CODE

;****************************************************************************
; Draws a horizontal floor span
;				EAX (EBP+8)
;	void DrawFSpan(struct HLine *pHLine)
;****************************************************************************
DrawFSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; put all registers to stack

	mov ebx, eax			; pointer to HLine structure

	movzx eax, word ptr [ebx+Count]	; EAX=pixels_to_draw
	mov LoopCount, eax		; LoopCount=EAX
	and eax, FLOOP_LEN-1		; EAX=pixels_to_draw%LOOP_LEN
	mov esi, offset FLoopOffset	; Address of the offset table
	mov eax, [esi+eax*4]		; EAX=relative offset of the cell
	add eax, offset FLoopStart	; EAX=absolute address of start cell
	push eax

	mov cl, [ebx+Width2]		; get log 2 of tex width
	movzx eax, word ptr [ebx+LeftCol] ; put cur col to eax
	mov ebp, [ebx+Delta]		; get delta's
	mul ebp				; make cur coords
	mov edx, [ebx+Coord]		; get start coords
	add edx, eax;			; edx contains full coords
	mov esi, [ebx+RawPtr]		; Tex pointer
	mov edi, [ebx+PixPtr]		; Buffer pointer
	mov ebx, [ebx+PalPtr]		; Pal buffer pointer

	ret				; Jump to the start loop cell

FLoopStart:				; Start of unrolled loop
	FLoop				; The loop itself (macro)

	sub dword ptr LoopCount, FLOOP_LEN
	jae FLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return 
DrawFSpan_ endp

	.DATA

LoopCount	dd 0

end
