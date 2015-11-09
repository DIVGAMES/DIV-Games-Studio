/*
 * Internal header file: Ultrasound shit from the SDK
 */
#define JOYSTICK_TIMER	0x201		/* 201 */
#define JOYSTICK_DATA	0x201		/* 201 */
#define GF1_MIDI_CTRL	0x100		/* 3X0 */
#define GF1_MIDI_DATA	0x101		/* 3X1 */

#define	GF1_PAGE	0x102		/* 3X2 */
#define GF1_REG_SELECT	0x103		/* 3X3 */
#define GF1_DATA_LOW	0x104		/* 3X4 */
#define GF1_DATA_HI	0x105		/* 3X5 */
#define GF1_IRQ_STAT	0x006		/* 2X6 */
#define GF1_DRAM	0x107		/* 3X7 */
#define	GF1_MIX_CTRL	0x000		/* 2X0 */
#define GF1_TIMER_CTRL	0x008		/* 2X8 */
#define GF1_TIMER_DATA	0x009		/* 2X9 */
#define GF1_IRQ_CTRL	0x00B		/* 2XB */

/* The GF1 Hardware clock. */
#define CLOCK_RATE		9878400L

/* Mixer control bits. */
#define	ENABLE_LINE		0x01
#define	ENABLE_DAC		0x02
#define	ENABLE_MIC		0x04

/* interrupt controller 1 */
#define CNTRL_8259		0x21
#define OCR_8259		0x20
#define EOI			0x20
#define REARM3			0x2F3
#define REARM5			0x2F5

/* interrupt controller 2 */
#define CNTRL_M_8259	0x21
#define CNTRL_M2_8259	0xA1
#define OCR_2_8259		0xA0
#define	DMA_CONTROL		0x41
#define	SET_DMA_ADDRESS		0x42
#define SET_DRAM_LOW		0x43
#define SET_DRAM_HIGH		0x44

#define TIMER_CONTROL		0x45
#define TIMER1			0x46
#define TIMER2			0x47

#define	SET_SAMPLE_RATE		0x48
#define	SAMPLE_CONTROL		0x49

#define SET_JOYSTICK		0x4B
#define MASTER_RESET		0x4C

/* Voice register mapping. */
#define	SET_CONTROL		0x00
#define	SET_FREQUENCY		0x01
#define	SET_START_HIGH		0x02
#define	SET_START_LOW		0x03
#define	SET_END_HIGH		0x04
#define	SET_END_LOW		0x05
#define	SET_VOLUME_RATE		0x06
#define	SET_VOLUME_START	0x07
#define	SET_VOLUME_END		0x08
#define	SET_VOLUME		0x09
#define	SET_ACC_HIGH		0x0a
#define	SET_ACC_LOW		0x0b
#define	SET_BALANCE		0x0c
#define	SET_VOLUME_CONTROL 	0x0d
#define	SET_VOICES		0x0e

#define	GET_CONTROL		0x80
#define	GET_FREQUENCY		0x81
#define	GET_START_HIGH		0x82
#define	GET_START_LOW		0x83
#define	GET_END_HIGH		0x84
#define	GET_END_LOW		0x85
#define	GET_VOLUME_RATE		0x86
#define	GET_VOLUME_START	0x87
#define	GET_VOLUME_END		0x88
#define	GET_VOLUME		0x89
#define	GET_ACC_HIGH		0x8a
#define	GET_ACC_LOW		0x8b
#define	GET_BALANCE		0x8c
#define	GET_VOLUME_CONTROL 	0x8d
#define	GET_VOICES		0x8e
#define	GET_IRQV		0x8f

/********************************************************************
 *
 * MIDI defines
 *
 *******************************************************************/

#define MIDI_RESET	0x03
#define	MIDI_ENABLE_XMIT	0x20
#define	MIDI_ENABLE_RCV		0x80

#define	MIDI_RCV_FULL		0x01
#define MIDI_XMIT_EMPTY		0x02
#define MIDI_FRAME_ERR		0x10
#define MIDI_OVERRUN		0x20
#define MIDI_IRQ_PEND		0x80

/********************************************************************
 *
 * JOYSTICK defines
 *
 *******************************************************************/

#define JOY_POSITION		0x0f
#define JOY_BUTTONS			0xf0

/********************************************************************
 *
 * GF1 irq/dma programmable latches
 *
 *******************************************************************/

/* GF1_IRQ_STATUS (port 3X6) */
#define	MIDI_TX_IRQ			0x01		/* pending MIDI xmit IRQ */
#define MIDI_RX_IRQ			0x02		/* pending MIDI recv IRQ */
#define GF1_TIMER1_IRQ		0x04		/* general purpose timer */
#define GF1_TIMER2_IRQ		0x08		/* general purpose timer */
#define WAVETABLE_IRQ		0x20		/* pending wavetable IRQ */
#define ENVELOPE_IRQ		0x40		/* pending volume envelope IRQ */
#define DMA_TC_IRQ			0x80		/* pending dma tc IRQ */


/* GF1_MIX_CTRL (port 2X0) */
#define ENABLE_LINE_IN		0x01		/* 0=enable */
#define ENABLE_OUTPUT		0x02		/* 0=enable */
#define ENABLE_MIC_IN		0x04		/* 1=enable */
#define ENABLE_GF1_IRQ		0x08		/* 1=enable */
#define GF122				0x10		/* ?? */
#define ENABLE_MIDI_LOOP	0x20		/* 1=enable loop back */
#define SELECT_GF1_REG		0x40		/* 0=irq latches */
										/* 1=dma latches */

/********************************************************************
 *
 * GF1 global registers	($41-$4C)
 *
 *******************************************************************/

/* DMA control register */
#define DMA_ENABLE		0x01
#define DMA_READ		0x02		/* 1=read,0=write */
#define DMA_WIDTH_16		0x04		/* 1=16 bit,0=8 bit (dma chan width)*/
#define DMA_RATE		0x18		/* 00=fast, 11=slow */
#define DMA_IRQ_ENABLE		0x20		/* 1=enable */
#define DMA_IRQ_PENDING		0x40		/* read */
#define DMA_DATA_16		0x40		/* write (data width) */
#define DMA_TWOS_COMP		0x80		/* 1=do twos comp */

/* These are the xfer rate bits ... */
#define DMA_R0		0x00		/* Fastest DMA xfer (~650khz) */
#define DMA_R1		0x08		/* fastest / 2 */
#define DMA_R2		0x10		/* fastest / 4 */
#define DMA_R3		0x18		/* Slowest DMA xfer (fastest / 8) */

/* SAMPLE control register */
#define ENABLE_ADC		0x01
#define ADC_MODE		0x02		/* 0=mono, 1=stereo */
#define ADC_DMA_WIDTH		0x04		/* 0=8 bit, 1=16 bit */
#define ADC_IRQ_ENABLE		0x20		/* 1=enable */
#define ADC_IRQ_PENDING		0x40		/* 1=irq pending */
#define ADC_TWOS_COMP		0x80		/* 1=do twos comp */

/* RESET control register */
#define GF1_MASTER_RESET	0x01		/* 0=hold in reset */
#define GF1_OUTPUT_ENABLE	0x02		/* enable output */
#define GF1_MASTER_IRQ		0x04		/* master IRQ enable */

/********************************************************************
 *
 * GF1 voice specific registers	($00 - $0E and $80-$8f)
 *
 *******************************************************************/

/* ($0,$80) Voice control register */
#define VOICE_STOPPED		0x01		/* voice has stopped */
#define STOP_VOICE		0x02		/* stop voice */
#define VC_DATA_TYPE		0x04		/* 0=8 bit,1=16 bit */
#define VC_LOOP_ENABLE		0x08		/* 1=enable */
#define VC_BI_LOOP		0x10		/* 1=bi directional looping */
#define VC_WAVE_IRQ		0x20		/* 1=enable voice's wave irq */
#define VC_DIRECT		0x40		/* 0=increasing,1=decreasing */
#define VC_IRQ_PENDING		0x80		/* 1=wavetable irq pending */

/* ($1,$81) Frequency control */
/* Bit 0  - Unused */
/* Bits 1-9 - Fractional portion */
/* Bits 10-15 - Integer portion */

/* ($2,$82) Accumulator start address (high) */
/* Bits 0-11 - HIGH 12 bits of address */
/* Bits 12-15 - Unused */

/* ($3,$83) Accumulator start address (low) */
/* Bits 0-4 - Unused */
/* Bits 5-8 - Fractional portion */
/* Bits 9-15 - Low 7 bits of integer portion */

/* ($4,$84) Accumulator end address (high) */
/* Bits 0-11 - HIGH 12 bits of address */
/* Bits 12-15 - Unused */

/* ($5,$85) Accumulator end address (low) */
/* Bits 0-4 - Unused */
/* Bits 5-8 - Fractional portion */
/* Bits 9-15 - Low 7 bits of integer portion */


/* ($6,$86) Volume Envelope control register */
#define VL_RATE_MANTISSA		0x3f
#define VL_RATE_RANGE			0xC0

/* ($7,$87) Volume envelope start */
#define	VL_START_MANT			0x0F
#define VL_START_EXP			0xF0

/* ($8,$88) Volume envelope end */
#define VL_END_MANT			0x0F
#define VL_END_EXP			0xF0

/* ($9,$89) Current volume register */
/* Bits 0-3 are unused */
/* Bits 4-11 - Mantissa of current volume */
/* Bits 10-15 - Exponent of current volume */

/* ($A,$8A) Accumulator value (high) */
/* Bits 0-12 - HIGH 12 bits of current position (a19-a7) */

/* ($B,$8B) Accumulator value (low) */
/* Bits 0-8 - Fractional portion */
/* Bits 9-15 - Integer portion of low adress (a6-a0) */

/* ($C,$8C) Pan (balance) position */
/* Bits 0-3 - Balance position  0=full left, 0x0f=full right */

/* ($D,$8D) Volume control register */
#define VOLUME_STOPPED		0x01		/* volume has stopped */
#define STOP_VOLUME		0x02		/* stop volume */
#define VC_ROLLOVER		0x04		/* Roll PAST end & gen IRQ */
#define VL_LOOP_ENABLE		0x08		/* 1=enable */
#define VL_BI_LOOP		0x10		/* 1=bi directional looping */
#define VL_WAVE_IRQ		0x20		/* 1=enable voice's wave irq */
#define VL_DIRECT		0x40		/* 0=increasing,1=decreasing */
#define VL_IRQ_PENDING		0x80		/* 1=wavetable irq pending */

/* ($E,$8E) # of Active voices */
/* Bits 0-5 - # of active voices -1 */

/* ($F,$8F) - Sources of IRQs */
/* Bits 0-4 - interrupting voice number */
/* Bit 5 - Always a 1 */
#define VOICE_VOLUME_IRQ	0x40		/* individual voice irq bit */
#define VOICE_WAVE_IRQ		0x80		/* individual waveform irq bit */

