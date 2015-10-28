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


// ESTRUCTURAS PARA EL FICHERO DE ENTRADA (IFS)
typedef struct _TABLAIFS{
    uint32_t    desp;
    uint32_t    size;
} TABLAIFS;

typedef struct _IFSHEADER{
    uint8_t    id[4];
    uint32_t    offset8;
    uint32_t   offset10;
    uint32_t    offset12;
    uint32_t    offset14;
    uint32_t    offset128;
} IFSHEADER;

// ESTRUCTURA PARA EL FICHERO DE SALIDA (FNT)
typedef struct _TABLAFNT{
    int    ancho;
    int    alto;
    int    incY;
    int    offset;
}TABLAFNT;

typedef struct _FNTHEADER{
    char    id[7];
    char    eof;
} FNTHEADER;

// PARAMETROS NECESARIOS
typedef struct _IFS {
    char            ifsName[256];
    char            fntName[256];
    char            tamX;
    char            tamY;
    char            outline;
    char            brillo; // (0-4) Sin, Arr-Izq, Abj-drcha, Arr-dcha, Abj-izq.
    short           sombraX;
    short           sombraY;
    unsigned short  bodyTexAncho;    // textura de la letra
    unsigned short  bodyTexAlto;
    char            bodyTexColor;
    char            bodyTexModo;
    unsigned short  shadowTexAncho;  // textura de la Sombra
    unsigned short  shadowTexAlto;
    char            shadowTexColor;
    char            shadowTexModo;
    unsigned short  outTexAncho;    // textura del Outline
    unsigned short  outTexAlto;
    char            outTexColor;
    char            outTexModo;
    char            tabla[256];     // para indicar que letras se generan
    char            claros[256];    // correspondencias para los brillos
    char            oscuros[256];   // correspondencias para los brillos

}IFS;

extern char *bodyTexBuffer, *outTexBuffer, *shadowTexBuffer;
extern IFS ifs;
int Jorge_Crea_el_font(int GenCode);
int ShowChar(int WhatChar,int cx,int cy,char *ptr,int an);
int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int an,char *buffer);
void GetCharSize(int WhatChar,int *ancho,int *alto);
void GetCharSizeBuffer(int WhatChar,int *ancho,int *alto,char *buffer);
void ConvertFntToPal(char *Buffer);
int Progress(char *titulo,int current,int total);
#endif
