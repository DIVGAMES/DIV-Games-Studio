/**************************************************************************
*  This file is part of TopFLC library v1.0
*  Copyright 1996 Johannes Lehtinen
*  All rights reserved
*
*  The license in file license.txt applies to this file.
**************************************************************************/
#ifndef __TFANIMAT_H__
#define __TFANIMAT_H__

/**************************************************************************
*  #includes
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "topflc.h"

/**************************************************************************
*  Data structures
**************************************************************************/
/* Data structure for animation */
struct TFAnimation {
	short              Source;           /* Animation source:
                                        *     0  -  From file
                                        *     1  -  From handle
                                        *     2  -  From memory */
	FILE              *Handle;           /* File handle (for sources 0 and 1) */
	long               StartOffset;      /* Start offset (for sources 0 and 1) */
	TFUByte           *Address;          /* Address (for source 2) */
	size_t             NowOffset;        /* Current offset (for source 2) */
	short              Width;            /* Width of animation */
	short              Height;           /* Height of animation */
	short              NumFrames;        /* Number of frames in animation */
	short              CurFrame;         /* Number of current frame */
	TFStatus           UserBuffers;      /* TF_TRUE if user supplied buffers */
	TFUByte           *Frame;            /* Pointer to frame buffer or NULL */
	TFUByte          (*Palette)[256][3]; /* Pointer to palette buffer or NULL */
	TFAnimation       *Prev;             /* Pointer to previous animation or NULL */
	TFAnimation       *Next;             /* Pointer to next animation or NULL */
	TFStatus           LoopFlag;         /* TF_TRUE if to be looped */
	TFPaletteFunction  PaletteFunction;  /* Palette update function or NULL */
};
#endif
