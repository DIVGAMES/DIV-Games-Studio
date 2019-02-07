#pragma once

/*
        SCRplus - (c) Edward Cree 2010-12
        Licensed under the GNU GPL v3+
*/

#include "bits.h"
#include <math.h>
#include <stdio.h>

typedef unsigned char rgb_t[3], hsv_t[3], yuv_t[3];

// hsv is VALUE, SATURATION, HUE, all in [0, 255] n N
// VALUE 0 is black, 255 is max
// SATURATION 0 is grey, 255 is max
// HUE 0 is red, 85 is green, 170 is blue
void rgbtohsv(const rgb_t rgb, hsv_t hsv);
void hsvtorgb(const hsv_t hsv, rgb_t rgb);

// YUV is LUMINANCE, CHROMINANCE_RED, CHROMINANCE_BLUE, all in [0, 255] n N
void rgbtoyuv(const rgb_t rgb, yuv_t yuv);
void yuvtorgb(const yuv_t yuv, rgb_t rgb);
double yuvdist(const yuv_t pal, const yuv_t dat);
