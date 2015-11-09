	.386P

include vpe.inc
include wloop.inc
include wmloop.inc
include wtloop.inc

	public DrawWSpan_
	public DrawMaskWSpan_
	public DrawTransWSpan_

	.CODE

;****************************************************************************
; Draws a vertical wall span
;					EAX
;	void DrawWSpan(struct WLine *pWLine)
;****************************************************************************
DrawWSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, WLOOP_LEN-1
	mov esi, offset WLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset WLoopStart
	push eax

	mov esi, [ebx+RawPtr]
	mov ecx, [ebx+Delta-2]
	xor cx, cx
	mov eax, [ebx+Coord-2]
	mov al, [ebx+Delta+2]
	mov ah, [ebx+WMask]
	xor edx, edx
	mov dl, [ebx+Coord+2]
	movzx ebp, word ptr [ebx+ViewWidth]
	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]

	ret

WLoopStart:
	WLoop				; unrolled loop is in the macro

	sub dword ptr LoopCount, WLOOP_LEN
	jae WLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawWSpan_ endp

;****************************************************************************
; Draws a vertical masked wall span (any Tex width)
;					EAX
;	void DrawMaskWSpan(struct WLine *pWLine)
;****************************************************************************
DrawMaskWSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, WMLOOP_LEN-1
	mov esi, offset WMLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset WMLoopStart
	push eax

	mov esi, [ebx+RawPtr]
	mov ecx, [ebx+Delta-2]
	xor cx, cx
	mov eax, [ebx+Coord-2]
	mov al, [ebx+Delta+2]
	mov ah, [ebx+WMask]
	xor edx, edx
	mov dl, [ebx+Coord+2]
	movzx ebp, word ptr [ebx+ViewWidth]
	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]

	ret

WMLoopStart:
	WMLoop				; unrolled loop

	sub dword ptr LoopCount, WMLOOP_LEN
	jae WMLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawMaskWSpan_ endp

;****************************************************************************
; Draws a vertical translucent wall span (any Tex width)
;					EAX
;	void DrawTransWSpan(struct WLine *pWLine)
;****************************************************************************
DrawTransWSpan_ proc near
        push ebp			; preserve caller's stack frame
        mov ebp,esp			; point to our stack frame
	pushad				; save all registers
	mov ebx, eax

	movzx eax, word ptr [ebx+Count]
	mov LoopCount, eax
	and eax, WTLOOP_LEN-1
	mov esi, offset WTLoopOffset
	mov eax, [esi+eax*4]
	add eax, offset WTLoopStart
	push eax

	mov esi, [ebx+RawPtr]
	mov ecx, [ebx+Delta-2]
	xor cx, cx
	mov eax, [ebx+Coord-2]
	mov al, [ebx+Delta+2]
	mov ah, [ebx+WMask]
	xor edx, edx
	mov dl, [ebx+Coord+2]
	movzx ebp, word ptr [ebx+ViewWidth]
	mov edi, [ebx+PixPtr]
	mov ebx, [ebx+PalPtr]
	sub ebx, ecx			; different from others!

	ret

WTLoopStart:
	WTLoop				; unrolled loop

	sub dword ptr LoopCount, WTLOOP_LEN
	jae WTLoopStart

	popad				; restore all registers
	mov esp, ebp
        pop ebp				; restore caller's stack frame
        ret				; return
DrawTransWSpan_ endp

	.DATA

LoopCount	dd 0

end
