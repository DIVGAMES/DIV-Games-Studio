/**************************************************************************
*  This file is part of TopFLC library v1.0
*  Copyright 1996 Johannes Lehtinen
*  All rights reserved
*
*  The license in file license.txt applies to this file.
**************************************************************************/
#ifndef __TFLIB_H__
#define __TFLIB_H__

/**************************************************************************
*  #includes
**************************************************************************/
#include <stdio.h>
#include "topflc.h"

/**************************************************************************
*  Function prototypes
**************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
*  void TFError_Report(char *msg);
*
*  This function is used by other library functions to report possible
*  error situations.
**************************************************************************/
void TFError_Report(char *msg);

#ifdef __cplusplus
};
#endif

#endif
