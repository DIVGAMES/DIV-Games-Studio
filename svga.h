/****************************************************************************
*
*			The SuperVGA Kit - UniVBE Software Development Kit
*
*                   Copyright (C) 1994 SciTech Software
*                           All rights reserved.
*
* Filename:     $RCSfile: svga.h $
* Version:      $Revision: 1.2 $
*
* Language:     ANSI C
* Environment:  IBM PC (MSDOS) Real Mode and 16/32 bit Protected Mode.
*
* Description:  Header file for the small SuperVGA Kit Library. This
*				library provides a simplified interface to SuperVGA
*               cards that have a VESA VBE (or with the Universal VESA
*               VBE installed). This library requires at least a
*               VBE 1.2 interface to function correctly.
*
*				For performance in protected modes, the SuperVGA Kit
*				internally caches the video memory selector in the GS
*				register, and uses the VBE 2.0 32 bit protected mode
*				routines.
*
*               NOTE: This library only works in the large or flat models.
*
* $Id: svga.h 1.2 1995/09/16 10:45:15 kjb release $
*
****************************************************************************/

#ifndef __SVGA_H
#define __SVGA_H

#ifndef	__VESAVBE_H
#include "vesavbe.h"
#endif

/*--------------------------- Global Variables ----------------------------*/

extern  int     VBEVersion;
extern  int     maxx,maxy;          /* Maximum coordinate values        */
extern	ulong	maxcolor,defcolor;	/* Maximum and default colors		*/
extern  int     maxpage;            /* Maximum video page number        */
extern  int     bytesperline;       /* Bytes in a logical scanline      */
extern	int		bitsperpixel;		/* Bits per pixel for mode			*/
extern  int		bytesperpixel;		/* Bytes in a pixel (if > 1)		*/
extern  int     memory;             /* Memory on board in k             */
extern	long	linearAddr;			/* Address of linear framebuffer	*/
extern  ushort	modeList[];         /* List of available video modes    */
extern  char    OEMString[];        /* OEM string from VBE              */
extern	int		capabilities;		/* Capabilities bits from VBE		*/
extern 	int     curBank;			/* Current read/write bank          */
extern	bool	haveVirtualBuffer;	/* True if virtual buffer is avail	*/
extern	bool	virtualBuffer;		/* True if using virtual buffer		*/
extern	bool	useVirtualBuffer;   /* True to use virtual buffer		*/
extern	void	*videoMem;			/* Pointer to video memory			*/
extern	ulong	originOffset;		/* Offset into video memory region	*/
extern	ushort	bankOffset;			/* Bank offset in banked SVGA modes	*/

/* Pixel format information - used by the rgbColor() routine to build
 * the correct pixel format, but you can use it yourself to build scanline
 * information in the desired format.
 */

extern	uchar		redMask,greenMask,blueMask;
extern	int			redPos,redAdjust;
extern	int			greenPos,greenAdjust;
extern 	int			bluePos,blueAdjust;
extern	VBE_palette	VGA8_defPal[256];

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

int 	SV_init(void);
bool 	SV_setMode(int mode);
int 	SV_getModeName(char *buf,VBE_modeInfo *mi,ushort mode,bool useLinear);
void    SV_restoreMode(void);
bool	SV_set8BitDAC(void);
bool 	SV_set6BitDAC(void);
ulong 	SV_rgbColor(uchar r,uchar g,uchar b);
void 	SV_writeText(int x,int y,char *str,ulong color);
bool 	SV_setBytesPerLine(int bytes);
bool 	SV_setPixelsPerLine(int xMax);
void	SV_setPalette(int start,int num,VBE_palette *pal,int maxProg);
void    _cdecl SV_setVisualPage(int page,bool waitVRT);
void    _cdecl SV_setBank(int bank);

extern void (_cdecl *SV_putPixel)(int x,int y,ulong color);
extern void (_cdecl *SV_clear)(ulong color);
extern void (_cdecl *SV_line)(int x1,int y1,int x2,int y2,ulong color);
extern void (_cdecl *SV_setActivePage)(int page);

/* CPU detection routine */

typedef enum    {SV_cpu86,SV_cpu186,SV_cpu286,SV_cpu286p,
				 SV_cpu386,SV_cpu386p,SV_cpu486,SV_cpu486p,
				 SV_cpu586,SV_cpu586p} SV_cpuType;

int 	_cdecl SV_queryCpu(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* __SVGA_H */
