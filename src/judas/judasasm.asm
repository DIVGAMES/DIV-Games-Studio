; This module contains soundcard IRQ handlers, DMA routines, mixing routines
; as well as some time-critical GUS routines.
;
; N„m„ ei mit„„n (kovin) optimisoituja rutiineja ole, PERKELE!

                .386
                .model flat

                include judascfg.inc
                include judasgus.inc

MONO            =       0
EIGHTBIT        =       0
STEREO          =       1
SIXTEENBIT      =       2

VM_OFF          =       0
VM_ON           =       1
VM_LOOP         =       2
VM_16BIT        =       4

DEV_NOSOUND     =       0
DEV_SB          =       1
DEV_SBPRO       =       2
DEV_SB16        =       3
DEV_GUS         =       4
DEV_FILE        =       5

CACHESLOTS      =       16

IPMINUS1        =       -1
IP0             =       0
IP1             =       1
IP2             =       2

CACHESLOT       struc
GDC_Pos         dd      0
GDC_Length      dd      0
CACHESLOT       ends

DMACHANNEL      struc
DMA_PagePort    dw      0
DMA_AdrPort     dw      0
DMA_LenPort     dw      0
DMA_MaskPort    dw      0
DMA_ModePort    dw      0
DMA_ClearPort   dw      0
DMA_Mask        db      0
DMA_UnMask      db      0
DMA_Unused      dw      0
DMACHANNEL      ends

CHANNEL          struc
Chn_Pos          dd      0
Chn_Repeat       dd      0
Chn_End          dd      0
Chn_Sample       dd      0
Chn_Freq         dd      0
Chn_FractPos     dw      0
Chn_MasterVol    db      0
Chn_Panning      db      0
Chn_Vol          dw      0
Chn_VoiceMode    db      0
Chn_PrevVM       db      0
Chn_PrevPos      dd      0
Chn_LastValL     dd      0
Chn_LastValR     dd      0
Chn_SmoothVolL   dd      0
Chn_SmoothVolR   dd      0
CHANNEL          ends


                .code

                public  judas_code_lock_start_
                public  judas_code_lock_end_
                public  judas_update_
                public  judas_get_ds_
                public  sb_handler_
                public  sb_aihandler_
                public  sb16_handler_
                public  gus_handler_
                public  gus_peek_
                public  gus_poke_
                public  gus_handler_
                public  gus_dmawait_
                public  gus_dmainit_
                public  gus_dmaprogram_
                public  gus_startchannels_
                public  fmixer_
                public  qmixer_
                public  safemixer_
                public  normalmix_
                public  ipmix_
                public  qmix_linear_
                public  qmix_cubic_
                public  dma_program_

                extrn   _judas_ds:word
                extrn   _judas_initialized:byte
                extrn   _judas_mixmode:byte
                extrn   _judas_samplesize:byte
                extrn   _judas_clipbuffer:dword
                extrn   _judas_zladdbuffer:dword
                extrn   _judas_zerolevell:dword
                extrn   _judas_zerolevelr:dword
                extrn   _judas_cliptable:dword
                extrn   _judas_volumetable:dword
                extrn   _judas_mixrate:dword
                extrn   _judas_channel:dword
                extrn   _judas_mixroutine:dword
                extrn   _judas_mixersys:dword
                extrn   _judas_device:dword
                extrn   _judas_port:dword
                extrn   _judas_irq:dword
                extrn   _judas_dma:dword
                extrn   _judas_irqcount:dword
                extrn   _judas_bufferlength:dword
                extrn   _judas_buffermask:dword
                extrn   _judas_bpmcount:dword
                extrn   _judas_bpmtempo:byte
                extrn   _judas_player:dword
                extrn   _judas_mixpos:dword
                extrn   _dma_address:dword
                extrn   _judas_clipped:byte
                extrn   _reloj:dword

judas_get_ds_:  mov     AX, DS
                mov     _judas_ds, AX
                ret

judas_code_lock_start_:

                .const

                align   4

DMAChannels     DMACHANNEL <87h, 0h, 1h, 0ah, 0bh, 0ch, 4h, 0h>
                DMACHANNEL <83h, 2h, 3h, 0ah, 0bh, 0ch, 5h, 1h>
                DMACHANNEL <81h, 4h, 5h, 0ah, 0bh, 0ch, 6h, 2h>
                DMACHANNEL <82h, 6h, 7h, 0ah, 0bh, 0ch, 7h, 3h>
                DMACHANNEL <8fh, 0c0h, 0c2h, 0d4h, 0d6h, 0d8h, 4h, 0h>
                DMACHANNEL <8bh, 0c4h, 0c6h, 0d4h, 0d6h, 0d8h, 5h, 1h>
                DMACHANNEL <89h, 0c8h, 0cah, 0d4h, 0d6h, 0d8h, 6h, 2h>
                DMACHANNEL <8ah, 0cch, 0ceh, 0d4h, 0d6h, 0d8h, 7h, 3h>

                align   4

shittable       dd      0, 60, 56, 52, 48, 44, 40, 36
                dd      32, 28, 24, 20, 16, 12, 8, 4

                .data

                align   4

gdc             CACHESLOT CACHESLOTS dup (<>)

                align   4

loopcount       dd      0
fractadd        dd      0
integeradd      dd      0
smpend          dd      0
smpsubtract     dd      0
samples         dd      0
totalwork       dd      0
postproc        dd      0
cptr            dd      0
dptr            dd      0
fptr            dd      0
ipminus1        dd      0
ip0             dd      0
ip1             dd      0
ip2             dd      0
leftvol         dd      0
rightvol        dd      0
SmoothVolL      dd      0
SmoothVolR      dd      0
saved_reg       dd      0

mix_exec        db      0
gus_dmainprogress db    0

                .code

                align   4

        ;DMA functions. DMA polling is really fucked up: if reading the
        ;position too often (> 100 Hz) one may get bogus values. This is
        ;compensated by reading two values, and if their offset is too big or
        ;they're outside the buffer, the position is read again.
        ;
        ;Actually GUS fucks up just in the same way when reading the channel
        ;position. Shit, what is wrong with the hardware?!
        ;
        ;Previously I though that EMM386 causes these fuckups, but no, it
        ;wasn't so. However, under DPMI there's no fuckups!
        ;
        ;It would be really nice & simple to just update one bufferhalf at a
        ;time in the soundcard interrupt, but I think it's important to give
        ;the user full control of the sound updating, even at the expense of
        ;PAIN!!!

dma_program_:   push    ESI
                push    EDI
                push    ECX
                mov     ECX, EAX                        ;ECX = mode
                mov     EDI, EDX                        ;EDI = offset
                mov     ESI, _judas_dma                 ;Get channel num
                cmp     ESI, 4
                jae     dma16_program
                shl     ESI, 4                          ;16 = dma struc len
                add     ESI, offset DMAChannels         ;Ptr now ready
                mov     DX, [ESI.DMA_MaskPort]
                mov     AL, [ESI.DMA_Mask]
                out     DX, AL                          ;Mask the DMA channel
                xor     AL, AL
                mov     DX, [ESI.DMA_ClearPort]
                out     DX, AL                          ;Clear byte ptr.
                mov     DX, [ESI.DMA_ModePort]
                mov     AL, CL                          ;Get mode
                or      AL, [ESI.DMA_UnMask]            ;Or with channel num
                out     DX, AL                          ;Set DMA mode
                mov     DX, [ESI.DMA_LenPort]
                dec     EBX                             ;EBX = length
                mov     AL, BL
                out     DX, AL                          ;Set length low and
                mov     AL, BH                          ;high bytes
                out     DX, AL
                mov     DX, [ESI.DMA_AdrPort]
                mov     EBX, _dma_address               ;Get DMA buffer address
                add     EBX, EDI                        ;Add offset
                mov     AL, BL
                out     DX, AL                          ;Set offset
                mov     AL, BH
                out     DX, AL
                mov     DX, [ESI.DMA_PagePort]
                shr     EBX, 16
                mov     AL, BL
                out     DX, AL                          ;Set page
                mov     DX, [ESI.DMA_MaskPort]
                mov     AL, [ESI.DMA_UnMask]
                out     DX, AL                          ;Unmask the DMA channel
                pop     ECX
                pop     EDI
                pop     ESI
                ret
dma16_program:  shl     ESI, 4                          ;16 = dma struc len
                add     ESI, offset DMAChannels         ;Ptr now ready
                mov     DX, [ESI.DMA_MaskPort]
                mov     AL, [ESI.DMA_Mask]
                out     DX, AL                          ;Mask the DMA channel
                xor     AL, AL
                mov     DX, [ESI.DMA_ClearPort]
                out     DX, AL                          ;Clear byte ptr.
                mov     DX, [ESI.DMA_ModePort]
                mov     AL, CL                          ;Get mode
                or      AL, [ESI.DMA_UnMask]            ;Or with channel num
                out     DX, AL                          ;Set DMA mode
                mov     DX, [ESI.DMA_LenPort]
                shr     EBX, 1
                dec     EBX
                mov     AL, BL
                out     DX, AL                          ;Set length low and
                mov     AL, BH                          ;high bytes
                out     DX, AL
                mov     DX, [ESI.DMA_AdrPort]
                mov     EBX, _dma_address               ;Get DMA buffer address
                add     EBX, EDI                        ;Add offset
                shr     EBX, 1                          ;Because of 16-bitness
                mov     AL, BL
                out     DX, AL                          ;Set offset
                mov     AL, BH
                out     DX, AL
                mov     DX, [ESI.DMA_PagePort]
                shr     EBX, 15
                mov     AL, BL
                out     DX, AL                          ;Set page
                mov     DX, [ESI.DMA_MaskPort]
                mov     AL, [ESI.DMA_UnMask]
                out     DX, AL                          ;Unmask the DMA channel
                pop     ECX
                pop     EDI
                pop     ESI
                ret

dma_query_:     cli
                push    EBX
                push    ECX
                push    EDX
                push    ESI
                mov     ESI, _judas_dma
                cmp     ESI, 4
                jae     dma16_query
                shl     ESI, 4                          ;16 = dma struc len
                add     ESI, offset DMAChannels         ;Ptr now ready
                xor     EAX, EAX
                mov     DX, [ESI.DMA_ClearPort]         ;Clear flip-flop
                out     DX, AL
                mov     DX, [ESI.DMA_AdrPort]
dqloop1:        xor     EAX, EAX
                in      AL, DX
                xchg    AL, AH
                in      AL, DX
                xchg    AL, AH
                sub     AX, word ptr _dma_address       ;Subtract page offset
                mov     EBX, EAX                        ;EBX = position 1
                in      AL, DX
                xchg    AL, AH
                in      AL, DX
                xchg    AL, AH
                sub     AX, word ptr _dma_address       ;Subtract page offset
                mov     ECX, EAX                        ;ECX = position 2
                cmp     EBX, _judas_bufferlength        ;Outside buffer?
                jae     dqloop1
                mov     EAX, EBX
                sub     EAX, ECX
                cmp     EAX, 64
                jg      dqloop1
                cmp     EAX, -64
                jl      dqloop1
                mov     EAX, EBX
                pop     ESI
                pop     EDX
                pop     ECX
                pop     EBX
                sti
                ret
dma16_query:    shl     ESI, 4                          ;16 = dma struc len
                add     ESI, offset DMAChannels         ;Ptr now ready
                mov     DX, [ESI.DMA_ClearPort]         ;Clear flip-flop
                xor     EAX, EAX
                out     DX, AL
                mov     DX, [ESI.DMA_AdrPort]
                mov     ESI, _dma_address
                and     ESI, 1ffffh
dqloop2:        xor     EAX, EAX
                in      AL, DX
                xchg    AL, AH
                in      AL, DX
                xchg    AL, AH
                shl     EAX, 1
                sub     EAX, ESI                        ;Subtract page offset
                mov     EBX, EAX                        ;EBX = position 1
                xor     EAX, EAX
                in      AL, DX
                xchg    AL, AH
                in      AL, DX
                xchg    AL, AH
                shl     EAX, 1
                sub     EAX, ESI                        ;Subtract page offset
                mov     ECX, EAX                        ;ECX = position 2
                cmp     EBX, _judas_bufferlength        ;Outside buffer?
                jae     dqloop2
                mov     EAX, EBX
                sub     EAX, ECX
                cmp     EAX, 64
                jg      dqloop2
                cmp     EAX, -64
                jl      dqloop2
                mov     EAX, EBX
                pop     ESI
                pop     EDX
                pop     ECX
                pop     EBX
                sti
                ret

        ;Generic send-EOI routine.

send_eoi:       inc     _judas_irqcount
                cmp     _judas_irq, 8
                jae     highirq
                mov     AL, 20h
                out     20h, AL
                ret
highirq:        mov     AL, 20h
                out     0a0h, AL
                mov     AL, 00001011b
                out     0a0h, AL
                in      AL, 0a0h
                or      AL, AL
                jnz     sb_noeoi
                mov     AL, 20h
                out     20h, AL
sb_noeoi:       ret

        ;Soundblaster IRQ handlers, one for singlecycle, one for 8bit autoinit
        ;and one for 16bit autoinit.

sb_handler_:    pushad
                push    DS
                mov     AX, CS:_judas_ds
                mov     DS, AX
                mov     EDX, _judas_port
                add     EDX, 0eh
                in      AL, DX
                sub     EDX, 2h
sb_wait1:       in      AL, DX
                or      AL, AL
                js      sb_wait1
                mov     AL, 14h
                out     DX, AL
sb_wait2:       in      AL, DX
                or      AL, AL
                js      sb_wait2
                mov     AX, 0fff0h
                out     DX, AL
sb_wait3:       in      AL, DX
                or      AL, AL
                js      sb_wait3
                mov     AL, AH
                out     DX, AL
                sti
                call    send_eoi
                pop     DS
                popad
                iretd

sb_aihandler_:  pushad
                push    DS
                mov     AX, CS:_judas_ds
                mov     DS, AX
                mov     EDX, _judas_port
                add     EDX, 0eh
                in      AL, DX
                sti
                call    send_eoi
                pop     DS
                popad
                iretd

sb16_handler_:  pushad
                push    DS
                mov     AX, CS:_judas_ds
                mov     DS, AX
                mov     EDX, _judas_port
                add     EDX, 0fh
                in      AL, DX
                sti
                call    send_eoi
                pop     DS
                popad
                iretd

        ;GUS IRQ handler

gus_handler_:   pushad
                push    DS
                mov     AX, CS:_judas_ds
                mov     DS, AX
gus_irqloop:    mov     EDX, _judas_port
                add     EDX, GF1_IRQ_STAT
                in      AL, DX
                test    AL, DMA_TC_IRQ
                jz      gus_irqdone
                mov     EDX, _judas_port                ;Acknowledge the DMA
                add     EDX, GF1_REG_SELECT             ;interrupt
                mov     AL, DMA_CONTROL
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_HI
                in      AL, DX
                dec     gus_dmainprogress
                mov     ESI, offset gdc
                mov     ECX, CACHESLOTS
gusirq_seekslot:cmp     [ESI.GDC_Length], 0
                jnz     gusirq_slotfound
                add     ESI, type CACHESLOT
                dec     ECX
                jnz     gusirq_seekslot
                jmp     gus_irqloop
gusirq_slotfound:
                mov     EBX, [ESI.GDC_Pos]              ;DMA offset
                shr     EBX, 4
                mov     CL, DMA_ENABLE or DMA_R0 or DMA_TWOS_COMP or DMA_IRQ_ENABLE
                test    _judas_mixmode, SIXTEENBIT
                jz      gus_dma_eight2
                mov     CL, DMA_ENABLE or DMA_R0 or DMA_DATA_16 or DMA_IRQ_ENABLE
gus_dma_eight2: cmp     _judas_dma, 4
                jb      gus_nohighdma2
                or      CL, DMA_WIDTH_16
                shr     EBX, 1
gus_nohighdma2: mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, SET_DMA_ADDRESS
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                mov     AX, BX
                out     DX, AX
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, DMA_CONTROL
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_HI
                mov     AL, CL
                out     DX, AL
                mov     EBX, [ESI.GDC_Length]
                mov     [ESI.GDC_Length], 0
                mov     EDX, [ESI.GDC_Pos]              ;DMA offset
                mov     EAX, 48h                        ;DMA mode
                call    dma_program_                    ;Program it!
                jmp     gus_irqloop
gus_irqdone:    sti
                call    send_eoi
                pop     DS
                popad
                iretd

        ;Various GUS functions

gus_peek_:      push    EBX
                mov     EBX, EAX
                mov     AL, SET_DRAM_LOW
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                out     DX, AL
                mov     AX, BX
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                out     DX, AX
                mov     AL, SET_DRAM_HIGH
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                out     DX, AL
                shr     EBX, 16
                mov     AL, BL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_HI
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DRAM
                in      AL, DX
                pop     EBX
                ret

gus_poke_:      push    EBX
                push    EDX
                mov     EBX, EAX
                mov     AL, SET_DRAM_LOW
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                out     DX, AL
                mov     AX, BX
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                out     DX, AX
                mov     AL, SET_DRAM_HIGH
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                out     DX, AL
                shr     EBX, 16
                mov     AL, BL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_HI
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DRAM
                pop     EAX
                out     DX, AL
                pop     EBX
                ret

gus_startchannels_:
                push    EBX                             ;This routine starts
                push    ECX                             ;the two channels
                push    EDX                             ;as quickly as possible.
                mov     EBX, _judas_port
                add     EBX, GF1_PAGE
                mov     ECX, _judas_port
                add     ECX, GF1_DATA_HI
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, SET_CONTROL
                out     DX, AL
                test    _judas_mixmode, SIXTEENBIT
                jz      gus_start8
                mov     EDX, EBX
                mov     AL, 0
                out     DX, AL
                mov     EDX, ECX
                mov     AL, VC_LOOP_ENABLE or VC_DATA_TYPE
                out     DX, AL
                mov     EDX, EBX
                mov     AL, 1
                out     DX, AL
                mov     EDX, ECX
                mov     AL, VC_LOOP_ENABLE or VC_DATA_TYPE
                out     DX, AL
                pop     EDX
                pop     ECX
                pop     EBX
                ret
gus_start8:     mov     EDX, EBX
                xor     AL, AL
                out     DX, AL
                mov     EDX, ECX
                mov     AL, VC_LOOP_ENABLE
                out     DX, AL
                mov     EDX, EBX
                mov     AL, 1
                out     DX, AL
                mov     EDX, ECX
                mov     AL, VC_LOOP_ENABLE
                out     DX, AL
                pop     EDX
                pop     ECX
                pop     EBX
                ret

gus_dmaprogram_:
                or      EDX, EDX                        ;Zero length fucks up!
                jz      gus_skipdma
                pushad
                cli
                cmp     gus_dmainprogress, 0            ;Do we have to cache the
                je      gus_dontcache                   ;block?
                mov     EBX, offset gdc
                mov     ECX, CACHESLOTS
gus_seekslot:   cmp     [EBX.GDC_Length], 0
                je      gus_slotfound
                add     EBX, type CACHESLOT
                dec     ECX
                jnz     gus_seekslot
                sti
                popad
gus_skipdma:    ret
gus_slotfound:  mov     [EBX.GDC_Pos], EAX
                mov     [EBX.GDC_Length], EDX
                inc     gus_dmainprogress
                sti
                popad
                ret
gus_dontcache:  sti
                inc     gus_dmainprogress
                mov     ESI, EAX
                mov     EDI, EDX
                mov     EBX, ESI                        ;DMA offset
                shr     EBX, 4
                mov     CL, DMA_ENABLE or DMA_R0 or DMA_TWOS_COMP or DMA_IRQ_ENABLE
                test    _judas_mixmode, SIXTEENBIT
                jz      gus_dma_eight
                mov     CL, DMA_ENABLE or DMA_R0 or DMA_DATA_16 or DMA_IRQ_ENABLE
gus_dma_eight:  cmp     _judas_dma, 4
                jb      gus_nohighdma
                or      CL, DMA_WIDTH_16
                shr     EBX, 1
gus_nohighdma:  mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, SET_DMA_ADDRESS
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                mov     EAX, EBX
                out     DX, AX
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, DMA_CONTROL
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_HI
                mov     AL, CL
                out     DX, AL
                mov     EBX, EDI                        ;DMA length
                mov     EDX, ESI                        ;DMA offset
                mov     EAX, 48h                        ;DMA mode
                call    dma_program_                    ;Program it!
                popad
                ret

gus_dmainit_:   cli
		mov     gus_dmainprogress, 0
		push    EAX EDX
		mov     EDX, _judas_port                ;Acknowledge the DMA
		add     EDX, GF1_REG_SELECT             ;interrupt
		mov     AL, DMA_CONTROL
		out     DX, AL
		mov     EDX, _judas_port
		add     EDX, GF1_DATA_HI
		in      AL, DX
		mov     EAX, offset gdc
diloop:         mov     [EAX.GDC_Pos], 0
		mov     [EAX.GDC_Length], 0
		add     EAX, type CACHESLOT
		cmp     EAX, offset gdc + CACHESLOTS * type CACHESLOT
		jne     diloop
		pop     EDX EAX
		sti
		ret

gus_dmawait_:   mov     EAX, 200000h                    ;Timeout counter
gus_dmawaitloop:cmp     gus_dmainprogress, 0            ;(might time out if
                je      gus_dmadone                     ;there is a DMA
                dec     EAX                             ;conflict.) This routine
                jnz     gus_dmawaitloop                 ;is used just for click
gus_dmadone:    ret                                     ;removal!

gus_getpos:     push    EBX
                push    EDX
                mov     EDX, _judas_port                ;Get the channel
                add     EDX, GF1_PAGE                   ;playing position to
                xor     AL, AL                          ;know where we'll mix
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, GET_ACC_HIGH
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                in      AX, DX
                and     EAX, 8191
                shl     EAX, 7
                mov     EBX, EAX
                mov     EDX, _judas_port
                add     EDX, GF1_REG_SELECT
                mov     AL, GET_ACC_LOW
                out     DX, AL
                mov     EDX, _judas_port
                add     EDX, GF1_DATA_LOW
                in      AX, DX
                shr     AX, 9
                or      EAX, EBX
                test    _judas_mixmode, SIXTEENBIT
                jz      ggp_not16
                shl     EAX, 1
ggp_not16:      pop     EDX
                pop     EBX
                ret

        ;General DMAbuffer update routine (call either from main program or
        ;within your timer interrupt)

judas_update_:  cmp     _judas_device, DEV_NOSOUND
                je      judas_gotohell
                cmp     _judas_device, DEV_FILE
                je      judas_gotohell
                cmp     mix_exec, 0
                jne     judas_gotohell
                cmp     _judas_initialized, 0
                je      judas_gotohell
                inc     mix_exec
                pushad
                cmp     _judas_device, DEV_GUS
                je      updategus                       ;This is a different story
                call    dma_query_
                                                        ;Must be aligned on 8
                and     EAX, _judas_buffermask          ;samples (unrolling!)
                mov     EBX, _judas_mixpos              ;This is the old pos
                cmp     EAX, EBX
                je      judas_donothing
                jb      judas_wrap
judas_normal:   mov     _judas_mixpos, EAX
                mov     EDX, EAX
                sub     EDX, EBX                        ;EDX = length to mix
                mov     EAX, EBX                        ;EAX = pos. to mix
                add     EAX, _dma_address
                call    _judas_mixersys
judas_donothing:popad
                dec     mix_exec
judas_gotohell: inc     _reloj
                ret
judas_wrap:     mov     _judas_mixpos, EAX
                mov     EAX, EBX                        ;Mix to buffer end
                mov     EDX, _judas_bufferlength
                sub     EDX, EBX
                add     EAX, _dma_address
                call    _judas_mixersys
                mov     EAX, _dma_address               ;Then to start
                mov     EDX, _judas_mixpos
                or      EDX, EDX
                jz      judas_donothing
                call    _judas_mixersys
                jmp     judas_donothing

updategus:      cmp     gus_dmainprogress, CACHESLOTS - 4 ;Is there too many
		jb      ug_notstuck                    ;DMA's waiting? I mean,
                call    gus_dmainit_                   ;maybe WIN95 has stuck
                                                       ;the card somehow. In
                                                       ;that case, release all
                                                       ;waiting DMAs manually!
ug_notstuck:    cli
                test    _judas_mixmode, STEREO
                jz      updategus_mono
ipc_s:          xor     EAX, EAX
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                shr     EAX, 1
                call    gus_poke_
                mov     EAX, 1
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                shr     EAX, 1
                inc     EAX
                call    gus_poke_
                mov     EAX, _judas_bufferlength
                shr     EAX, 1
                add     EAX, 32
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                add     EAX, 32
                call    gus_poke_
                mov     EAX, _judas_bufferlength
                shr     EAX, 1
                add     EAX, 33
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                add     EAX, 33
                call    gus_poke_
                mov     EDX, _judas_bufferlength
                shr     EDX, 1
ugs_shitloop:   call    gus_getpos
                mov     EBX, EAX
                call    gus_getpos
                mov     ECX, EAX
                cmp     EBX, EDX
                jae     ugs_shitloop
                mov     EAX, EBX
                sub     EAX, ECX
                cmp     EAX, 64
                jg      ugs_shitloop
                cmp     EAX, -64
                jl      ugs_shitloop
                mov     EAX, EBX
                sti
                and     EAX, _judas_buffermask
                mov     EBX, _judas_mixpos              ;EBX = old mixpos
                cmp     EAX, EBX
                je      judas_donothing
                jb      updategus_wrap
                mov     _judas_mixpos, EAX              ;New "oldpos"
                mov     EDX, EAX
                sub     EDX, EBX                        ;How much to mix
                mov     EAX, EBX                        ;Where to mix
                push    EAX
                push    EDX
                shl     EDX, 1
                add     EAX, _dma_address
                call    _judas_mixersys
                pop     EDX
                pop     EAX
                call    gus_dmaprogram_
                add     EAX, 32
                mov     EBX, _judas_bufferlength
                shr     EBX, 1
                add     EAX, EBX
                call    gus_dmaprogram_
                jmp     judas_donothing
updategus_wrap: mov     _judas_mixpos, EAX              ;Mix first to buffer
                mov     EAX, EBX                        ;end, then to start
                mov     EDX, _judas_bufferlength
                shr     EDX, 1
                sub     EDX, EBX
                push    EAX
                push    EDX
                shl     EDX, 1
                add     EAX, _dma_address
                call    _judas_mixersys
                mov     EAX, _dma_address
                mov     EDX, _judas_mixpos
                shl     EDX, 1
                call    _judas_mixersys
                pop     EDX
                pop     EAX
                call    gus_dmaprogram_
                add     EAX, 32
                mov     EBX, _judas_bufferlength
                shr     EBX, 1
                add     EAX, EBX
                call    gus_dmaprogram_
                xor     EAX, EAX
                mov     EDX, _judas_mixpos
                call    gus_dmaprogram_
                add     EAX, 32
                mov     EBX, _judas_bufferlength
                shr     EBX, 1
                add     EAX, EBX
                call    gus_dmaprogram_
                jmp     judas_donothing

updategus_mono: xor     EAX, EAX
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                call    gus_poke_
                mov     EAX, 1
                call    gus_peek_
                mov     DL, AL
                mov     EAX, _judas_bufferlength
                inc     EAX
                call    gus_poke_
                mov     EDX, _judas_bufferlength
ugm_shitloop:   call    gus_getpos
                mov     EBX, EAX
                call    gus_getpos
                mov     ECX, EAX
                cmp     EBX, EDX
                jae     ugm_shitloop
                mov     EAX, EBX
                sub     EAX, ECX
                cmp     EAX, 64
                jg      ugm_shitloop
                cmp     EAX, -64
                jl      ugm_shitloop
                mov     EAX, EBX
                sti
                and     EAX, _judas_buffermask
                mov     EBX, _judas_mixpos              ;EBX = old mixpos
                cmp     EAX, EBX
                je      judas_donothing
                jb      updategusm_wrap
                mov     _judas_mixpos, EAX              ;New "oldpos"
                mov     EDX, EAX
                sub     EDX, EBX                        ;How much to mix
                mov     EAX, EBX                        ;Where to mix
                push    EAX
                push    EDX
                add     EAX, _dma_address
                call    _judas_mixersys
                pop     EDX
                pop     EAX
                call    gus_dmaprogram_
                jmp     judas_donothing
updategusm_wrap:mov     _judas_mixpos, EAX              ;Mix first to buffer
                mov     EAX, EBX                        ;end
                mov     EDX, _judas_bufferlength
                sub     EDX, EBX
                push    EAX
                push    EDX
                add     EAX, _dma_address
                call    _judas_mixersys
                mov     EAX, _dma_address
                mov     EDX, _judas_mixpos
                call    _judas_mixersys
                pop     EDX
                pop     EAX
                call    gus_dmaprogram_
                xor     EAX, EAX
                mov     EDX, _judas_mixpos
                call    gus_dmaprogram_
                jmp     judas_donothing









;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                 Fast Mixer                                 ³
;³                                                                            ³
;³                                 by Cadaver                                 ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

;First macros...

getvolume       macro
                local   @limit
                xor     EAX, EAX
                mov     AX, [ESI.Chn_Vol]
                cmp     EAX, 64*256
                jbe     @limit
                mov     EAX, 64*256
@limit:         xor     EBX, EBX
                mov     BL, [ESI.Chn_MasterVol]
                imul    EAX, EBX
                shr     EAX, 6+8
                or      AL, AL
                jz      zerovolume
                mov     EBX, EAX
                endm

stereoadjust    macro
                local   @limit
                shl     EBX, 1                          ;Stereo can have 2x vol
                cmp     EBX, 255                        ;Check that volume is
                jbe     @limit                          ;within volumetable
                mov     EBX, 255                        ;limits though
@limit:         endm

smoothpanadjust macro
                local   @limit1, @limit2
                xor     EAX, EAX
                mov     AL, [ESI.Chn_Panning]
                mov     ECX, EBX
                imul    ECX, EAX
                shr     ECX, 7
                neg     EAX
                add     EAX, 256
                imul    EBX, EAX
                shr     EBX, 7
                cmp     EBX, 255
                jbe     @limit1
                mov     EBX, 255
@limit1:        cmp     ECX, 255
                jbe     @limit2
                mov     ECX, 255
@limit2:        endm

mix8_mono_n     macro   n
                mov     BL, [ESI]
                add     EDX, ECX
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 4 * n], EAX
                endm

mix8_left_n     macro   n
                mov     BL, [ESI]
                add     EDX, ECX
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                endm

mix8_middle_n   macro   n
                mov     BL, [ESI]
                add     EDX, ECX
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                add     [EDI + 8 * n + 4], EAX
                endm

mix8_right_n    macro   n
                mov     BL, [ESI]
                add     EDX, ECX
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mix8_smooth_n   macro   n
                mov     BL, [ESI]
                add     EDX, fractadd
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                mov     CL, BL
                add     [EDI + 8 * n], EAX
                mov     EAX, [ECX * 4]
                add     [EDI + 8 * n + 4], EAX
                endm

mix8_mono_i     macro   n
                movsx   EAX, byte ptr [ESI+1]
                movsx   ECX, byte ptr [ESI]
                sub     EAX, ECX
                movzx   ECX, DH
                imul    AX, CX
                mov     BL, AH
                add     BL, [ESI]
                add     DX, word ptr fractadd + 2
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 4 * n], EAX
                endm

mix8_left_i     macro   n
                movsx   EAX, byte ptr [ESI+1]
                movsx   ECX, byte ptr [ESI]
                sub     EAX, ECX
                movzx   ECX, DH
                imul    AX, CX
                mov     BL, AH
                add     BL, [ESI]
                add     DX, word ptr fractadd + 2
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                endm

mix8_middle_i   macro   n
                movsx   EAX, byte ptr [ESI+1]
                movsx   ECX, byte ptr [ESI]
                sub     EAX, ECX
                movzx   ECX, DH
                imul    AX, CX
                mov     BL, AH
                add     BL, [ESI]
                add     DX, word ptr fractadd + 2
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                add     [EDI + 8 * n + 4], EAX
                endm

mix8_right_i    macro   n
                movsx   EAX, byte ptr [ESI+1]
                movsx   ECX, byte ptr [ESI]
                sub     EAX, ECX
                movzx   ECX, DH
                imul    AX, CX
                mov     BL, AH
                add     BL, [ESI]
                add     DX, word ptr fractadd + 2
                mov     EAX, [EBX * 4]
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mix8_smooth_i   macro   n
                movsx   EAX, byte ptr [ESI+1]
                movsx   EBP, byte ptr [ESI]
                sub     EAX, EBP
                movzx   EBP, DH
                imul    AX, BP
                mov     BL, AH
                add     BL, [ESI]
                add     DX, word ptr fractadd + 2
                mov     EAX, [EBX * 4]
                adc     ESI, integeradd
                mov     CL, BL
                add     [EDI + 8 * n], EAX
                mov     EAX, [ECX * 4]
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_mono_n    macro   n
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, EBX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     EDX, ECX
                adc     ESI, EBP
                add     [EDI + 4 * n], EAX
                endm

mix16_left_n    macro   n
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, EBX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     EDX, ECX
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                endm

mix16_middle_n  macro   n
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, EBX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     EDX, ECX
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_right_n   macro   n
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, EBX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     EDX, ECX
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_smooth_n  macro   n
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, EBX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     [EDI + 8 * n], EAX
                movsx   EAX, word ptr [ESI * 2]
                imul    EAX, ECX
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     EDX, fractadd
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_mono_i    macro   n
                movsx   EAX, word ptr [ESI * 2 + 2]
                movsx   ECX, word ptr [ESI * 2]
                sub     EAX, ECX
                movzx   EBX, DH
                imul    EAX, EBX
                sar     EAX, 8
                add     EAX, ECX
                imul    EAX, leftvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     DX, word ptr fractadd + 2
                adc     ESI, EBP
                add     [EDI + 4 * n], EAX
                endm

mix16_left_i    macro   n
                movsx   EAX, word ptr [ESI * 2 + 2]
                movsx   ECX, word ptr [ESI * 2]
                sub     EAX, ECX
                movzx   EBX, DH
                imul    EAX, EBX
                sar     EAX, 8
                add     EAX, ECX
                imul    EAX, leftvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     DX, word ptr fractadd + 2
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                endm

mix16_middle_i  macro   n
                movsx   EAX, word ptr [ESI * 2 + 2]
                movsx   ECX, word ptr [ESI * 2]
                sub     EAX, ECX
                movzx   EBX, DH
                imul    EAX, EBX
                sar     EAX, 8
                add     EAX, ECX
                imul    EAX, leftvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     DX, word ptr fractadd + 2
                adc     ESI, EBP
                add     [EDI + 8 * n], EAX
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_right_i   macro   n
                movsx   EAX, word ptr [ESI * 2 + 2]
                movsx   ECX, word ptr [ESI * 2]
                sub     EAX, ECX
                movzx   EBX, DH
                imul    EAX, EBX
                sar     EAX, 8
                add     EAX, ECX
                imul    EAX, leftvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     DX, word ptr fractadd + 2
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mix16_smooth_i  macro   n
                movsx   EAX, word ptr [ESI * 2 + 2]
                movsx   ECX, word ptr [ESI * 2]
                sub     EAX, ECX
                movzx   EBX, DH
                imul    EAX, EBX
                sar     EAX, 8
                add     EAX, ECX
                mov     EBX, EAX
                imul    EAX, leftvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     [EDI + 8 * n], EAX
                mov     EAX, EBX
                imul    EAX, rightvol
                sar     EAX, 8 + (16 - SIGNIFICANT_BITS_16)
                add     DX, word ptr fractadd + 2
                adc     ESI, EBP
                add     [EDI + 8 * n + 4], EAX
                endm

mixloop8        macro   routine, samplesize, ip
                local   @hitend, @subloop, @oneshot, @offsettable,
                local   @offset0, @offset1, @offset2, @offset3
                local   @offset4, @offset5, @offset6, @offset7
                local   @offset8, @offset9, @offseta, @offsetb
                local   @offsetc, @offsetd, @offsete, @offsetf
                mov     DX, [ESI.Chn_FractPos]          ;Get fractional pos
                if ip eq 0
                shl     EDX, 16
                endif
                mov     EAX, [ESI.Chn_End]              ;Get end & endsubtract-
                mov     smpend, EAX                     ;value
                sub     EAX, [ESI.Chn_Repeat]
                mov     smpsubtract, EAX
                mov     ESI, [ESI.Chn_Pos]              ;Get sample position
                mov     EDI, dptr                       ;Get bufferptr
                mov     EAX, samples                    ;Fix loopcount &
                dec     EAX
                shr     EAX, 4                          ;jumpoffset & subtract
                inc     EAX                             ;EDI accordingly
                mov     loopcount, EAX
                mov     EAX, samples
                and     EAX, 15
                mov     EAX, shittable[EAX * 4]
                if samplesize eq 4
                sub     EDI, EAX
                else
                sub     EDI, EAX
                sub     EDI, EAX
                endif
                add     EAX, offset @offsettable
                jmp     [EAX]

                NOWARN
                align 16
                WARN

@offset0:       routine 0
@offset1:       routine 1
@offset2:       routine 2
@offset3:       routine 3
@offset4:       routine 4
@offset5:       routine 5
@offset6:       routine 6
@offset7:       routine 7
@offset8:       routine 8
@offset9:       routine 9
@offseta:       routine 10
@offsetb:       routine 11
@offsetc:       routine 12
@offsetd:       routine 13
@offsete:       routine 14
@offsetf:       routine 15
                add     EDI, 16 * samplesize
                cmp     ESI, smpend
                jae     @hitend
                dec     loopcount
                jnz     @offset0
                mov     EAX, cptr
                mov     [EAX.Chn_Pos], ESI
                if ip eq 0
                shr     EDX, 16
                endif
                mov     [EAX.Chn_FractPos], DX
                ret
@hitend:        mov     EAX, cptr
                test    [EAX.Chn_VoiceMode], VM_LOOP
                jz      @oneshot
@subloop:       sub     ESI, smpsubtract
                cmp     ESI, smpend
                jae     @subloop
                dec     loopcount
                jnz     @offset0
                mov     [EAX.Chn_Pos], ESI
                if ip eq 0
                shr     EDX, 16
                endif
                mov     [EAX.Chn_FractPos], DX
                ret
@oneshot:       mov     [EAX.Chn_VoiceMode], VM_OFF
                ret
                align   4
@offsettable:   dd      offset @offset0
                dd      offset @offset1
                dd      offset @offset2
                dd      offset @offset3
                dd      offset @offset4
                dd      offset @offset5
                dd      offset @offset6
                dd      offset @offset7
                dd      offset @offset8
                dd      offset @offset9
                dd      offset @offseta
                dd      offset @offsetb
                dd      offset @offsetc
                dd      offset @offsetd
                dd      offset @offsete
                dd      offset @offsetf
                endm

mixloop16       macro   routine, samplesize, ip
                local   @hitend, @subloop, @oneshot, @offsettable,
                local   @offset0, @offset1, @offset2, @offset3
                local   @offset4, @offset5, @offset6, @offset7
                local   @offset8, @offset9, @offseta, @offsetb
                local   @offsetc, @offsetd, @offsete, @offsetf
                mov     DX, [ESI.Chn_FractPos]          ;Get fractional pos
                if ip eq 0
                shl     EDX, 16
                endif
                mov     EAX, [ESI.Chn_End]              ;Get end & endsubtract-
                shr     EAX, 1
                mov     smpend, EAX                     ;value
                mov     EAX, [ESI.Chn_End]
                sub     EAX, [ESI.Chn_Repeat]
                shr     EAX, 1
                mov     smpsubtract, EAX
                mov     ESI, [ESI.Chn_Pos]              ;Get sample position
                shr     ESI, 1
                mov     EDI, dptr                       ;Get bufferptr
                mov     EAX, samples                    ;Fix loopcount &
                dec     EAX
                shr     EAX, 4                          ;jumpoffset & subtract
                inc     EAX                             ;EDI accordingly
                mov     loopcount, EAX
                mov     EAX, samples
                and     EAX, 15
                mov     EAX, shittable[EAX * 4]
                if samplesize eq 4
                sub     EDI, EAX
                else
                sub     EDI, EAX
                sub     EDI, EAX
                endif
                add     EAX, offset @offsettable
                jmp     [EAX]

                NOWARN
                align 16
                WARN

@offset0:       routine 0
@offset1:       routine 1
@offset2:       routine 2
@offset3:       routine 3
@offset4:       routine 4
@offset5:       routine 5
@offset6:       routine 6
@offset7:       routine 7
@offset8:       routine 8
@offset9:       routine 9
@offseta:       routine 10
@offsetb:       routine 11
@offsetc:       routine 12
@offsetd:       routine 13
@offsete:       routine 14
@offsetf:       routine 15
                add     EDI, 16 * samplesize
                cmp     ESI, smpend
                jae     @hitend
                dec     loopcount
                jnz     @offset0
                mov     EAX, cptr
                shl     ESI, 1
                mov     [EAX.Chn_Pos], ESI
                if ip eq 0
                shr     EDX, 16
                endif
                mov     [EAX.Chn_FractPos], DX
                ret
@hitend:        mov     EAX, cptr
                test    [EAX.Chn_VoiceMode], VM_LOOP
                jz      @oneshot
@subloop:       sub     ESI, smpsubtract
                cmp     ESI, smpend
                jae     @subloop
                dec     loopcount
                jnz     @offset0
                shl     ESI, 1
                mov     [EAX.Chn_Pos], ESI
                if ip eq 0
                shr     EDX, 16
                endif
                mov     [EAX.Chn_FractPos], DX
                ret
@oneshot:       mov     [EAX.Chn_VoiceMode], VM_OFF
                ret
                align   4
@offsettable:   dd      offset @offset0
                dd      offset @offset1
                dd      offset @offset2
                dd      offset @offset3
                dd      offset @offset4
                dd      offset @offset5
                dd      offset @offset6
                dd      offset @offset7
                dd      offset @offset8
                dd      offset @offset9
                dd      offset @offseta
                dd      offset @offsetb
                dd      offset @offsetc
                dd      offset @offsetd
                dd      offset @offsete
                dd      offset @offsetf
                endm

        ;16bit fast mixer routines start here!
        ;This is the main mixing routine, which mixes EDX bytes of sound into
        ;address EAX, calling the music player at correct intervals. EDX must
        ;be a multiply of (samplesize * 8) because there is an unrolled
        ;postprocessing loop.
        ;WARNING: This routine destroys every register!

                align   4

fmixer_:        or      EDX, EDX                        ;Check zero length
                jz      mix_quit
                mov     ECX, EDX
                test    _judas_mixmode, STEREO          ;Stereo or mono?
                jz      mix_noshift1
                shr     EDX, 1
mix_noshift1:   test    _judas_mixmode, SIXTEENBIT      ;8- or 16bit?
                jz      mix_noshift2
                shr     EDX, 1
                shr     ECX, 1
mix_noshift2:   mov     samples, EDX                    ;Save number of samples
                mov     totalwork, EDX                  ;"Total work" counter
                mov     fptr, EAX                       ;Save final destination
                shr     ECX, 3
                mov     postproc, ECX                   ;Save clipbuffer size
                mov     EDI, _judas_clipbuffer          ;Clear the clipbuffer
                mov     dptr, EDI
                xor     EAX, EAX
mix_clearloop:  mov     [EDI], EAX
                mov     [EDI + 4], EAX
                mov     [EDI + 8], EAX
                mov     [EDI + 12], EAX
                mov     [EDI + 16], EAX
                mov     [EDI + 20], EAX
                mov     [EDI + 24], EAX
                mov     [EDI + 28], EAX
                add     EDI, 32
                dec     ECX
                jnz     mix_clearloop
                cmp     _judas_player, 0
                jne     mix_hardwayloop
                call    _judas_mixroutine
                jmp     mix_firstphasedone
mix_hardwayloop:cmp     _judas_bpmcount, 0              ;Time to play?
                jne     mix_skipplaying
                cmp     _judas_player, 0                ;Might change in the
                je      mix_fuckshitup                  ;middle of a loop
                call    _judas_player
mix_fuckshitup: mov     EAX, _judas_mixrate
                mov     EBX, 5
                mul     EBX
                shr     EAX, 1
                xor     EDX, EDX
                movzx   EBX, _judas_bpmtempo
                div     EBX
                mov     _judas_bpmcount, EAX
mix_skipplaying:mov     EAX, totalwork
                cmp     EAX, _judas_bpmcount
                jbe     mix_nolimit
                mov     EAX, _judas_bpmcount
mix_nolimit:    mov     samples, EAX
                call    _judas_mixroutine
                mov     EAX, samples
                sub     _judas_bpmcount, EAX
                mov     EBX, EAX
                shl     EBX, 2
                test    _judas_mixmode, STEREO
                jz      mix_noshift3
                shl     EBX, 1
mix_noshift3:   add     dptr, EBX
                sub     totalwork, EAX
                jnz     mix_hardwayloop
mix_firstphasedone:
                test    _judas_mixmode, SIXTEENBIT
                jz      mix_8bit_endphase
mix_16bit_endphase:
                test    _judas_mixmode, STEREO
                jz      mix_nogusshit1
                cmp     _judas_device, DEV_GUS
                je      mix_gus16_endphase
mix_nogusshit1: mov     EDI, fptr
                mov     EBX, postproc
                mov     ESI, _judas_clipbuffer
                mov     ECX, _judas_cliptable
                xor     EAX, EAX
mix_16bit_endphase_loop:
                mov     AX, [ESI]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI], AX
                mov     AX, [ESI + 4]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 2], AX
                mov     AX, [ESI + 8]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 4], AX
                mov     AX, [ESI + 12]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 6], AX
                mov     AX, [ESI + 16]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 8], AX
                mov     AX, [ESI + 20]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 10], AX
                mov     AX, [ESI + 24]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 12], AX
                mov     AX, [ESI + 28]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 14], AX
                add     ESI, 32
                add     EDI, 16
                dec     EBX
                jnz     mix_16bit_endphase_loop
mix_quit:       ret
mix_8bit_endphase:
                test    _judas_mixmode, STEREO
                jz      mix_nogusshit2
                cmp     _judas_device, DEV_GUS
                je      mix_gus8_endphase
mix_nogusshit2: mov     EDI, fptr
                mov     EBX, postproc
                mov     ESI, _judas_clipbuffer
                mov     ECX, _judas_cliptable
                xor     EAX, EAX
mix_8bit_endphase_loop:
                mov     AX, [ESI]
                mov     AL, [ECX + EAX]
                mov     [EDI], AL
                mov     AX, [ESI + 4]
                mov     AL, [ECX + EAX]
                mov     [EDI + 1], AL
                mov     AX, [ESI + 8]
                mov     AL, [ECX + EAX]
                mov     [EDI + 2], AL
                mov     AX, [ESI + 12]
                mov     AL, [ECX + EAX]
                mov     [EDI + 3], AL
                mov     AX, [ESI + 16]
                mov     AL, [ECX + EAX]
                mov     [EDI + 4], AL
                mov     AX, [ESI + 20]
                mov     AL, [ECX + EAX]
                mov     [EDI + 5], AL
                mov     AX, [ESI + 24]
                mov     AL, [ECX + EAX]
                mov     [EDI + 6], AL
                mov     AX, [ESI + 28]
                mov     AL, [ECX + EAX]
                mov     [EDI + 7], AL
                add     ESI, 32
                add     EDI, 8
                dec     EBX
                jnz     mix_8bit_endphase_loop
                jmp     mix_quit
mix_gus16_endphase:
                mov     EDI, fptr
                mov     EBX, postproc
                mov     EDX, _judas_bufferlength
                shr     EDX, 1
                add     EDX, EDI
                add     EDX, 32
                mov     ESI, _judas_clipbuffer
                mov     ECX, _judas_cliptable
                xor     EAX, EAX
mix_gus16_endphase_loop:
                mov     AX, [ESI]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI], AX
                mov     AX, [ESI + 4]
                mov     AX, [ECX + EAX * 2]
                mov     [EDX], AX
                mov     AX, [ESI + 8]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 2], AX
                mov     AX, [ESI + 12]
                mov     AX, [ECX + EAX * 2]
                mov     [EDX + 2], AX
                mov     AX, [ESI + 16]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 4], AX
                mov     AX, [ESI + 20]
                mov     AX, [ECX + EAX * 2]
                mov     [EDX + 4], AX
                mov     AX, [ESI + 24]
                mov     AX, [ECX + EAX * 2]
                mov     [EDI + 6], AX
                mov     AX, [ESI + 28]
                mov     AX, [ECX + EAX * 2]
                mov     [EDX + 6], AX
                add     ESI, 32
                add     EDI, 8
                add     EDX, 8
                dec     EBX
                jnz     mix_gus16_endphase_loop
                jmp     mix_quit
mix_gus8_endphase:
                mov     EDI, fptr
                mov     EBX, postproc
                mov     EDX, _judas_bufferlength
                shr     EDX, 1
                add     EDX, EDI
                add     EDX, 32
                mov     ESI, _judas_clipbuffer
                mov     ECX, _judas_cliptable
                xor     EAX, EAX
mix_gus8_endphase_loop:
                mov     AX, [ESI]
                mov     AL, [ECX + EAX]
                mov     [EDI], AX
                mov     AX, [ESI + 4]
                mov     AL, [ECX + EAX]
                mov     [EDX], AX
                mov     AX, [ESI + 8]
                mov     AL, [ECX + EAX]
                mov     [EDI + 1], AX
                mov     AX, [ESI + 12]
                mov     AL, [ECX + EAX]
                mov     [EDX + 1], AX
                mov     AX, [ESI + 16]
                mov     AX, [ECX + EAX]
                mov     [EDI + 2], AX
                mov     AX, [ESI + 20]
                mov     AL, [ECX + EAX]
                mov     [EDX + 2], AX
                mov     AX, [ESI + 24]
                mov     AL, [ECX + EAX]
                mov     [EDI + 3], AX
                mov     AX, [ESI + 28]
                mov     AL, [ECX + EAX]
                mov     [EDX + 3], AX
                add     ESI, 32
                add     EDI, 4
                add     EDX, 4
                dec     EBX
                jnz     mix_gus8_endphase_loop
                jmp     mix_quit

normalmix_:     mov     cptr, offset _judas_channel
normalmixloop:  call    mixchannel
                add     cptr, type CHANNEL
                cmp     cptr, offset _judas_channel + CHANNELS * type CHANNEL
                jne     normalmixloop
                ret

ipmix_:         mov     cptr, offset _judas_channel
ipmixloop:      call    ipmixchannel
                add     cptr, type CHANNEL
                cmp     cptr, offset _judas_channel + CHANNELS * type CHANNEL
                jne     ipmixloop
                ret

        ;Mixes [samples] of channel [cptr] to buffer at [dptr]. Destroys
        ;every register.

mixchannel:     mov     ESI, cptr
                test    [ESI.Chn_VoiceMode], VM_ON
                jz      mixchannel_quit
                mov     EAX, [ESI.Chn_Freq]             ;Get playing speed here
                cmp     EAX, 535232                     ;Highest linear freq
                jbe     mixchannel_freqok
                mov     EAX, 535232
mixchannel_freqok:
                mov     EDX, EAX                        ;Don't worry: overflow
                shr     EDX, 16                         ;prevented by check
                shl     EAX, 16                         ;above
                div     _judas_mixrate                  ;DIV is always
                mov     word ptr fractadd + 2, AX       ;frightening!!!
                shr     EAX, 16
                mov     integeradd, EAX
                test    [ESI.Chn_VoiceMode], VM_16BIT   ;16bit takes the branch
                jnz     mixchannel16                    ;because it's unusual
                test    _judas_mixmode, STEREO          ;Mono takes the branch
                jz      mixchannel_mono                 ;because it's faster
mixchannel_stereo:
                getvolume
                cmp     [ESI.Chn_Panning], 0            ;Left panning?
                jne     mc8_notleft
                stereoadjust
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mov     ECX, fractadd                   ;ECX = fraction add
                mixloop8 mix8_left_n, 8, 0              ;DO IT!
mc8_notleft:    cmp     [ESI.Chn_Panning], 128          ;Middle panning?
                jne     mc8_notmiddle
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mov     ECX, fractadd                   ;ECX = fraction add
                mixloop8 mix8_middle_n, 8, 0            ;DO IT!
mc8_notmiddle:  cmp     [ESI.Chn_Panning], 255          ;Right panning?
                jne     mc8_notright
                stereoadjust
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mov     ECX, fractadd                   ;ECX = fraction add
                mixloop8 mix8_right_n, 8, 0             ;DO IT!
mc8_notright:   smoothpanadjust                         ;Oh no, smooth panning!
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                shl     ECX, 8
                add     ECX, _judas_volumetable
                mov     EBP, integeradd                 ;ECX not available!
                mixloop8 mix8_smooth_n, 8, 0            ;But yet we must do it..
mixchannel_mono:getvolume
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mov     ECX, fractadd                   ;ECX = fraction add
                mixloop8 mix8_mono_n, 4, 0              ;DO IT!
mixchannel_quit:ret
mixchannel16:   test    _judas_mixmode, STEREO          ;Mono takes the branch
                jz      mixchannel16_mono               ;because it's faster
mixchannel16_stereo:
                getvolume
                cmp     [ESI.Chn_Panning], 0            ;Left panning?
                jne     mc16_notleft
                stereoadjust
                mov     ECX, fractadd                   ;ECX = fraction add
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_left_n, 8, 0            ;DO IT!
mc16_notleft:   cmp     [ESI.Chn_Panning], 128          ;Middle panning?
                jne     mc16_notmiddle
                mov     ECX, fractadd                   ;ECX = fraction add
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_middle_n, 8, 0          ;DO IT!
mc16_notmiddle: cmp     [ESI.Chn_Panning], 255          ;Right panning?
                jne     mc16_notright
                stereoadjust
                mov     ECX, fractadd                   ;ECX = fraction add
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_right_n, 8, 0           ;DO IT!
mc16_notright:  smoothpanadjust                         ;Oh no, smooth panning!
                mov     EBP, integeradd
                mixloop16 mix16_smooth_n, 8, 0          ;But yet we must do it..
mixchannel16_mono:
                getvolume
                mov     ECX, fractadd                   ;ECX = fraction add
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_mono_n, 4, 0            ;DO IT!
mixchannel16_quit:
                ret

        ;Mixes [samples] of channel [cptr] to buffer at [dptr] with
        ;interpolation. Destroys every register.

ipmixchannel:   mov     ESI, cptr
                test    [ESI.Chn_VoiceMode], VM_ON
                jz      ipmixchannel_quit
                mov     EAX, [ESI.Chn_Freq]             ;Get playing speed here
                cmp     EAX, 535232                     ;Highest linear freq
                jbe     ipmixchannel_freqok
                mov     EAX, 535232
ipmixchannel_freqok:
                mov     EDX, EAX
                shr     EDX, 16
                shl     EAX, 16
                div     _judas_mixrate
                mov     word ptr fractadd + 2, AX
                shr     EAX, 16
                mov     integeradd, EAX
                test    [ESI.Chn_VoiceMode], VM_16BIT   ;16bit takes the branch
                jnz     ipmixchannel16                  ;because it's unusual
                test    _judas_mixmode, STEREO          ;Mono takes the branch
                jz      ipmixchannel_mono               ;because it's faster
ipmixchannel_stereo:
                getvolume
                cmp     [ESI.Chn_Panning], 0            ;Left panning?
                jne     imc8_notleft
                stereoadjust
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop8 mix8_left_i, 8, 1              ;DO IT!
imc8_notleft:   cmp     [ESI.Chn_Panning], 128          ;Middle panning?
                jne     imc8_notmiddle
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop8 mix8_middle_i, 8, 1            ;DO IT!
imc8_notmiddle: cmp     [ESI.Chn_Panning], 255          ;Right panning?
                jne     imc8_notright
                stereoadjust
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop8 mix8_right_i, 8, 1             ;DO IT!
imc8_notright:  smoothpanadjust                         ;Oh no, smooth panning!
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                shl     ECX, 8
                add     ECX, _judas_volumetable
                mixloop8 mix8_smooth_i, 8, 1
ipmixchannel_mono:getvolume
                shl     EBX, 8                          ;Convert to volumetable
                add     EBX, _judas_volumetable         ;ofs.
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop8 mix8_mono_i, 4, 1              ;DO IT!
ipmixchannel_quit:ret
ipmixchannel16: test    _judas_mixmode, STEREO          ;Mono takes the branch
                jz      ipmixchannel16_mono             ;because it's faster
ipmixchannel16_stereo:
                getvolume
                cmp     [ESI.Chn_Panning], 0            ;Left panning?
                jne     imc16_notleft
                stereoadjust
                mov     leftvol, EBX
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_left_i, 8, 1            ;DO IT!
imc16_notleft:  cmp     [ESI.Chn_Panning], 128          ;Middle panning?
                jne     imc16_notmiddle
                mov     leftvol, EBX
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_middle_i, 8, 1          ;DO IT!
imc16_notmiddle:cmp     [ESI.Chn_Panning], 255          ;Right panning?
                jne     imc16_notright
                stereoadjust
                mov     leftvol, EBX
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_right_i, 8, 1           ;DO IT!
imc16_notright: smoothpanadjust                         ;Oh no, smooth panning!
                mov     leftvol, EBX
                mov     rightvol, ECX
                mov     EBP, integeradd
                mixloop16 mix16_smooth_i, 8, 1          ;But yet we must do it..
ipmixchannel16_mono:
                getvolume
                mov     leftvol, EBX
                mov     EBP, integeradd                 ;EBP = integeradd
                mixloop16 mix16_mono_i, 4, 1            ;DO IT!
ipmixchannel16_quit:
                ret

zerovolume:     mov     EBP, samples
                mov     EBX, [ESI.Chn_Pos]
                mov     ECX, EBX
                shr     EBX, 16
                shl     ECX, 16
                mov     CX, [ESI.Chn_FractPos]
                mov     EAX, [ESI.Chn_Freq]
                mov     EDX, EAX
                shr     EDX, 16
                shl     EAX, 16
                div     _judas_mixrate                  ;EAX = mixrate
                test    [ESI.Chn_VoiceMode], VM_16BIT   ;If 16bit then double
                jz      zerovolume_not16bit             ;the count
                shl     EBP, 1
zerovolume_not16bit:
                mul     EBP                             ;EDX:EAX = pos. add
                add     ECX, EAX                        ;Add low part
                adc     EBX, EDX                        ;Add high part
                mov     [ESI.Chn_FractPos], CX          ;Save fractpos
                shl     EBX, 16                         ;(won't change now)
                shr     ECX, 16                         ;Now shift back: ECX
                or      ECX, EBX                        ;is integer pos
                test    [ESI.Chn_VoiceMode], VM_16BIT   ;Final adjust for 16bit
                jz      zerovolume_no16bitadjust
                and     ECX, 0fffffffeh
zerovolume_no16bitadjust:
                test    [ESI.Chn_VoiceMode], VM_LOOP    ;Is it looped?
                jnz     zerovolume_looped
                cmp     ECX, [ESI.Chn_End]
                jae     zerovolume_oneshot_end
zerovolume_ready:
                mov     [ESI.Chn_Pos], ECX              ;Store pos
                ret
zerovolume_oneshot_end:
                mov     [ESI.Chn_VoiceMode], VM_OFF
                ret
zerovolume_looped:
                mov     EAX, [ESI.Chn_End]
                sub     EAX, [ESI.Chn_Repeat]           ;EAX = subtract value
zerovolume_looped_loop:
                cmp     ECX, [ESI.Chn_End]
                jb      zerovolume_ready
                sub     ECX, EAX
                jmp     zerovolume_looped_loop









;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                               Quality Mixer                                ³
;³                                                                            ³
;³                                  by Yehar                                  ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
;32 bit mixing with 30 significant bits. 16 bit interpolation for both 8 and 16
;bit samples. Cubic (8192x) and linear (32768x) interpolation aka oversampling.
;14 bit volume. Zero-level reposition & volume change smoothing click removal.
;
;Don't try to read for educational purposes =)))) It's just too shitty & lazy!

        ;32bit quality mixer main routine starts here. This is separated from
        ;the usual mixer because of different postprocessing etc. This is an
        ;alternative main mixing routine, which mixes EDX bytes of sound into
        ;address EAX, calling the music player at correct intervals.
        ;WARNING: This routine destroys every register!

qmixer_:
                test    EDX, EDX        ;Buffer size is zero -> nothing to do
                jz      qmixer_quit
                test    _judas_mixmode, SIXTEENBIT      ;16/8bit? 16 means half
                jz      qmixer_noshift2                 ;the number of samples.
                shr     EDX, 1                  ;It is 16 bit!
qmixer_noshift2:
                test    _judas_mixmode, STEREO  ;Stereo/mono? Stereo means half
                jz      qmixer_noshift1         ;the number of samples.
                shr     EDX, 1                  ;It is stereo!
qmixer_noshift1:
                mov     fptr, EAX       ;Save pointer to final buffer.
                mov     samples, EDX    ;Save number of samples.
                mov     totalwork, EDX  ;Save "total samples left" counter.
                mov     postproc, EDX   ;Number of samples to postprocess
                mov     EDI, _judas_clipbuffer
                mov     dptr, EDI       ;Dptr says to mixroutine: "Mix to clipbuffer!"
                mov     EBX, _judas_zladdbuffer
                xor     EAX, EAX
                mov     ECX, EDX
                shl     ECX, 1
qmixer_clearloop:                       ;Clear clipbuffer & zeroleveladdbuffer
                mov     [EDI], EAX
                mov     [EBX], EAX
                add     EDI, 4
                add     EBX, 4
                dec     ECX
                jnz     qmixer_clearloop

                ;Player calling shit...

                cmp     _judas_player, 0        ;Is there a player?
                jne     qmixer_hardwayloop
                call    _judas_mixroutine       ;No, just mix the sfx and
                jmp     qmixer_firstphasedone   ;    postprocess
qmixer_hardwayloop:
                cmp     _judas_bpmcount, 0      ;Yes, check if it has to be
                jne     qmixer_skipplaying      ;     called.
                cmp     _judas_player, 0        ;It has to, but player might
                je      qmixer_fuckshitup       ;have disappeared.
                call    _judas_player           ;Call if it hasn't.
qmixer_fuckshitup:
                mov     EAX, _judas_mixrate
                mov     EBP, 5
                mul     EBP
                shr     EAX, 1
                xor     EDX, EDX
                movzx   EBP, _judas_bpmtempo
                div     EBP
                mov     _judas_bpmcount, EAX
qmixer_skipplaying:
                mov     EAX, totalwork
                cmp     EAX, _judas_bpmcount
                jbe     qmixer_nolimit
                mov     EAX, _judas_bpmcount
qmixer_nolimit:
                mov     samples, EAX
                call    _judas_mixroutine
                mov     EAX, samples
                sub     _judas_bpmcount, EAX
                mov     EBP, EAX
                shl     EBP, 3
                add     dptr, EBP
                sub     totalwork, EAX
                jnz     qmixer_hardwayloop

                ;Postprocessing...

qmixer_firstphasedone:
                test    _judas_mixmode, SIXTEENBIT
                jz      qmixer_8bit_endphase
qmixer_16bit_endphase:
                test    _judas_mixmode, STEREO
                jz      qmixer_nogusshit1
                cmp     _judas_device, DEV_GUS
                je      qmixer_gus16s_endphase
                ;stereo
                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr
qmixer_s16bit_endphase_loop:
                ;left
                mov     EAX, [ESI]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevell
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 16    ;Shrink to 16 bit
                cmp     EAX, 32767
                jg      qmixer_s16bit_overflowl
                cmp     EAX, -32768
                jl      qmixer_s16bit_underflowl
                mov     [EDI], AX
qmixer_s16bit_cutdonel:

                ;right
                mov     EAX, [ESI+4]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevelr
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 16    ;Shrink to 16 bit
                cmp     EAX, 32767
                jg      qmixer_s16bit_overflowr
                cmp     EAX, -32768
                jl      qmixer_s16bit_underflowr
                mov     [EDI+2], AX
qmixer_s16bit_cutdoner:

                add     ESI, 8
                add     EBX, 8
                add     EDI, 4
                dec     postproc
                jnz     qmixer_s16bit_endphase_loop
                ret
qmixer_s16bit_overflowl:
                mov     word ptr [EDI], 32767
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_s16bit_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_s16bit_cutdonel
qmixer_s16bit_underflowl:
                mov     word ptr [EDI], -32768
                neg     EAX
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_s16bit_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_s16bit_cutdonel
qmixer_s16bit_overflowr:
                mov     word ptr [EDI+2], 32767
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_s16bit_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_s16bit_cutdoner
qmixer_s16bit_underflowr:
                mov     word ptr [EDI+2], -32768
                neg     EAX
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_s16bit_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_s16bit_cutdoner
qmixer_nogusshit1: ;mono 16bit
                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr
qmixer_m16bit_endphase_loop:

                mov     EAX, [ESI]              ;Get value from clipbuffer
                sar     EAX, 1
                mov     ECX, [ESI+4] ; + right
                sar     ECX, 1
                add     EAX, ECX

                mov     ECX, _judas_zerolevell ;left zerolevel
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                sar     ECX, 1
                add     EAX, ECX
                mov     ECX, _judas_zerolevelr ;right zerolevel
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                sar     ECX, 1
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 16    ;Shrink to 16 bit
                cmp     EAX, 32767
                jg      qmixer_m16bit_overflow
                cmp     EAX, -32768
                jl      qmixer_m16bit_underflow
                mov     [EDI], AX
qmixer_m16bit_cutdone:

                add     ESI, 8
                add     EBX, 8
                add     EDI, 2
                dec     postproc
                jnz     qmixer_m16bit_endphase_loop
                ret
qmixer_m16bit_overflow:
                mov     word ptr [EDI], 32767
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_m16bit_cutdone
                mov     _judas_clipped, AL
                jmp     qmixer_m16bit_cutdone
qmixer_m16bit_underflow:
                mov     word ptr [EDI], -32768
                neg     EAX
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_m16bit_cutdone
                mov     _judas_clipped, AL
                jmp     qmixer_m16bit_cutdone

qmixer_8bit_endphase:
                test    _judas_mixmode, STEREO
                jz      qmixer_nogusshit2
                cmp     _judas_device, DEV_GUS
                je      qmixer_gus8s_endphase
                ;stereo
                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr
qmixer_s8bit_endphase_loop:
                ;left
                mov     EAX, [ESI]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevell
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 8    ;Shrink to 8 bit
                cmp     EAX, 127
                jg      qmixer_s8bit_overflowl
                cmp     EAX, -128
                jl      qmixer_s8bit_underflowl
                add     AL, 128
                mov     byte ptr [EDI], AL
qmixer_s8bit_cutdonel:

                ;right
                mov     EAX, [ESI+4]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevelr
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 8    ;Shrink to 8 bit
                cmp     EAX, 127
                jg      qmixer_s8bit_overflowr
                cmp     EAX, -128
                jl      qmixer_s8bit_underflowr
                add     AL, 128
                mov     byte ptr [EDI+1], AL
qmixer_s8bit_cutdoner:

                add     ESI, 8
                add     EBX, 8
                add     EDI, 2
                dec     postproc
                jnz     qmixer_s8bit_endphase_loop
                ret
qmixer_s8bit_overflowl:
                mov     byte ptr [EDI], 255
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_s8bit_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_s8bit_cutdonel
qmixer_s8bit_underflowl:
                mov     byte ptr [EDI], 0
                neg     EAX
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_s8bit_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_s8bit_cutdonel
qmixer_s8bit_overflowr:
                mov     byte ptr [EDI+1], 255
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_s8bit_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_s8bit_cutdoner
qmixer_s8bit_underflowr:
                mov     byte ptr [EDI+1], 0
                neg     EAX
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_s8bit_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_s8bit_cutdoner
qmixer_nogusshit2: ;mono 8bit

                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr
qmixer_m8bit_endphase_loop:

                mov     EAX, [ESI]              ;Get value from clipbuffer
                sar     EAX, 1
                mov     ECX, [ESI+4] ; + right
                sar     ECX, 1
                add     EAX, ECX

                mov     ECX, _judas_zerolevell ;left zerolevel
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                sar     ECX, 1
                add     EAX, ECX
                mov     ECX, _judas_zerolevelr ;right zerolevel
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                sar     ECX, 1
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 8    ;Shrink to 8 bit
                cmp     EAX, 127
                jg      qmixer_m8bit_overflow
                cmp     EAX, -128
                jl      qmixer_m8bit_underflow
                add     AL, 128
                mov     byte ptr [EDI], AL
qmixer_m8bit_cutdone:

                add     ESI, 8
                add     EBX, 8
                inc     EDI
                dec     postproc
                jnz     qmixer_m8bit_endphase_loop
                ret
qmixer_m8bit_overflow:
                mov     byte ptr [EDI], 255
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_m8bit_cutdone
                mov     _judas_clipped, AL
                jmp     qmixer_m8bit_cutdone
qmixer_m8bit_underflow:
                mov     byte ptr [EDI], 0
                neg     EAX
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_m8bit_cutdone
                mov     _judas_clipped, AL
                jmp     qmixer_m8bit_cutdone

qmixer_gus16s_endphase:
                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr                       ;[EDI] = gus left
                mov     EDX, _judas_bufferlength        
                shr     EDX, 1
                add     EDX, EDI
                add     EDX, 32                         ;[EDX] = gus right

qmixer_gus16s_endphase_loop:
                ;left
                mov     EAX, [ESI]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevell
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 16    ;Shrink to 16 bit
                cmp     EAX, 32767
                jg      qmixer_gus16s_overflowl
                cmp     EAX, -32768
                jl      qmixer_gus16s_underflowl
                mov     [EDI], AX
qmixer_gus16s_cutdonel:

                ;right
                mov     EAX, [ESI+4]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevelr
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 16    ;Shrink to 16 bit
                cmp     EAX, 32767
                jg      qmixer_gus16s_overflowr
                cmp     EAX, -32768
                jl      qmixer_gus16s_underflowr
                mov     [EDX], AX
qmixer_gus16s_cutdoner:

                add     ESI, 8
                add     EBX, 8
                add     EDI, 2
                add     EDX, 2
                dec     postproc
                jnz     qmixer_gus16s_endphase_loop
                ret
qmixer_gus16s_overflowl:
                mov     word ptr [EDI], 32767
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_gus16s_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_gus16s_cutdonel
qmixer_gus16s_underflowl:
                mov     word ptr [EDI], -32768
                neg     EAX
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_gus16s_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_gus16s_cutdonel
qmixer_gus16s_overflowr:
                mov     word ptr [EDX], 32767
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_gus16s_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_gus16s_cutdoner
qmixer_gus16s_underflowr:
                mov     word ptr [EDX], -32768
                neg     EAX
                shr     EAX, 9
                cmp     _judas_clipped, AL
                jae     qmixer_gus16s_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_gus16s_cutdoner

qmixer_gus8s_endphase:
                mov     ESI, _judas_clipbuffer
                mov     EBX, _judas_zladdbuffer
                mov     EDI, fptr                       ;[EDI] = gus left
                mov     EDX, _judas_bufferlength        
                shr     EDX, 1
                add     EDX, EDI
                add     EDX, 32                         ;[EDX] = gus right
qmixer_gus8s_endphase_loop:
                ;left
                mov     EAX, [ESI]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevell
                add     ECX, [EBX]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevell, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 8    ;Shrink to 8 bit
                cmp     EAX, 127
                jg      qmixer_gus8s_overflowl
                cmp     EAX, -128
                jl      qmixer_gus8s_underflowl
                add     AL, 128
                mov     byte ptr [EDI], AL
qmixer_gus8s_cutdonel:

                ;right
                mov     EAX, [ESI+4]              ;Get value from clipbuffer
                mov     ECX, _judas_zerolevelr
                add     ECX, [EBX+4]      ;Add from zladdbuffer to zerolevel
                mov     EBP, ECX ;zerolevel gets 1/2048 closer to real zero
                sar     EBP, QMIXER_ZEROLEVELDECAY
                sub     ECX, EBP
                mov     _judas_zerolevelr, ECX
                add     EAX, ECX

                sar     EAX, SIGNIFICANT_BITS_32 - 8    ;Shrink to 8 bit
                cmp     EAX, 127
                jg      qmixer_gus8s_overflowr
                cmp     EAX, -128
                jl      qmixer_gus8s_underflowr
                add     AL, 128
                mov     byte ptr [EDX], AL
qmixer_gus8s_cutdoner:

                add     ESI, 8
                add     EBX, 8
                inc     EDI
                inc     EDX
                dec     postproc
                jnz     qmixer_gus8s_endphase_loop
                ret
qmixer_gus8s_overflowl:
                mov     byte ptr [EDI], 255
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_gus8s_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_gus8s_cutdonel
qmixer_gus8s_underflowl:
                mov     byte ptr [EDI], 0
                neg     EAX
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_gus8s_cutdonel
                mov     _judas_clipped, AL
                jmp     qmixer_gus8s_cutdonel
qmixer_gus8s_overflowr:
                mov     byte ptr [EDX], 255
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_gus8s_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_gus8s_cutdoner
qmixer_gus8s_underflowr:
                mov     byte ptr [EDX], 0
                neg     EAX
                shr     EAX, 1
                cmp     _judas_clipped, AL
                jae     qmixer_gus8s_cutdoner
                mov     _judas_clipped, AL
                jmp     qmixer_gus8s_cutdoner
qmixer_quit:    ret

qmix_cubic_:
                mov     cptr, offset _judas_channel
qmix_cubic_channelloop:
                call    qmixchannel_cubic
                add     cptr, type CHANNEL
                cmp     cptr, offset _judas_channel + CHANNELS * type CHANNEL
                jne     qmix_cubic_channelloop
                ret

qmix_linear_:
                mov     cptr, offset _judas_channel
qmix_linear_channelloop:
                call    qmixchannel_linear
                add     cptr, type CHANNEL
                cmp     cptr, offset _judas_channel + CHANNELS * type CHANNEL
                jne     qmix_cubic_channelloop
                ret

q_zerovolume:
                ; Zerolevel stuff...

                mov     EAX, dptr               ;Get correct place at zladdbuf
                sub     EAX, _judas_clipbuffer  ;
                add     EAX, _judas_zladdbuffer ;
                mov     EDX, [EBX.Chn_LastValL]
                add     [EAX], EDX
                mov     EDX, [EBX.Chn_LastValR]
                mov     [EBX.Chn_LastValL], 0
                add     [EAX+4], EDX
                mov     [EBX.Chn_LastValR], 0

                mov     ESI, samples
                mov     EBP, [EBX.Chn_Pos]
                mov     ECX, EBP
                shr     EBP, 16
                shl     ECX, 16
                mov     CX, [EBX.Chn_FractPos]
                mov     EAX, [EBX.Chn_Freq]
                mov     EDX, EAX
                shr     EDX, 16
                shl     EAX, 16
                div     _judas_mixrate                  ;EAX = mixrate
                test    [EBX.Chn_VoiceMode], VM_16BIT   ;If 16bit then double
                jz      q_zerovolume_not16bit           ;the count
                shl     ESI, 1
q_zerovolume_not16bit:
                mul     ESI                             ;EDX:EAX = pos. add
                add     ECX, EAX                        ;Add low part
                adc     EBP, EDX                        ;Add high part
                mov     [EBX.Chn_FractPos], CX          ;Save fractpos
                shl     EBP, 16                         ;(won't change now)
                shr     ECX, 16                         ;Now shift back: ECX
                or      ECX, EBP                        ;is integer pos
                test    [EBX.Chn_VoiceMode], VM_16BIT   ;Final adjust for 16bit
                jz      q_zerovolume_no16bitadjust
                and     ECX, 0fffffffeh
q_zerovolume_no16bitadjust:
                test    [EBX.Chn_VoiceMode], VM_LOOP    ;Is it looped?
                jnz     q_zerovolume_looped
                cmp     ECX, [EBX.Chn_End]
                jae     q_zerovolume_oneshot_end
q_zerovolume_ready:
                mov     [EBX.Chn_Pos], ECX              ;Store pos
                ret
q_zerovolume_oneshot_end:
                mov     [EBX.Chn_VoiceMode], VM_OFF
                ret
q_zerovolume_looped:
                mov     EAX, [EBX.Chn_End]
                sub     EAX, [EBX.Chn_Repeat]           ;EAX = subtract value
q_zerovolume_looped_loop:
                cmp     ECX, [EBX.Chn_End]
                jb      q_zerovolume_ready
                sub     ECX, EAX
                jmp     q_zerovolume_looped_loop

        ;MACROS FOR QUALITY MIXER!

q_volpan14      macro
                local   @limit
                ;Volume!
                xor     EAX, EAX
                mov     AX, [EBX.Chn_Vol]
                xor     ECX, ECX
                mov     CL, [EBX.Chn_MasterVol]
                imul    EAX, ECX
                shr     EAX, 8
                mov     EBP, EAX                 ;14 bit volume in AX
                or      AX, word ptr [EBX.Chn_SmoothVolL+1]
                or      AX, word ptr [EBX.Chn_SmoothVolR+1]
                jz      q_zerovolume
                ;Panning!
                xor     EAX, EAX
                mov     AL, [EBX.Chn_Panning]
                mov     ECX, EBP                ;EBP = leftvol, ECX = rightvol
                imul    ECX, EAX
                neg     EAX      ;EAX = 255-EAX
                add     EAX, 255 ;
                imul    EBP, EAX
                endm

q_mix8_stereo_l macro
                movsx   EAX, byte ptr [ESI]                                ;-
                movsx   EBP, byte ptr [ESI+1]                              ;-
                mov     EBX, rightvol                 ;Smooth volume slide ;uv\
                sub     EBP, EAX        ;EBP = 16bit slidevalue            ;uv \
                mov     EDX, ECX        ;EDX = fractpos                    ;uv\
                sub     EBX, SmoothVolR               ;                    ;uv \
                shr     EDX, 17         ;32768x interpolation              ;u \
                add     EDI, 8                                             ;uv \
                imul    EBP, EDX        ;EBP = interpolated value          ;-
                sar     EBX, QMIXER_VOLUMESMOOTH      ;1/32 closer         ;u \
                mov     EDX, leftvol                    ;Smooth volume sli.;uv \
                add     EBX, SmoothVolR               ;                    ;uv\
                sub     EDX, SmoothVolL                 ;                  ;uv \
                sal     EAX, 8                                             ;u
                sar     EDX, QMIXER_VOLUMESMOOTH        ;1/32 closer       ;u \
                mov     SmoothVolR, EBX               ;                    ;uv \
                shr     EBX, 7                        ;                    ;u \
                add     EDX, SmoothVolL                 ;                  ;uv \
                sar     EBP, 7                                             ;u \
                mov     SmoothVolL, EDX                 ;                  ;uv \
                shr     EDX, 7                          ;                  ;u \
                add     EBP, EAX                                           ;uv \

                add     ECX, fractadd        ;Sample pos fractional part   ;uv\
                mov     EAX, EBP                                           ;uv \
                adc     ESI, integeradd      ;Sample pos integer part      ;u \

                imul    EBP, EBX                                           ;-

                imul    EAX, EDX                                           ;-

                add     [EDI-8], EAX                                       ;uv\
                add     [EDI-8+4], EBP                                     ;uv \

                endm

q_mix8_stereo_c macro   ;Here starts the pervert Hermite interpolation!!!

                movsx   EBP, byte ptr [ESI+IPMINUS1]
                movsx   EDX, byte ptr [ESI+IP1]
                movsx   EBX, byte ptr [ESI+IP2]
                movsx   EAX, byte ptr [ESI+IP0]
                sal     EBX, 8                                  ;u
                sal     EDX, 8                                  ;u \
                mov     ip2, EBX                                ;uv \
                sal     EAX, 8                                  ;u \
                mov     ip1, EDX                                ;uv \
                mov     EBX, EAX                                ;uv\
                sub     EAX, EDX                                ;uv \
                sal     EBP, 8                                  ;u

                mov     ipminus1, EBP                           ;uv\
                lea     EAX, [EAX*4+EDX]                        ;uv \
                mov     EDX, ECX                                ;uv\
                sub     EAX, EBX                                ;uv \
                shr     EDX, 19         ;8192x interpolation    ;u \
                sub     EAX, EBP                                ;uv \
                add     EAX, ip2                                ;uv\
                lea     EBP, [EBX*4+EBX]                        ;uv \

                imul    EAX, EDX                                ;-

                sar     EAX, 32-19+1                            ;u \
                add     EBP, ip2                                ;uv \
                sar     EBP, 1                                  ;u \
                add     EAX, ip1                                ;uv \
                add     EAX, ip1                                ;uv\
                add     EDI, 8                                  ;uv \
                sub     EAX, EBP                                ;uv\
                mov     EBP, ip1                                ;uv \
                add     EAX, ipminus1                           ;uv\
                sub     EBP, ipminus1                           ;uv \

                imul    EAX, EDX                                ;-

                sar     EBP, 1                                  ;u \
                sar     EAX, 32-19                              ;u \
                add     ECX, fractadd   ;Sample pos fract.      ;uv \
                adc     ESI, integeradd ;Sample pos integer     ;u \
                add     EAX, EBP                                ;uv \

                imul    EAX, EDX                                ;-

                sar     EAX, 32-19                                      ;u \
                mov     EDX, leftvol                    ;Smooth vol.sl. ;uv \
                add     EAX, EBX                                        ;uv\
                mov     EBX, rightvol                 ;Smooth vol.sl.   ;uv \
                sub     EDX, SmoothVolL                 ;               ;uv\
                sub     EBX, SmoothVolR               ;                 ;uv \
                sar     EBX, QMIXER_VOLUMESMOOTH      ;1/32 closer      ;u \
                add     EBX, SmoothVolR               ;                 ;uv \
                sar     EDX, QMIXER_VOLUMESMOOTH        ;1/32 closer    ;u \
                mov     SmoothVolR, EBX               ;                 ;uv \
                shr     EBX, 7                        ;                 ;u \
                add     EDX, SmoothVolL                 ;               ;uv \
                mov     SmoothVolL, EDX                 ;               ;uv\
                mov     EBP, EAX                                        ;uv \
                shr     EDX, 7                          ;               ;u

                imul    EAX, EDX

                imul    EBP, EBX

                add     [EDI-8], EAX                    ;uv\
                add     [EDI-8+4], EBP                  ;uv \

                endm

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;pos adds are already defined. EBP contains volume for left channel
;and ECX for right.
q_mixloop8      macro routine
                local   @mixloop, @hitend, @subloop, @oneshot, @donezladjust, @oneshot_lastsample, @soonmixloop
                mov     leftvol, EBP    ;Save volumes
                mov     rightvol, ECX   ;

                mov     ESI, [EBX.Chn_Pos]      ;ESI:ECX = pos. ESI is integer
                mov     CX, [EBX.Chn_FractPos]  ;part and ECX fraction.
                shl     ECX, 16

                mov     EAX, [EBX.Chn_End]      ;Get sample end and repeat
                mov     smpend, EAX             ;positions. Calculate subtract
                sub     EAX, [EBX.Chn_Repeat]   ;value.
                mov     smpsubtract, EAX

                mov     EDI, dptr       ;Where to mix to?
                mov     EAX, samples    ;How many samples to mix?
                mov     loopcount, EAX

                ;Click removing for volume change and sample replace

                mov     EAX, [EBX.Chn_Pos]      ;Is it a new sound?
                cmp     EAX, [EBX.Chn_PrevPos]
                je      @soonmixloop            ;jump if not
;New sound
                mov     EDX, leftvol
IFDEF QMIXER_STARTMUTE
                shr     EDX, QMIXER_STARTMUTE ;Just to remove low frequency clicks
ENDIF
                mov     SmoothVolL, EDX
                mov     EDX, rightvol
IFDEF QMIXER_STARTMUTE
                shr     EDX, QMIXER_STARTMUTE
ENDIF
                mov     SmoothVolR, EDX
                mov     saved_reg, EBX
                routine
                mov     EBX, saved_reg
                mov     EDX, [EBX.Chn_LastValL] 
                mov     saved_reg, EAX
                sub     EDX, EAX
                mov     EAX, _judas_zladdbuffer ;Get correct place at zladdbuf
                sub     EAX, _judas_clipbuffer  ;
                add     [EAX+EDI-8], EDX
                mov     EDX, [EBX.Chn_LastValR]
                sub     EDX, EBP
                add     [EAX+EDI-8+4], EDX
                mov     EAX, saved_reg ;Maybe needed if sample ends now
                mov     saved_reg, EBX
                jmp     @donezladjust

@soonmixloop:   mov     EDX, [EBX.Chn_SmoothVolL]
                mov     EBP, [EBX.Chn_SmoothVolR]
                mov     SmoothVolL, EDX
                mov     SmoothVolR, EBP
                mov     saved_reg, EBX

                NOWARN
                align 16
                WARN

@mixloop:       routine                         ;Mix one sample
@donezladjust:
                cmp     ESI, smpend     ;End of sample?
                jae     @hitend

                dec     loopcount       ;Still shit to do?
                jnz     @mixloop

                mov     EBX, saved_reg
                mov     [EBX.Chn_Pos], ESI      ;No, all done. Save position
                mov     [EBX.Chn_PrevPos], ESI  ;..and prevpos
                shr     ECX, 16
                mov     [EBX.Chn_FractPos], CX
                mov     [EBX.Chn_LastValL], EAX ;Save Last values also
                mov     [EBX.Chn_LastValR], EBP ;
                mov     EAX, SmoothVolL           ;Save volume
                mov     EBP, SmoothVolR           ;
                mov     [EBX.Chn_SmoothVolL], EAX ;
                mov     [EBX.Chn_SmoothVolR], EBP ;
                ret

@hitend:
                mov     EBX, saved_reg
                test    [EBX.Chn_VoiceMode], VM_LOOP    ;Is it a looped sample?
                jz      @oneshot
@subloop:
                sub     ESI, smpsubtract        ;Looped. Go to loop start.
                cmp     ESI, smpend
                jae     @subloop  ;Fucking shit, it got far beyond sample end.

                dec     loopcount       ;Still shit to do?
                jnz     @mixloop

                mov     [EBX.Chn_Pos], ESI      ;No, all done. Save position
                shr     ECX, 16
                mov     [EBX.Chn_FractPos], CX
                mov     [EBX.Chn_PrevPos], ESI  ;Save prevpos too
                mov     [EBX.Chn_LastValL], EAX ;...Last values also
                mov     [EBX.Chn_LastValR], EBP ;
                mov     EAX, SmoothVolL           ;Save volume
                mov     EBP, SmoothVolR           ;
                mov     [EBX.Chn_SmoothVolL], EAX ;
                mov     [EBX.Chn_SmoothVolR], EBP ;
                ret

@oneshot:
                mov     [EBX.Chn_VoiceMode], VM_OFF ;Your time to die, sample!

                ;If sample doesn't end at zero, zerolevel must be adjusted.
                mov     [EBX.Chn_Pos], 0
                mov     [EBX.Chn_LastValL], 0
                mov     [EBX.Chn_LastValR], 0
                cmp     loopcount, 1
                jbe     @oneshot_lastsample
                mov     EDX, EDI                ;Get correct place in zladdbuf
                sub     EDX, _judas_clipbuffer  ;
                add     EDX, _judas_zladdbuffer ;
                add     [EDX], EAX
                add     [EDX+4], EBP
                mov     [EBX.Chn_PrevVM], VM_OFF ;No need to update zerolevel
                ret                              ;at beginning of next chunk

@oneshot_lastsample:                            ;Last sample in mixed chunk
                mov     [EBX.Chn_PrevVM], VM_ON
                ret
                endm

q_mix16_stereo_l macro
                movsx   EAX, word ptr [ESI*2]                              ;-
                movsx   EBP, word ptr [ESI*2+2]                            ;-
                mov     EBX, rightvol                 ;Smooth volume slide ;uv\
                sub     EBP, EAX        ;EBP = 16bit slidevalue            ;uv \
                mov     EDX, ECX        ;EDX = fractpos                    ;uv\
                sub     EBX, SmoothVolR               ;                    ;uv \
                shr     EDX, 17         ;32768x interpolation              ;u \
                add     EDI, 8                                             ;uv \
                imul    EBP, EDX        ;EBP = interpolated value          ;-
                sar     EBX, QMIXER_VOLUMESMOOTH      ;1/32 closer         ;u \
                mov     EDX, leftvol                    ;Smooth volume sli.;uv \
                add     EBX, SmoothVolR               ;                    ;uv\
                sub     EDX, SmoothVolL                 ;                  ;uv \
                sar     EDX, QMIXER_VOLUMESMOOTH        ;1/32 closer       ;u \
                mov     SmoothVolR, EBX               ;                    ;uv \
                shr     EBX, 7                        ;                    ;u \
                add     EDX, SmoothVolL                 ;                  ;uv \
                sar     EBP, 15                                            ;u \
                mov     SmoothVolL, EDX                 ;                  ;uv \
                shr     EDX, 7                          ;                  ;u \
                add     EBP, EAX                                           ;uv \

                add     ECX, fractadd        ;Sample pos fractional part   ;uv\
                mov     EAX, EBP                                           ;uv \
                adc     ESI, integeradd      ;Sample pos integer part      ;u \

                imul    EBP, EBX                                           ;-

                imul    EAX, EDX                                           ;-

                add     [EDI-8], EAX                                       ;uv\
                add     [EDI-8+4], EBP                                     ;uv \

                endm

q_mix16_stereo_c macro  ;Here starts the pervert Hermite interpolation!!!
                movsx   EBP, word ptr [ESI*2+IPMINUS1*2]
                movsx   EAX, word ptr [ESI*2+IP0*2]
                movsx   EBX, word ptr [ESI*2+IP2*2]
                movsx   EDX, word ptr [ESI*2+IP1*2]
                mov     ip2, EBX                                ;uv\
                mov     ip1, EDX                                ;uv \
                mov     EBX, EAX                                ;uv\

                sub     EAX, EDX                                ;uv \
                mov     ipminus1, EBP                           ;uv\
                lea     EAX, [EAX*4+EDX]                        ;uv \
                mov     EDX, ECX                                ;uv\
                sub     EAX, EBX                                ;uv \
                shr     EDX, 19         ;8192x interpolation    ;u \
                sub     EAX, EBP                                ;uv \
                add     EAX, ip2                                ;uv\
                lea     EBP, [EBX*4+EBX]                        ;uv \

                imul    EAX, EDX                                ;-

                sar     EAX, 32-19+1                            ;u \
                add     EBP, ip2                                ;uv \
                sar     EBP, 1                                  ;u \
                add     EAX, ip1                                ;uv \
                add     EAX, ip1                                ;uv\
                add     EDI, 8                                  ;uv \
                sub     EAX, EBP                                ;uv\
                mov     EBP, ip1                                ;uv \
                add     EAX, ipminus1                           ;uv\
                sub     EBP, ipminus1                           ;uv \

                imul    EAX, EDX                                ;-

                sar     EBP, 1                                  ;u \
                sar     EAX, 32-19                              ;u \
                add     ECX, fractadd   ;Sample pos fract.      ;uv \
                adc     ESI, integeradd ;Sample pos integer     ;u \
                add     EAX, EBP                                ;uv \

                imul    EAX, EDX                                ;-

                sar     EAX, 32-19                                      ;u \
                mov     EDX, leftvol                    ;Smooth vol.sl. ;uv \
                add     EAX, EBX                                        ;uv\
                mov     EBX, rightvol                 ;Smooth vol.sl.   ;uv \
                sub     EDX, SmoothVolL                 ;               ;uv\
                sub     EBX, SmoothVolR               ;                 ;uv \
                sar     EBX, QMIXER_VOLUMESMOOTH      ;1/32 closer      ;u \
                add     EBX, SmoothVolR               ;                 ;uv \
                sar     EDX, QMIXER_VOLUMESMOOTH        ;1/32 closer    ;u \
                mov     SmoothVolR, EBX               ;                 ;uv \
                shr     EBX, 7                        ;                 ;u \
                add     EDX, SmoothVolL                 ;               ;uv \
                mov     SmoothVolL, EDX                 ;               ;uv\
                mov     EBP, EAX                                        ;uv \
                shr     EDX, 7                          ;               ;u

                imul    EAX, EDX

                imul    EBP, EBX

                add     [EDI-8], EAX                    ;uv\
                add     [EDI-8+4], EBP                  ;uv \

                endm

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;pos adds are already defined. EBP contains volume for left channel
;and ECX for right.
q_mixloop16     macro routine
                local   @mixloop, @hitend, @subloop, @oneshot, @donezladjust, @oneshot_lastsample, @soonmixloop
                mov     leftvol, EBP    ;Save volumes
                mov     rightvol, ECX   ;

                mov     ESI, [EBX.Chn_Pos]      ;ESI:ECX = pos. ESI is integer
                shr     ESI, 1
                mov     CX, [EBX.Chn_FractPos]  ;part and ECX fraction.
                shl     ECX, 16

                mov     EAX, [EBX.Chn_End]      ;Get sample end and repeat
                shr     EAX, 1
                mov     smpend, EAX             ;positions. Calculate subtract
                mov     EAX, [EBX.Chn_End]
                sub     EAX, [EBX.Chn_Repeat]   ;value.
                shr     EAX, 1
                mov     smpsubtract, EAX

                mov     EDI, dptr       ;Where to mix to?
                mov     EAX, samples    ;How many samples to mix?
                mov     loopcount, EAX

                ;Click removing for volume change and sample replace

                mov     EAX, [EBX.Chn_Pos]      ;Is it a new sound?
                cmp     EAX, [EBX.Chn_PrevPos]
                je      @soonmixloop            ;jump if not
;New sound
                mov     EDX, leftvol
IFDEF QMIXER_STARTMUTE
                shr     EDX, QMIXER_STARTMUTE ;Just to remove low frequency clicks
ENDIF
                mov     SmoothVolL, EDX
                mov     EDX, rightvol
IFDEF QMIXER_STARTMUTE
                shr     EDX, QMIXER_STARTMUTE
ENDIF
                mov     SmoothVolR, EDX
                mov     saved_reg, EBX
                routine
                mov     EBX, saved_reg
                mov     EDX, [EBX.Chn_LastValL]
                mov     saved_reg, EAX
                sub     EDX, EAX
                mov     EAX, _judas_zladdbuffer ;Get correct place at zladdbuf
                sub     EAX, _judas_clipbuffer  ;
                add     [EAX+EDI-8], EDX
                mov     EDX, [EBX.Chn_LastValR]
                sub     EDX, EBP
                add     [EAX+EDI-8+4], EDX
                mov     EAX, saved_reg ;Maybe needed if sample ends now
                mov     saved_reg, EBX
                jmp     @donezladjust

@soonmixloop:   mov     EDX, [EBX.Chn_SmoothVolL]
                mov     EBP, [EBX.Chn_SmoothVolR]
                mov     SmoothVolL, EDX
                mov     SmoothVolR, EBP
                mov     saved_reg, EBX

                NOWARN
                align 16
                WARN

@mixloop:       routine                         ;Mix one sample
@donezladjust:
                cmp     ESI, smpend     ;End of sample?
                jae     @hitend

                dec     loopcount       ;Still shit to do?
                jnz     @mixloop

                mov     EBX, saved_reg
                shl     ESI, 1
                mov     [EBX.Chn_Pos], ESI      ;No, all done. Save position
                mov     [EBX.Chn_PrevPos], ESI  ;...and prevpos
                shr     ECX, 16
                mov     [EBX.Chn_FractPos], CX
                mov     [EBX.Chn_LastValL], EAX ;Save Last values also
                mov     [EBX.Chn_LastValR], EBP ;
                mov     EAX, SmoothVolL           ;Save volume
                mov     EBP, SmoothVolR           ;
                mov     [EBX.Chn_SmoothVolL], EAX ;
                mov     [EBX.Chn_SmoothVolR], EBP ;
                ret

@hitend:
                mov     EBX, saved_reg
                test    [EBX.Chn_VoiceMode], VM_LOOP    ;Is it a looped sample?
                jz      @oneshot
@subloop:
                sub     ESI, smpsubtract        ;Looped. Go to loop start.
                cmp     ESI, smpend
                jae     @subloop  ;Fucking shit, it got far beyond sample end.

                dec     loopcount       ;Still shit to do?
                jnz     @mixloop

                shl     ESI, 1
                mov     [EBX.Chn_Pos], ESI      ;No, all done. Save position
                mov     [EBX.Chn_PrevPos], ESI  ;Save prevpos too
                shr     ECX, 16
                mov     [EBX.Chn_FractPos], CX
                mov     [EBX.Chn_LastValL], EAX ;...Last values also
                mov     [EBX.Chn_LastValR], EBP ;
                mov     EAX, SmoothVolL           ;Save volume
                mov     EBP, SmoothVolR           ;
                mov     [EBX.Chn_SmoothVolL], EAX ;
                mov     [EBX.Chn_SmoothVolR], EBP ;
                ret

@oneshot:
                mov     [EBX.Chn_VoiceMode], VM_OFF ;Your time to die, sample!

                ;If sample doesn't end at zero, zerolevel must be adjusted.
                mov     [EBX.Chn_Pos], 0
                mov     [EBX.Chn_LastValL], 0
                mov     [EBX.Chn_LastValR], 0
                cmp     loopcount, 1
                jbe     @oneshot_lastsample
                mov     EDX, EDI                ;Get correct place in zladdbuf
                sub     EDX, _judas_clipbuffer  ;
                add     EDX, _judas_zladdbuffer ;
                add     [EDX], EAX
                add     [EDX+4], EBP
                mov     [EBX.Chn_PrevVM], VM_OFF ;No need to update zerolevel
                ret                              ;at beginning of next chunk

@oneshot_lastsample:                            ;Last sample in mixed chunk
                mov     [EBX.Chn_PrevVM], VM_ON
                ret
                endm

        ;Qualitymixes [samples] of channel [cptr] to buffer at [dptr]. Destroys
        ;every register. LINEAR INTERPOLATION!

qmixchannel_linear:
                mov     EBX, cptr
                mov     AL, [EBX.Chn_VoiceMode]
                test    AL, VM_ON
                jnz     qmixc_l_vm_on
                cmp     AL, [EBX.Chn_PrevVM]            ;Sound discontinuity?
                je      qmixc_l_quit                    ;no?
                mov     [EBX.Chn_PrevVM], AL            ;yes...
                mov     EDX, _judas_zladdbuffer         ;so must move zerolevel
                add     EDX, dptr
                mov     EAX, [EBX.Chn_LastValL]
                sub     EDX, _judas_clipbuffer
                mov     [EBX.Chn_LastValL], 0
                add     [EDX], EAX
                mov     EAX, [EBX.Chn_LastValR]
                mov     [EBX.Chn_LastValR], 0
                add     [EDX+4], EAX
                jmp     qmixc_l_quit
qmixc_l_vm_on:
                mov     EAX, [EBX.Chn_Freq]
                cmp     EAX, 535232                     ;Highest linear freq
                jbe     qmixc_l_freqok
                mov     EAX, 535232
qmixc_l_freqok:
                mov     EDX, EAX
                shr     EDX, 16
                shl     EAX, 16
                div     _judas_mixrate
                mov     word ptr fractadd + 2, AX
                shr     EAX, 16
                mov     integeradd, EAX
                test    [EBX.Chn_VoiceMode], VM_16BIT   ;Is sampledata 16bit?
                jnz     qmixc_l_16bit                     ;Jump if yes.
                test    _judas_mixmode, STEREO          ;No, 8bit. Stereo audio?
                jz      qmixc_l_mono_8bit                 ;Jump if mono.
qmixc_l_stereo_8bit:
qmixc_l_mono_8bit:        ;Mix in stereo, even if mono output
                q_volpan14
                q_mixloop8 q_mix8_stereo_l
                ret
qmixc_l_16bit:
                test    _judas_mixmode, STEREO          ;It's 16bit. Stereo?
                jz      qmixc_l_mono_16bit
qmixc_l_stereo_16bit:
qmixc_l_mono_16bit:        ;Mix in stereo, even if mono output
                q_volpan14
                q_mixloop16 q_mix16_stereo_l
qmixc_l_quit:     ret

        ;Qualitymixes [samples] of channel [cptr] to buffer at [dptr]. Destroys
        ;every register. CUBIC INTERPOLATION!

qmixchannel_cubic:
                mov     EBX, cptr
                mov     AL, [EBX.Chn_VoiceMode]
                test    AL, VM_ON
                jnz     qmixc_c_vm_on
                cmp     AL, [EBX.Chn_PrevVM]            ;Sound discontinuity?
                je      qmixc_c_quit                    ;no?
                mov     [EBX.Chn_PrevVM], AL            ;yes...
                mov     EDX, _judas_zladdbuffer         ;so must move zerolevel
                add     EDX, dptr
                mov     EAX, [EBX.Chn_LastValL]
                sub     EDX, _judas_clipbuffer
                mov     [EBX.Chn_LastValL], 0
                add     [EDX], EAX
                mov     EAX, [EBX.Chn_LastValR]
                mov     [EBX.Chn_LastValR], 0
                add     [EDX+4], EAX
                jmp     qmixc_c_quit
qmixc_c_vm_on:
                mov     [EBX.Chn_PrevVM], AL
                mov     EAX, [EBX.Chn_Freq]
                cmp     EAX, 535232                     ;Highest linear freq
                jbe     qmixc_c_freqok
                mov     EAX, 535232
qmixc_c_freqok:
                mov     EDX, EAX
                shr     EDX, 16
                shl     EAX, 16
                div     _judas_mixrate
                mov     word ptr fractadd + 2, AX
                shr     EAX, 16
                mov     integeradd, EAX
                test    [EBX.Chn_VoiceMode], VM_16BIT   ;Is sampledata 16bit?
                jnz     qmixc_c_16bit                     ;Jump if yes.
                test    _judas_mixmode, STEREO          ;No, 8bit. Stereo audio?
                jz      qmixc_c_mono_8bit                 ;Jump if mono.
qmixc_c_stereo_8bit:
qmixc_c_mono_8bit:        ;Mix in stereo, even if mono output
                q_volpan14
                q_mixloop8 q_mix8_stereo_c
                ret
qmixc_c_16bit:
                test    _judas_mixmode, STEREO          ;It's 16bit. Stereo?
                jz      qmixc_c_mono_16bit
qmixc_c_stereo_16bit:
qmixc_c_mono_16bit:        ;Mix in stereo, even if mono output
                q_volpan14
                q_mixloop16 q_mix16_stereo_c
qmixc_c_quit:     ret

;Safety mixer for calls from c program

safemixer_:     cmp     _judas_mixersys, 0
                je      safemixer_helldontcall
                cmp     _judas_initialized, 0
                je      safemixer_helldontcall
                pushad
                call    _judas_mixersys
                popad
safemixer_helldontcall:
                ret

judas_code_lock_end_:
                end
