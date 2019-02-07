/**************************************************************************
 *  This file is part of TopFLC library v1.0
 *  Copyright 1996 Johannes Lehtinen
 *  All rights reserved
 *
 *  The license in file license.txt applies to this file.
 **************************************************************************/

#ifndef __TOPFLC_H__
#define __TOPFLC_H__

/**************************************************************************
 *  #includes
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/**************************************************************************
 *  Constant #defines
 **************************************************************************/

/* Constants for status values */

#define TF_TRUE 1
#define TF_FALSE 0

#define TF_SUCCESS 1
#define TF_FAILURE 0

/**************************************************************************
 *  Type definitions
 **************************************************************************/

/* Types for some primitives */

typedef unsigned char TFUByte;
typedef signed char TFSByte;
typedef unsigned short int TFUWord;
typedef signed short int TFSWord;
typedef unsigned long int TFUDWord;
typedef signed long int TFSDWord;

/* Type for status values */

typedef unsigned char TFStatus;

/* Type for animation */

typedef struct TFAnimation TFAnimation;

/* Type for animation info */

typedef struct TFAnimationInfo TFAnimationInfo;

/* Type for external error handler */

typedef void (*TFErrorHandler)(char *msg);

/* Type for palette update function */

typedef void (*TFPaletteFunction)(TFUByte (*palette)[256][3]);

/**************************************************************************
 *  Global data structures
 **************************************************************************/

struct TFAnimationInfo {
	short Width, Height;	/* Dimensions of animation */
	short NumFrames;	    /* Number of frames */
	short CurFrame;		    /* Index of next frame (0 for first one) */
	TFUByte *Frame;		    /* Frame buffer (NULL if not set) */
	TFUByte (*Palette)[256][3]; /* Palette buffer (NULL if not set) */
	TFStatus LoopFlag;	  /* TF_TRUE if animation is to be looped
				     *  (default is TF_FALSE) */
};

/**************************************************************************
 *  Global function prototypes
 **************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************
 *  Library handling
 ******************/

/**************************************************************************
 *  TFStatus TFErrorHandler_Set(TFErrorHandler handler);
 *
 *  This function can be used to enable/disable external error handling. If
 *  user specified function <handler> is not NULL, then <handler> is called
 *  with error message every time error occurs. If <handler> is NULL, external
 *  error handling will be disabled.
 **************************************************************************/

TFStatus TFErrorHandler_Set(TFErrorHandler handler);

/********************
 *  Animation handling
 ********************/

/**************************************************************************
 *  TFAnimation *TFAnimation_NewFile(char *name);
 *
 *  Opens new FLI/FLC animation file and returns pointer to animation
 *  structure. Sets frame pointer to first frame and reads animation
 *  information. Doesn't allocate memory for frame buffer nor palette
 *  buffer. Returns NULL on failure (file could not be read or file
 *  is not 256 color FLI/FLC animation).
 **************************************************************************/

TFAnimation *TFAnimation_NewFile(char *name);

/**************************************************************************
 *  TFAnimation *TFAnimation_NewHandle(FILE *handle);
 *
 *  Works exactly like the previous function except that the animation is
 *  read from current position of given file handle.
 **************************************************************************/

TFAnimation *TFAnimation_NewHandle(FILE *handle);

/**************************************************************************
 *  TFAnimation *TFAnimation_NewMem(void *rawdata);
 *
 *  Works exactly like the two previous functions except that the animation
 *  is read from given memory position.
 **************************************************************************/

TFAnimation *TFAnimation_NewMem(void *raw);

/**************************************************************************
 *  TFStatus TFAnimation_Delete(TFAnimation *animation);
 *
 *  Deletes given animation structure. Returns always TF_SUCCESS.
 **************************************************************************/

TFStatus TFAnimation_Delete(TFAnimation *animation);

/**************************************************************************
 *  TFStatus TFAnimation_DeleteAll(void);
 *
 *  Deletes all existing animation structures. Returns always TF_SUCCESS.
 **************************************************************************/

TFStatus TFAnimation_DeleteAll(void);

/**************************************************************************
 *  TFStatus TFAnimation_GetInfo(TFAnimation *animation, TFAnimationInfo *info);
 *
 *  Fills user supplied info structure with animation information. Returns
 *  always TF_SUCCESS.
 **************************************************************************/

TFStatus TFAnimation_GetInfo(TFAnimation *animation, TFAnimationInfo *info);

/**************************************************************************
 *  TFStatus TFAnimation_SetLooping(TFAnimation *animation, TFStatus state);
 *
 *  Enables/disables animation looping. If state is TF_TRUE looping is enabled
 *  and if it is TF_FALSE looping is disabled. Returns always TF_SUCCESS.
 **************************************************************************/

TFStatus TFAnimation_SetLooping(TFAnimation *animation, TFStatus state);

/**************************************************************************
 *  TFStatus TFAnimation_SetPaletteFunction(TFAnimation *animation,
 *                                          TFPaletteFunction update_func);
 *
 *  Sets palette update function for animation. Every time palette changes
 *  in animation this user specified function is called with new palette.
 *  If this function is called with NULL, update function is disabled.
 **************************************************************************/

TFStatus TFAnimation_SetPaletteFunction(TFAnimation *animation,
					TFPaletteFunction update_func);

/*****************
 *  Buffer handling
 *****************/

/**************************************************************************
 *  TFStatus TFBuffers_Set(TFAnimation *animation, void *framebuffer,
 *                         void *palettebuffer);
 *
 *  With this function you can set frame buffer for frame data and palette
 *  buffer for palette data. If there are no buffers set, animation can not
 *  be decoded. If this function is called with NULL pointers, buffers will
 *  be removed. Returns always TF_SUCCESS.
 *
 *  NOTICE! Buffers set with this function are not automatically freed when
 *  animation structure is deleted.
 *
 *  Frame buffer size must be <width>*<height> bytes and palette buffer size
 *  must be 768 bytes.
 **************************************************************************/

TFStatus TFBuffers_Set(TFAnimation *animation, void *framebuffer,
		       void *palettebuffer);

/**************************************************************************
 *  TFStatus TFBuffers_Alloc(TFAnimation *animation);
 *
 *  This function allocates frame buffer and palette buffer for animation.
 *  If there are no buffers set, animation can not be decoced. Function
 *  returns TF_SUCCESS on success and TF_FAILURE on failure. Buffers allocated
 *  with this function are automatically freed when animation structure is
 *  deleted.
 **************************************************************************/

TFStatus TFBuffers_Alloc(TFAnimation *animation);

/**************************************************************************
 *  TFStatus TFBuffers_Free(TFAnimation *animation);
 *
 *  This function frees buffers allocated with TFBuffers_Alloc(). Function
 *  returns TF_SUCCESS on success and TF_FAILURE on failure.
 **************************************************************************/

TFStatus TFBuffers_Free(TFAnimation *animation);

/****************
 *  Frame handling
 ****************/

/**************************************************************************
 *  TFStatus TFFrame_Decode(TFAnimation *animation);
 *
 *  Decodes next frame of animation. Returns TF_SUCCESS on success and
 *  TF_FAILURE on failure. Function can fail if there are no buffers allocated
 *  or if there are no more frames left. After decoding new frame can be read
 *  from frame buffer as a raw color indexed data. The new palette can be read
 *  from palette buffer.
 **************************************************************************/

TFStatus TFFrame_Decode(TFAnimation *animation);

/**************************************************************************
 *  TFStatus TFFrame_Reset(TFAnimation *animation);
 *
 *  Resets frame count of animation so that the next frame to be decoded is
 *  the first frame of animation. Returns TF_SUCCESS on success and
 *  TF_FAILURE on failure.
 **************************************************************************/

TFStatus TFFrame_Reset(TFAnimation *animation);

/**************************************************************************
 *  TFStatus TFFrame_Seek(TFAnimation *animation, int frame);
 *
 *  Seeks animation to given frame. This function can be very slow because
 *  FLIC animations can not be decoded backwards and only way to go
 *  backwards is to start decoding from beginning. Also if you are seeking
 *  forward it can take time to decode frames which are skipped.
 **************************************************************************/

TFStatus TFFrame_Seek(TFAnimation *animation, int frame);

#ifdef __cplusplus
};
#endif

#endif
