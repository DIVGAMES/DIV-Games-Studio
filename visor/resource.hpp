#ifndef __PACKED_H_
#define __PACKED_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <conio.h>

typedef struct _Pack{
        char Name[16];
        unsigned int Offsets;
        unsigned int Length;
        unsigned int RealLength;
}Pack;

typedef struct {
  Pack *MyPack;
  FILE *Packed;
  unsigned int Num_Pack;
} Resource;

void resource_create(Resource *resource_struct,char *name);
void resource_destroy(Resource *resource_struct);
char *resource_GetPacked(Resource *resource_struct,char *);
char *resource_GetGraphPacked(Resource *resource_struct,char *Name);
unsigned int resource_Exist(Resource *resource_struct,char *Name);

unsigned long resource_fTellPacked(Resource *resource_struct);
void resource_fSeekPacked(Resource *resource_struct,long pos);
unsigned int resource_GetPackedLen(Resource *resource_struct,char *Name);
FILE *resource_GetFilePtrPacked(Resource *resource_struct,char *Name);

#endif
