#ifndef __IFS_H_
#define __IFS_H_

#define IFS_MEM_ERROR       10
#define IFS_FORMAT_ERROR    20
#define IFS_OPEN_ERROR      30
#define IFS_READ_ERROR      40
#define IFS_WRITE_ERROR     50
#define IFS_PARAM_ERROR     60

#define NUM     1
#define MAY     2
#define MIN     4
#define SIM     8
#define EXT     16


// STRUCTURES FOR THE INPUT FILE (IFS)
typedef struct _IFSTABLE{
    int32_t    disp;
    int32_t    size;
} IFSTABLE;

typedef struct _IFSHEADER{
    uint8_t    id[4];
    uint32_t    offset8;
    uint32_t   offset10;
    uint32_t    offset12;
    uint32_t    offset14;
    uint32_t    offset128;
} IFSHEADER;

// STRUCTURE FOR THE OUTPUT FILE (FNT)
typedef struct _FNTTABLE{
    int    width;
    int    height;
    int    incY;
    int    offset;
}FNTTABLE;

typedef struct _FNTHEADER{
    char    id[7];
    char    eof;
} FNTHEADER;

// REQUIRED PARAMETERS
typedef struct _IFS {
    char            ifsName[256];
    char            fntName[256];
    char            sizeX;
    char            sizeY;
    char            outline;
    char            brightness; // (0-4) None, Up-Left, Down-Right, Up-Right, Down-Left.
    short           shadowX;
    short           shadowY;
    unsigned short  bodyTexWidth;    // texture of the letter
    unsigned short  bodyTexHeight;
    char            bodyTexColor;
    char            bodyTexMode;
    unsigned short  shadowTexWidth;  // texture of the Shadow
    unsigned short  shadowTexHeight;
    char            shadowTexColor;
    char            shadowTexMode;
    unsigned short  outTexWidth;    // texture of the Outline
    unsigned short  outTexHeight;
    char            outTexColor;
    char            outTexMode;
    char            table[256];     // to indicate which letters are generated
    char            light[256];    // correspondences for the highlights
    char            dark[256];   // correspondences for the highlights

}IFS;

extern char *bodyTexBuffer, *outTexBuffer, *shadowTexBuffer;
extern IFS ifs;
int Jorge_Creates_the_font(int GenCode);
int ShowChar(int WhatChar,int cx,int cy,char *ptr,int w);
int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int w,char *buffer);
void GetCharSize(int WhatChar,int *width,int *height);
void GetCharSizeBuffer(int WhatChar,int *width,int *height,char *buffer);
void ConvertFntToPal(char *Buffer);
int Progress(char *title,int current,int total);
#endif
