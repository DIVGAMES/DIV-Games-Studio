; Ugly shit from GUS SDK

 GF1_MIDI_CTRL          = 100h  ; 3X0
 GF1_MIDI_DATA          = 101h  ; 3X1
 GF1_PAGE               = 102h  ; 3X2
 GF1_REG_SELECT         = 103h  ; 3X3
 GF1_DATA_LOW           = 104h  ; 3X4
 GF1_DATA_HI            = 105h  ; 3X5
 GF1_IRQ_STAT           = 006h  ; 2X6
 GF1_DRAM               = 107h  ; 3X7
 GF1_MIX_CTRL           = 000h  ; 2X0
 GF1_TIMER_CTRL         = 008h  ; 2X8
 GF1_TIMER_DATA         = 009h  ; 2X9
 GF1_IRQ_CTRL           = 00Bh  ; 2XB

; The GF1 Hardware clock.
 CLOCK_RATE             = 9878400

; Mixer control bits.
 ENABLE_LINE            = 01h
 ENABLE_DAC             = 02h
 ENABLE_MIC             = 04h

 DMA_CONTROL            = 41h
 SET_DMA_ADDRESS        = 42h
 SET_DRAM_LOW           = 43h
 SET_DRAM_HIGH          = 44h
 TIMER_CONTROL          = 45h
 TIMER1                 = 46h
 TIMER2                 = 47h
 SET_SAMPLE_RATE        = 48h
 SAMPLE_CONTROL         = 49h
 SET_JOYSTICK           = 4Bh
 MASTER_RESET           = 4Ch

; Voice register mapping.
 SET_CONTROL            = 00h
 SET_FREQUENCY          = 01h
 SET_START_HIGH         = 02h
 SET_START_LOW          = 03h
 SET_END_HIGH           = 04h
 SET_END_LOW            = 05h
 SET_VOLUME_RATE        = 06h
 SET_VOLUME_START       = 07h
 SET_VOLUME_END         = 08h
 SET_VOLUME             = 09h
 SET_ACC_HIGH           = 0ah
 SET_ACC_LOW            = 0bh
 SET_BALANCE            = 0ch
 SET_VOLUME_CONTROL     = 0dh
 SET_VOICES             = 0eh

 GET_CONTROL            = 80h
 GET_FREQUENCY          = 81h
 GET_START_HIGH         = 82h
 GET_START_LOW          = 83h
 GET_END_HIGH           = 84h
 GET_END_LOW            = 85h
 GET_VOLUME_RATE        = 86h
 GET_VOLUME_START       = 87h
 GET_VOLUME_END         = 88h
 GET_VOLUME             = 89h
 GET_ACC_HIGH           = 8ah
 GET_ACC_LOW            = 8bh
 GET_BALANCE            = 8ch
 GET_VOLUME_CONTROL     = 8dh
 GET_VOICES             = 8eh
 GET_IRQV               = 8fh

 MIDI_RESET	            = 3h

;*******************************************************************
;*
;* GF1 irq/dma programmable latches
;*
;******************************************************************

; GF1_IRQ_STATUS (port 3X6)
 MIDI_TX_IRQ            = 01h  ; pending MIDI xmit IRQ
 MIDI_RX_IRQ            = 02h  ; pending MIDI recv IRQ
 GF1_TIMER1_IRQ         = 04h  ; general purpose timer
 GF1_TIMER2_IRQ         = 08h  ; general purpose timer
 WAVETABLE_IRQ          = 20h  ; pending wavetable IRQ
 ENVELOPE_IRQ           = 40h  ; pending volume envelope IRQ
 DMA_TC_IRQ             = 80h  ; pending dma tc IRQ


; GF1_MIX_CTRL (port 2X0)
 ENABLE_LINE_IN         = 01h  ; 0=enable
 ENABLE_OUTPUT          = 02h  ; 0=enable
 ENABLE_MIC_IN          = 04h  ; 1=enable
 ENABLE_GF1_IRQ         = 08h  ; 1=enable
 GF122                  = 10  ; ??
 ENABLE_MIDI_LOOP       = 20h  ; 1=enable loop back
 SELECT_GF1_REG         = 40h  ; 1=irq latches
          ; 0=dma latches

;*******************************************************************
;*
;* GF1 global registers ($41-$4C)
;*
;*******************************************************************

; DMA control register
 DMA_ENABLE             = 01h
 DMA_READ               = 02h  ; 1=read,0=write
 DMA_WIDTH_16           = 04h  ; 1=16 bit,0=8 bit (dma chan width)
 DMA_RATE               = 18h  ; 00=fast, 11=slow
 DMA_IRQ_ENABLE         = 20h  ; 1=enable
 DMA_IRQ_PENDING        = 40h  ; read
 DMA_DATA_16            = 40h  ; write (data width)
 DMA_TWOS_COMP          = 80h  ; 1=do twos comp

; These are the xfer rate bits ...
 DMA_R0  = 00h  ; Fastest DMA xfer (~650khz)
 DMA_R1  = 08h  ; fastest / 2
 DMA_R2  = 10h  ; fastest / 4
 DMA_R3  = 18h  ; Slowest DMA xfer (fastest / 8)

; SAMPLE control register
 ENABLE_ADC             = 01h
 ADC_MODE               = 02h  ; 0=mono, 1=stereo
 ADC_DMA_WIDTH          = 04h  ; 0=8 bit, 1=16 bit
 ADC_IRQ_ENABLE         = 20h  ; 1=enable
 ADC_IRQ_PENDING        = 40h  ; 1=irq pending
 ADC_TWOS_COMP          = 80h  ; 1=do twos comp

; RESET control register
 GF1_MASTER_RESET        = 01h  ; 0=hold in reset
 GF1_OUTPUT_ENABLE      = 02h  ; enable output
 GF1_MASTER_IRQ         = 04h  ; master IRQ enable

;*******************************************************************
;*
;* GF1 voice specific registers ($00 - $0E and $80-$8f)
;*
;******************************************************************

; ($0,$80) Voice control register
 VOICE_STOPPED          = 01h  ; voice has stopped
 STOP_VOICE             = 02h  ; stop voice
 VC_DATA_TYPE           = 04h  ; 0=8 bit,1=16 bit
 VC_LOOP_ENABLE         = 08h  ; 1=enable
 VC_BI_LOOP             = 10h  ; 1=bi directional looping
 VC_WAVE_IRQ            = 20h  ; 1=enable voice's wave irq
 VC_DIRECT              = 40h  ; 0=increasing,1=decreasing
 VC_IRQ_PENDING         = 80h  ; 1=wavetable irq pending

; ($1,$81) Frequency control
; Bit 0  - Unused
; Bits 1-9 - Fractional portion
; Bits 10-15 - Integer portion

; ($2,$82) Accumulator start address (high)
; Bits 0-11 - HIGH 12 bits of address
; Bits 12-15 - Unused

; ($3,$83) Accumulator start address (low)
; Bits 0-4 - Unused
; Bits 5-8 - Fractional portion
; Bits 9-15 - Low 7 bits of integer portion

; ($4,$84) Accumulator end address (high)
; Bits 0-11 - HIGH 12 bits of address
; Bits 12-15 - Unused

; ($5,$85) Accumulator end address (low)
; Bits 0-4 - Unused
; Bits 5-8 - Fractional portion
; Bits 9-15 - Low 7 bits of integer portion


; ($6,$86) Volume Envelope control register
 VL_RATE_MANTISSA       = 3fh
 VL_RATE_RANGE          = 0C0h

; ($7,$87) Volume envelope start
 VL_START_MANT          = 0Fh
 VL_START_EXP           = 0F0h

; ($8,$88) Volume envelope end
 VL_END_MANT            = 0Fh
 VL_END_EXP             = 0F0h

; ($9,$89) Current volume register
; Bits 0-3 are unused
; Bits 4-11 - Mantissa of current volume
; Bits 10-15 - Exponent of current volume

; ($A,$8A) Accumulator value (high)
; Bits 0-12 - HIGH 13 bits of current position (a19-a7)

; ($B,$8B) Accumulator value (low)
; Bits 0-8 - Fractional portion
; Bits 9-15 - Integer portion of low adress (a6-a0)

; ($C,$8C) Pan (balance) position
; Bits 0-3 - Balance position  0=full left, = 0f=full right

; ($D,$8D) Volume control register
 VOLUME_STOPPED         = 01h  ; volume has stopped
 STOP_VOLUME            = 02h  ; stop volume
 VC_ROLLOVER            = 04h  ; Roll PAST end & gen IRQ
 VL_LOOP_ENABLE         = 08h  ; 1=enable
 VL_BI_LOOP             = 10h  ; 1=bi directional looping
 VL_WAVE_IRQ            = 20h  ; 1=enable voice's wave irq
 VL_DIRECT              = 40h  ; 0=increasing,1=decreasing
 VL_IRQ_PENDING         = 80h  ; 1=wavetable irq pending

; ($E,$8E) # of Active voices
; Bits 0-5 - # of active voices -1

; ($F,$8F) - Sources of IRQs
; Bits 0-4 - interrupting voice number
; Bit 5 - Always a 1
 VOICE_VOLUME_IRQ       = 40h  ; individual voice irq bit
 VOICE_WAVE_IRQ         = 80h  ; individual waveform irq bit

