#pragma once

/*
        SCRplus - (c) Edward Cree 2010-11
        Licensed under the GNU GPL v3+
*/

#include <math.h>
#include <stdio.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

void fputw(unsigned short int a, FILE *stream);
void fputq(unsigned long int a, FILE *stream);
int vtweak(int value, int shift, int scale);
int ctweak(int value, int shift, int scale);
int htweak(int value, int shift);
int ncluts(unsigned char clm);
