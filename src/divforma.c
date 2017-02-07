
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      MÃ³dulo que contiene el cÃ³digo de lectura/escritura de formatos grÃ¡ficos
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

#include "global.h"


#ifdef  __cplusplus
extern "C" {
#endif

#ifdef JPGLIB
#include <jpeglib.h>
#endif


static jmp_buf jmp_error_ptr;

#ifdef  __cplusplus
}
#endif

// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Constantes
// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

#define BI_RGB      0
#define BI_RLE8     1
#define BI_RLE4     2

#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Estructuras
// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
typedef struct _rgb_quad
{
        unsigned char   rgbBlue;
        unsigned char   rgbGreen;
        unsigned char   rgbRed;
        unsigned char   rgbReserved;
}rgb_quad;

// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
// For adding graphics formats follow these steps
// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

// - Add the extension in div.cpp chain ".MAP ... * * *."

// - Create the following functions in divforma.cpp (if the type is XYZ)

// PROT: int is_XYZ (byte * buffer);
// DESC: Returns TRUE if the file is loaded into the buffer type XYZ, and Adem s 
// returns the width / height of the bitmap in the variables map_an / p

// PROT: void decompresses_XYZ (byte * buffer, byte * map vent int);
// DESC: The function Ã³ n receives the loaded file in buffer and another buffer
// Map_an * map_al + map_an bytes (map), you have to unpack this
// latest.
// "Vent" indicates whether to charge the additional information window
// (Such as checkpoints, see already defined formats)
// If the file has palette, it must be stored in the buffer
// 768 bytes called dac4 []

// PROT: int record_XYZ (byte * Map, FILE * f);
// DESC: You must save the bitmap in the filename. S own buffer addition
// Containing the map, you have access to the following information:
// - Map_an, map_al (map width and height in pixels)
// - Dac [768] (palette file, 256 RGB values between 0 and 63)
// When the file is recording MAP (own format) you have in addition s
// - Map_codigo (number associated with the map, long integer, 4 bytes)
// - Map_centro_x / y (center of the map)
// - Map_descripcion [32] (descriptive comment map)
// The function returns TRUE if the file has been recorded without any problems,
// Or FALSE if there was not enough disk space.
// Al pricipio should assume "f" a clever binary file for writing
// Must be written on with fwrite, and in the end should not close

// NOTE: Any function that requires additional memory for your
// Job, you can ask to malloc, and if you can not get the memory
// Required, then you must create an informative error window
// As follows: I = text v_texto [45]; dialogue ((voidReturnType) Err0);

// NOTE 2: The only format that returns value to decompress is the JPG. EYE !!!

// - Finally you must call the three functions created in divhandl.cpp
// And divbrow.cpp (look, for example, "MAP" on these two sources) and put
// New prototypes Global.cpp (where n is the above) 
// Tambi n have to be put in divsetup.cpp and divpalet.cpp (preparing Skin)

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      MAP Format
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

int es_MAP (byte * buffer) {
	if (!strcmp((char *)buffer,"map\x1a\x0d\x0a")) {
		map_an=*(word*)(buffer+8);
		map_al=*(word*)(buffer+10);
		return(1);
	} else 
		return(0);
}

void descomprime_MAP (byte * buffer, byte * mapa, int vent) {
	
	short npuntos;
	if (vent) {
		memcpy(&Codigo,buffer+12,4);
		memcpy(Descripcion,buffer+16,32);
	}

	memcpy(dac4,buffer+48,768);
	memcpy(&npuntos,buffer+1392,2);
	
	if (vent) {
		memcpy(reglas,buffer+816,sizeof(reglas));
		if(npuntos!=0)
			memcpy(v_mapa->puntos,buffer+1394,npuntos*4);
	}
	
	memcpy(mapa,buffer+1394+(npuntos*4),map_an*map_al);
}

int graba_MAP (byte * mapa, FILE * f) {

	word x,npuntos=0;
	int y;
	int i;

	fwrite("map\x1a\x0d\x0a\x00\x00",8,1,f);      // +000 Cabecera y version
	x=map_an; fwrite(&x,2,1,f);                   // +008 Ancho
	x=map_al; fwrite(&x,2,1,f);                   // +010 Alto
	y=ventana[v_ventana].mapa->Codigo; fwrite(&y,4,1,f);// +012 CÃ³digo

	fwrite(ventana[v_ventana].mapa->descripcion,32,1,f);// +016 Descripcion
	fwrite(dac,768,1,f);                          // +048 Paleta
	fwrite(reglas,1,sizeof(reglas),f);            // +816 Reglas de color

	npuntos=0;

	for(i=511;i>=0;i-=2)
		if(ventana[v_ventana].mapa->puntos[i]!=-1) {
				npuntos=(i+1)/2;
				i=-1;
		}

	fwrite(&npuntos,2,1,f);                     // +1392 Numero de puntos
	fwrite(&ventana[v_ventana].mapa->puntos,npuntos,4,f);

	y=map_an*map_al;

	do { 
		if (y>=768) {
			fwrite(mapa,768,1,f); y-=768; mapa+=768;
		} else {
			fwrite(mapa,y,1,f); y=0;
		} 
	} while (y);

	return 0;
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Formato PCX
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Structs
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

typedef struct _pcx_header {
	char manufacturer;
	char version;
	char encoding;
	char bits_per_pixel;
	short  xmin,ymin;
	short  xmax,ymax;
	short  hres;
	short  vres;
	char   palette16[48];
	char   reserved;
	char   color_planes;
	short  bytes_per_line;
	short  palette_type;
	short  Hresol;
	short  Vresol;
	char  filler[54];
}pcx_header;

struct pcx_struct {
	pcx_header header;
	unsigned char far *cimage;
	unsigned char palette[3*256];
	unsigned char far *image;
	int clength;
};
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Functions
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

int es_PCX(byte *buffer)
{
	int loes=0;

	if(buffer[2]==1) {

		if(buffer[3]==8 && (buffer[65]==1 || buffer[65]==3)) 
			loes=1;
		if(buffer[3]==1 && (buffer[65]==4)) 
			loes=1;
	}

	map_an=*(word*)(buffer+8)-*(word*)(buffer+4)+1;
	map_al=*(word*)(buffer+10)-*(word*)(buffer+6)+1;

	return(loes);
}

extern byte * muestra;
extern byte nueva_paleta[768];
extern int num_colores;
void crear_paleta(void);
void browser2(void);
byte * descomprime_rle(byte * buffer,unsigned int bytes_line,unsigned int last_byte,byte * pDest);

int cargar_paleta=0;

void descomprime_PCX(byte *buffer, byte *mapa, int vent)
{
  unsigned int con;
//unsigned int fExit=0;
  unsigned int pixel=0, pixel_line=0;
  unsigned int last_byte,bytes_line;
  char ch, rep;
  pcx_header header;
  byte *pSrc, *pDest, *pSrcLine, *AuxPtr;
  int  rgb_color, x, y;
  byte rgb_blue, rgb_green, rgb_red;
  byte color16;
  int  con16;
  rgb_quad Pcxdac[256];
  byte *old_muestra;

  memcpy((byte *)&header,buffer,sizeof(pcx_header));
  buffer+=128;                                  // Comienzo de la imagen

  map_an = header.xmax - header.xmin + 1;
  map_al = header.ymax - header.ymin + 1;

  if((!map_an && !map_al) || map_an<0 || map_al<0) return;

  memset (mapa, 0, map_an * map_al);

  last_byte  = header.bytes_per_line * header.color_planes * map_al ;
  bytes_line = header.bytes_per_line * header.color_planes;

  // Si es una imagen 24bpp
  if(header.color_planes==3)
  {
    if ((pDest=(byte *)malloc((map_an+1)*(map_al+1)*3))==NULL)
    {
      v_texto=(char *)texto[45];
      dialogo((voidReturnType)err0);
      return;
    }
    memset (pDest, 0, (map_an+1)*(map_al+1)*3);
    pSrc = pDest;
  }
  // Si es una imagen 4bpp
  else if(header.color_planes==4)
  {
    if ((pDest=(byte *)malloc(last_byte))==NULL)
    {
      v_texto=(char *)texto[45];
      dialogo((voidReturnType)err0);
      return;
    }
    memset (pDest, 0, last_byte);
    pSrc = pDest;
  }
  else pDest = mapa;

  if(header.bits_per_pixel!=8 || header.color_planes!=1) // Si no es 8bpp
  {
    do {
      ch=*buffer++;                               // Copia uno por defecto.
      if((ch&192)==192) {                         // Si RLE entonces
        rep=(ch&63);                              // rep = nÂº de veces a copiar.
        ch=*buffer++;
      } else rep=1;
      pixel+=rep;                                 // Controla que no nos salgamos.
      if(pixel>last_byte) {
        rep-=pixel-last_byte;
        for(con=0;con<rep;con++) *pDest++=ch;
        break;
      }
      for(con=0;con<rep;con++) *pDest++=ch;
    } while(1);
  }

  if(header.bits_per_pixel==8 && header.color_planes==1)
  {
    do {
      ch=*buffer++;                               // Copia uno por defecto.
      if((ch&192)==192) {                         // Si RLE entonces
        rep=(ch&63);                              // rep = nÂº de veces a copiar.
        ch=*buffer++;
      } else rep=1;
      pixel+=rep;                                 // Controla que no nos salgamos.
      pixel_line+=rep;
      if(pixel>last_byte) {
        rep-=pixel-last_byte;
        for(con=0;con<rep;con++) *pDest++=ch;
        break;
      }
      if(pixel_line==bytes_line) {
        pixel_line=0;
        rep-=bytes_line-map_an;
      }
      for(con=0;con<rep;con++) *pDest++=ch;
    } while(1);

    memcpy(dac4, buffer, 768);
    for (con=0; con<768; con++) dac4[con] /= 4;
  }
  else if(header.bits_per_pixel==1 && header.color_planes==4)
  {
    // Convierte de 4 a 8
    pDest    = mapa;
    AuxPtr   = pSrc;
    pSrcLine = pSrc;
    for (y=0; y<map_al; y++)                    // For each line...
    {
      pSrc  = pSrcLine;
      pixel = map_an;
      for (x=0; x<header.bytes_per_line; x++) // For each byte...
      {
        for (con16=7; con16>=0 && pixel; con16--)        // For each pixel...
        {
          color16  = 0;
          color16 |= (((*(pSrc)                         & (1 << con16)) >> con16) );
          color16 |= (((*(pSrc+header.bytes_per_line)   & (1 << con16)) >> con16) << 1);
          color16 |= (((*(pSrc+header.bytes_per_line*2) & (1 << con16)) >> con16) << 2);
          color16 |= (((*(pSrc+header.bytes_per_line*3) & (1 << con16)) >> con16) << 3);
          *pDest = color16;
          pDest++;
          pixel--;
        }
        pSrc++;
      }

      pSrcLine += header.bytes_per_line * 4;
    }
    free(AuxPtr);

    memcpy(dac4, header.palette16, 48);
    memset(&dac4[48], 0, 720);
    for (con=0; con<48; con++) dac4[con] /= 4;
  }
  else                                          // Si es 24bpp.
  {

    old_muestra=muestra;
    muestra=(byte*)malloc(32768);

    if ((v.click_handler==(voidReturnType)browser2 && !cargar_paleta) || muestra==NULL) {

      if (muestra!=NULL) free(muestra);
      muestra=old_muestra;

      // Crea la paleta 3-3-2
      for(rgb_color=0; rgb_color<256; rgb_color++) {
        Pcxdac[rgb_color].rgbRed   = (rgb_color & 0xE0) >> 5;
        Pcxdac[rgb_color].rgbGreen = (rgb_color & 0x1C) >> 2;
        Pcxdac[rgb_color].rgbBlue  = (rgb_color & 0x03);
      }

      // Convierte de 24 a 8
      pDest    = mapa;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_al; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = *(pSrc);
          rgb_green = *(pSrc + header.bytes_per_line);
          rgb_blue  = *(pSrc + header.bytes_per_line * 2);
          *pDest = (rgb_red&0xE0) | ((rgb_green&0xE0)>>3) | ((rgb_blue&0xC0)>>6);
          pDest ++;
          pSrc  ++;
        }
        pSrcLine += header.bytes_per_line * 3;
      }

      free(pSrc=AuxPtr);

      // Pasa los coeficientes a dac4 (valores de 0 a 63)
      for(x=0; x<256; x++) {
        dac4[x*3+0] = Pcxdac[x].rgbRed   *  9;
        dac4[x*3+1] = Pcxdac[x].rgbGreen *  9;
        dac4[x*3+2] = Pcxdac[x].rgbBlue  * 21;
      }

    } else {

      memset(muestra,0,32768);

      // Convierte de 24 a 8
      pDest    = mapa;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_al; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = *(pSrc);
          rgb_green = *(pSrc + header.bytes_per_line);
          rgb_blue  = *(pSrc + header.bytes_per_line * 2);
          muestra[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)]=1;
          pDest ++;
          pSrc  ++;
        }
        pSrcLine += header.bytes_per_line * 3;
      }

      pSrc=AuxPtr;

      crear_paleta();

      // Convierte de 24 a 8
      pDest    = mapa;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_al; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = *(pSrc);
          rgb_green = *(pSrc + header.bytes_per_line);
          rgb_blue  = *(pSrc + header.bytes_per_line * 2);
          *pDest = muestra[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)];
          pDest ++;
          pSrc  ++;
        }
        pSrcLine += header.bytes_per_line * 3;
      }

      free(muestra); free(pSrc=AuxPtr);
      muestra=old_muestra;

      memcpy(&dac4[0],&nueva_paleta[0],768);

    }

  }
  if (vent) {
    for(x=0;x<512;x++) v_mapa->puntos[x]=-1;
    Codigo=0;
    Descripcion[0]=0;
  }
}

int graba_PCX(byte *mapa,FILE *f) {
  byte p[768];
  int x;
  byte *cbuffer;
  struct pcx_struct pcx;
  int ptr=0;
  int cptr=0;
  int Desborde=0;
  char ActPixel;
  char cntPixel=0;
  char Paletilla=12;

        pcx.header.manufacturer=10;
        pcx.header.version=5;
        pcx.header.encoding=1;
        pcx.header.bits_per_pixel=8;
        pcx.header.xmin=0;
        pcx.header.ymin=0;
        pcx.header.xmax=map_an-1;
        pcx.header.ymax=map_al-1;
        pcx.header.hres=map_an;
        pcx.header.vres=map_al;
        pcx.header.color_planes=1;
        pcx.header.bytes_per_line=map_an;
        pcx.header.palette_type=0;
        pcx.header.Hresol=map_an;
        pcx.header.Vresol=map_al;

        if ((cbuffer=(unsigned char *)malloc(map_an*map_al*2))==NULL)
        {
                //Error reservando memoria.
                v_texto=(char *)texto[45]; dialogo((voidReturnType)err0);
                return(1);
        }
        ActPixel=mapa[ptr];
        while (ptr < map_an*map_al)
        {
                while((mapa[ptr]==ActPixel) && (ptr<map_an*map_al))
                {
                        cntPixel++;
                        Desborde++;
                        ptr++;
                        if(Desborde==map_an)
                        {
                                Desborde=0;
                                break;
                        }
                        if(cntPixel==63)
                                break;
                }
                if(cntPixel==1)
                {
                        if(ActPixel>63)
                                cbuffer[cptr++] = 193;
                        cbuffer[cptr++] = ActPixel;
                }
                else
                {
                        cbuffer[cptr++] = 192+cntPixel;
                        cbuffer[cptr++] = ActPixel;
                }
                ActPixel=mapa[ptr];
                cntPixel=0;
        }

        fwrite(&pcx.header,1,sizeof(pcx_header),f);
        fwrite(cbuffer,1,cptr,f);
        fwrite(&Paletilla,1,1,f);
        for (x=0;x<768;x++) p[x]=dac[x]*4;
        fwrite(p,1,768,f);
        free(cbuffer);
        return(0);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//  Rutina de descompresiÃ³n de RLE optimizada
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

byte * descomprime_rle(byte * buffer,unsigned int bytes_line,unsigned int last_byte,byte * pDest) {

  unsigned int con;
  unsigned int pixel=0, pixel_line=0;
  char ch, rep;

  do {
    ch=*buffer++;                               // Copia uno por defecto.
    if((ch&192)==192) {                         // Si RLE entonces
      rep=(ch&63);                              // rep = nÂº de veces a copiar.
      ch=*buffer++;
    } else rep=1;
    pixel+=rep;                                 // Controla que no nos salgamos.
    pixel_line+=rep;
    if(pixel>last_byte) {
      rep-=pixel-last_byte;
      for(con=0;con<rep;con++) *pDest++=ch;
      break;
    }
    if(pixel_line==bytes_line) {
      pixel_line=0;
      if(bytes_line&1) buffer++;
    }
    for(con=0;con<rep;con++) *pDest++=ch;
  } while(1);

  return(buffer);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Formato BMP
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

typedef struct _bmpfile_header
{
        unsigned short  bfType;         //2
        unsigned int    bfSize;         //4
        unsigned short  bfReserved1;    //2
        unsigned short  bfReserved2;    //2
        unsigned int    bfOffBits;      //4
                                        //14
}bmpfile_header;

typedef struct _bmpinfo_header
{
        unsigned int   biSize;                   // 4
        unsigned int   biWidth;                  // 4
        unsigned int   biHeight;                 // 4
        unsigned short biPlanes;                 // 2
        unsigned short biBitCount;               // 2
        unsigned int   biCompression;            // 4
        unsigned int   biSizeImage;              // 4
        unsigned int   biXPelsPerMeter;          // 4
        unsigned int   biYPelsPerMeter;          // 4
        unsigned int   biClrUsed;                // 4
        unsigned int   biClrImportant;           // 4
                                                 // 40

} bmpinfo_header;

int es_BMP(byte *buffer)
{
  bmpfile_header FileHeader;
  bmpinfo_header InfoHeader;
  byte           *CopiaBuffer;

  FileHeader.bfType=*((unsigned short*)buffer);
  FileHeader.bfSize=*((unsigned int*)(buffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(buffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(buffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(buffer+10));

  if(FileHeader.bfType!=0x4D42) return(0);
  CopiaBuffer=buffer+14;
  memcpy(&InfoHeader,CopiaBuffer,40);
//if(InfoHeader.biCompression) if(InfoHeader.biCompression!=BI_RLE8) return(0);
  if(InfoHeader.biBitCount!=4 &&
     InfoHeader.biBitCount!=8 &&
     InfoHeader.biBitCount!=24) return(0);

  map_an=InfoHeader.biWidth;
  map_al=InfoHeader.biHeight;

  return(1);
}

void descomprime_BMP(byte *buffer, byte *mapa, int vent)
{
  bmpfile_header FileHeader;
  bmpinfo_header InfoHeader;
  rgb_quad       Bmpdac[256];
  byte bEOL;    // 1 if end of line reached.
  byte bEOF=0;  // 1 if end of file reached.
  byte *pSrc, *pSrcLine, *pDest;
  byte *AuxPtr;
  int x, y, fixmap_an, rgb_color, num_colors;
  byte rgb_red, rgb_green, rgb_blue, RunLength, pixel;
  byte *old_muestra;

  FileHeader.bfType=*((unsigned short*)buffer);
  FileHeader.bfSize=*((unsigned int*)(buffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(buffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(buffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(buffer+10));

  pSrc=buffer+14;
  memcpy(&InfoHeader,pSrc,40);
  map_an = InfoHeader.biWidth;
  map_al = InfoHeader.biHeight;

  if((!map_an && !map_al) || map_an<0 || map_al<0) return;

  pSrc=buffer+54;
  if(InfoHeader.biBitCount==8)
  {
    if(InfoHeader.biClrUsed) memcpy(Bmpdac, pSrc, 4*InfoHeader.biClrUsed);
    else                     memcpy(Bmpdac, pSrc, 4*256);
  }
  else if(InfoHeader.biBitCount==4)
  {
    if(InfoHeader.biClrUsed) memcpy(Bmpdac, pSrc, 4*InfoHeader.biClrUsed);
    else                     memcpy(Bmpdac, pSrc, 4*16);
  }

  pSrc     = buffer+FileHeader.bfOffBits;
  pSrcLine = buffer+FileHeader.bfOffBits;

  fixmap_an=map_an; if(map_an%4) fixmap_an+=4-(map_an%4);

  pDest = mapa;

  memset (pDest, 0, map_an * map_al);

  // Descomprime el BMP si es necesario

  if(InfoHeader.biBitCount==8)
  {
    if(InfoHeader.biCompression==BI_RLE8)
    {
      for (y=0; y<map_al && !bEOF; y++)
      { // For each line...
        pDest = &mapa[map_an*map_al-map_an*(y+1)];
        bEOL=0;
        while (!bEOL)
        { // For each packet do
          RunLength = *pSrc;
          pSrc++;
          if (RunLength==0)
          { // Literal or escape.
            RunLength = *pSrc;
            pSrc++;
            switch(RunLength)
            {
              case 0:  // End of line
                bEOL = 1;
                break;
              case 1:  // End of file
                bEOL = 1;
                bEOF = 1;
                break;
              case 2:  // Delta (No soportado)
                bEOL = 1;
                bEOF = 1;
                break;
              default: // Copia absoluta de bytes (de 3 a 255)
                memcpy (pDest, pSrc, RunLength);
                pDest += RunLength;
                pSrc  += RunLength;
                if (RunLength & 1) pSrc++; // Word alignment at end of literal packet.
                break;
            }
          }
          else
          { // Encoded packet:
            // RunLength pixels, all with the same value
            memset (pDest, *pSrc, RunLength);
            pSrc++;
            pDest += RunLength;
          }
        }
      }
    }
    else
    {
      for(y=0;y<map_al;y++)
      {
        memcpy(mapa+y*map_an,pSrc+((map_al-1)-y)*fixmap_an,map_an);
      }
    }
    y=0;
    num_colors=InfoHeader.biClrUsed;
    if(!num_colors) num_colors=256;
    for(x=0; x<num_colors; x++)
    {
      dac4[y++] = Bmpdac[x].rgbRed   / 4;
      dac4[y++] = Bmpdac[x].rgbGreen / 4;
      dac4[y++] = Bmpdac[x].rgbBlue  / 4;
    }
    for(; x<256; x++) dac4[y++] = 0, dac4[y++] = 0, dac4[y++] = 0;
  }
  else if(InfoHeader.biBitCount==4)
  {
    if(InfoHeader.biCompression==BI_RLE4)
    {
      for (y=0; y<map_al && !bEOF; y++)
      { // For each line...
        pDest = &mapa[map_an*map_al-map_an*(y+1)];
        bEOL=0;
        while (!bEOL)
        { // For each packet do
          RunLength = *pSrc;
          pSrc++;
          if (RunLength==0)
          { // Literal or escape.
            RunLength = *pSrc;
            pSrc++;
            switch(RunLength)
            {
              case 0:  // End of line
                bEOL = 1;
                break;
              case 1:  // End of file
                bEOL = 1;
                bEOF = 1;
                break;
              case 2:  // Delta (No soportado)
                bEOL = 1;
                bEOF = 1;
                break;
              default: // Copia absoluta de bytes (de 3 a 255)
                for(x=0; x<RunLength; x++)
                {
                  if(x&1) { *pDest = (*pSrc  & 0x0F); pDest++; pSrc++;}
                  else    { *pDest = (*pSrc >> 4);    pDest++; }
                }
                if(x&1) pSrc++;
                if (((RunLength+1)/2) & 1) pSrc++; // Word alignment at end of literal packet.
                break;
            }
          }
          else
          { // Encoded packet:
            // RunLength pixels, all with the same value
            for(x=0; x<RunLength; x++)
            {
              if(x&1) { *pDest = (*pSrc  & 0x0F); pDest++; }
              else    { *pDest = (*pSrc >> 4);    pDest++; }
            }
            pSrc++;
          }
        }
      }
    }
    else
    {
      fixmap_an /= 2;
      if(map_an & 1) fixmap_an += 2;
      for(y=0;y<map_al;y++)
      {
        for(x=0; x<fixmap_an; x++)
        {
          pixel = pSrc[((map_al-1)-y)*fixmap_an+x];
          mapa[y*map_an+x*2]   = (pixel >> 4);
          mapa[y*map_an+x*2+1] = (pixel  & 0x0F);
        }
      }
    }
    y=0;
    num_colors=InfoHeader.biClrUsed;
    if(!num_colors) num_colors=16;
    for(x=0; x<num_colors; x++)
    {
      dac4[y++] = Bmpdac[x].rgbRed   / 4;
      dac4[y++] = Bmpdac[x].rgbGreen / 4;
      dac4[y++] = Bmpdac[x].rgbBlue  / 4;
    }
    for(; x<256; x++) dac4[y++] = 0, dac4[y++] = 0, dac4[y++] = 0;
  }
  else if(InfoHeader.biBitCount==24) // BMP de 24 bpp
  {
    old_muestra=muestra;
    muestra=(byte*)malloc(32768);

    if ((v.click_handler==(voidReturnType)browser2&&!cargar_paleta) || muestra==NULL) {

      if (muestra!=NULL) free(muestra);
      muestra=old_muestra;

      // Crea la paleta 3-3-2
      for(rgb_color=0; rgb_color<256; rgb_color++)
      {
        Bmpdac[rgb_color].rgbRed   = (rgb_color & 0xE0) >> 5;
        Bmpdac[rgb_color].rgbGreen = (rgb_color & 0x1C) >> 2;
        Bmpdac[rgb_color].rgbBlue  = (rgb_color & 0x03);
      }

      // Convierte de 24 a 8
      fixmap_an=((map_an*3)/4)*4;
      if(fixmap_an != map_an*3) fixmap_an+=4;
      for (y=0; y<map_al; y++)
      { // For each line...
        pDest = &mapa[map_an*map_al-map_an*(y+1)];
        pSrc  = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          *pDest = (rgb_red&0xE0) | ((rgb_green&0xE0)>>3) | ((rgb_blue&0xC0)>>6);
          pDest ++;
          pSrc += 3;
        }
        pSrcLine += fixmap_an;
      }

      // Pasa los coeficientes a dac4 (valores de 0 a 63)
      y=0;
      for(x=0; x<256; x++)
      {
        dac4[y++] = Bmpdac[x].rgbRed   *  9;
        dac4[y++] = Bmpdac[x].rgbGreen *  9;
        dac4[y++] = Bmpdac[x].rgbBlue  * 21;
      }

    } else {

      memset(muestra,0,32768);

      // Convierte de 24 a 8
      AuxPtr=pSrc;
      fixmap_an=((map_an*3)/4)*4;
      if(fixmap_an != map_an*3) fixmap_an+=4;
      for (y=0; y<map_al; y++)
      { // For each line...
        pSrc  = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          muestra[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)]=1;
          pSrc += 3;
        }
        pSrcLine += fixmap_an;
      } pSrc=pSrcLine=AuxPtr;

      crear_paleta();

      // Convierte de 24 a 8
      fixmap_an=((map_an*3)/4)*4;
      if(fixmap_an != map_an*3) fixmap_an+=4;
      for (y=0; y<map_al; y++)
      { // For each line...
        pDest = &mapa[map_an*map_al-map_an*(y+1)];
        pSrc  = pSrcLine;
        for (x=0; x<map_an; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          *pDest = muestra[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)];
          pDest ++;
          pSrc += 3;
        }
        pSrcLine += fixmap_an;
      }

      free(muestra);
      muestra=old_muestra;
      memcpy(&dac4[0],&nueva_paleta[0],768);

    }
  }

  if (vent) {
    for(x=0;x<512;x++) v_mapa->puntos[x]=-1;
    Codigo=0;
    Descripcion[0]=0;
  }
}

int graba_BMP(byte *mapa,FILE *f)
{
bmpfile_header     FileHeader;
bmpinfo_header     InfoHeader;
rgb_quad           Bmpdac[256];
int x,y=0;

byte pad[4]={0,0,0,0};
int pad_an = map_an + 4-(map_an%4);

//BITMAPFILE_HEADER
        FileHeader.bfType=0x4D42;
        FileHeader.bfSize=1078+pad_an*map_al;
        FileHeader.bfReserved1=0;
        FileHeader.bfReserved2=0;
        FileHeader.bfOffBits=1078;
        fwrite(&FileHeader.bfType,2,1,f);
        fwrite(&FileHeader.bfSize,4,1,f);
        fwrite(&FileHeader.bfReserved1,2,1,f);
        fwrite(&FileHeader.bfReserved2,2,1,f);
        fwrite(&FileHeader.bfOffBits,4,1,f);
//BITMAPINF_OHEADER
        InfoHeader.biSize=40;
        InfoHeader.biWidth=map_an;
        InfoHeader.biHeight=map_al;
        InfoHeader.biPlanes=1;
        InfoHeader.biBitCount=8;
        InfoHeader.biCompression=0;
        InfoHeader.biSizeImage=pad_an*map_al;
        InfoHeader.biXPelsPerMeter=0;
        InfoHeader.biYPelsPerMeter=0;
        InfoHeader.biClrUsed=0;
        InfoHeader.biClrImportant=0;

        fwrite(&InfoHeader.biSize,4,1,f);
        fwrite(&InfoHeader.biWidth,4,1,f);
        fwrite(&InfoHeader.biHeight,4,1,f);
        fwrite(&InfoHeader.biPlanes,2,1,f);
        fwrite(&InfoHeader.biBitCount,2,1,f);
        fwrite(&InfoHeader.biCompression,4,1,f);
        fwrite(&InfoHeader.biSizeImage,4,1,f);
        fwrite(&InfoHeader.biXPelsPerMeter,4,1,f);
        fwrite(&InfoHeader.biYPelsPerMeter,4,1,f);
        fwrite(&InfoHeader.biClrUsed,4,1,f);
        fwrite(&InfoHeader.biClrImportant,4,1,f);
        for(x=0;x<256;x++)
        {
                Bmpdac[x].rgbRed=dac[y++]*4;
                Bmpdac[x].rgbGreen=dac[y++]*4;
                Bmpdac[x].rgbBlue=dac[y++]*4;
        }
        fwrite(&Bmpdac,1024,1,f);
        for(x=0;x<map_al;x++) {
			fwrite(mapa+((map_al-1)-x)*map_an,1,map_an,f);
			if(pad_an!=map_an)
				fwrite(pad,1,pad_an-map_an,f);
		}
return(1);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Formato JPG
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

int es_JPG(byte *buffer, int img_filesize)
{
	return 0;
	
#ifdef JPGLIB
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         my_err_mgr;

  cinfo.err = jpeg_std_error(&my_err_mgr);

  if (setjmp(jmp_error_ptr))
  {
    jpeg_destroy((j_common_ptr) &cinfo);
    return(0);
  }

  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, buffer, img_filesize);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);
  map_an = cinfo.output_width;
  map_al = cinfo.output_height;
  jpeg_destroy_decompress(&cinfo);

  return(1);
#else
return (0);
#endif

}

int descomprime_JPG(byte *buffer, byte *mapa, int vent, int img_filesize)
{
	
#ifdef JPGLIB
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         my_err_mgr;
  
  JSAMPARRAY buffer_scanline;
  int row_stride;
  int x, y;

  vent=vent;
  memset(&cinfo,0,sizeof(struct jpeg_decompress_struct));
  
  cinfo.err = jpeg_std_error(&my_err_mgr);

  if (setjmp(jmp_error_ptr))
  {
    jpeg_destroy((j_common_ptr) &cinfo);
    return(0);
  }
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, buffer, img_filesize);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.quantize_colors=TRUE;
  if (v.click_handler==(voidReturnType)browser2) {
    cinfo.dither_mode=JDITHER_NONE;
    cinfo.two_pass_quantize=FALSE;
  } else {
    cinfo.dither_mode=JDITHER_FS;
    cinfo.two_pass_quantize=TRUE;
  }
  jpeg_start_decompress(&cinfo);

  map_an = cinfo.output_width;
  map_al = cinfo.output_height;

  if((!map_an && !map_al) || map_an<0 || map_al<0)
  {
    jpeg_destroy_decompress(&cinfo);
    return(0);
  }

  row_stride = cinfo.output_width * cinfo.output_components;
  buffer_scanline = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height)
  {
    jpeg_read_scanlines(&cinfo, buffer_scanline, 1);
    memcpy(&mapa[(cinfo.output_scanline-1)*cinfo.output_width],
           buffer_scanline[0], row_stride);
  }
  // Pasa los coeficientes a dac4 (valores de 0 a 63)

  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    y=0;
    for(x=0; x<256; x++)
    {
      dac4[y++] = x/4;
      dac4[y++] = x/4;
      dac4[y++] = x/4;
    }
  } else {
    y=0;
    for(x=0; x<256; x++)
    {
      dac4[y++] = cinfo.colormap[0][x] / 4;
      dac4[y++] = cinfo.colormap[1][x] / 4;
      dac4[y++] = cinfo.colormap[2][x] / 4;
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  if (vent) {
    for(x=0;x<512;x++) v_mapa->puntos[x]=-1;
    Codigo=0;
    Descripcion[0]=0;
  }

  return(1);
#else 
return (0);
#endif
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//  Funciones para cargar la paleta de un archivo
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

// Quita de dac4 los colores no usados en el mapa
// OJO, quien llame a esta funcion debe guardar y restaurar "paleta_original"

void quitar_colores(byte * buffer,int len) {
  byte pal[256],*fin;
  int n;

  memset(pal,0,256);

  fin=buffer+len;
  do {
    pal[*buffer++]=1;
  } while (buffer<fin);

  for (n=0;n<256;n++) {
    if (!pal[n]) {
      dac4[n*3+0]=0;
      dac4[n*3+1]=0;
      dac4[n*3+2]=0;
    }
  }
}

int cargadac_FNT(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }
  fread(par,1,8,file);
  if (strcmp(par,"fnt\x1a\x0d\x0a\x00\x00"))
  {
    fclose(file);
    return(0);
  }
//  fseek(file,48,SEEK_SET);
  fread(dac4,768,1,file);
  fclose(file);

  return(1);
}

int cargadac_FPG(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }
  fread(par,1,8,file);
  if (strcmp(par,"fpg\x1a\x0d\x0a\x00\x00"))
  {
    fclose(file);
    return(0);
  }
//  fseek(file,48,SEEK_SET);
  fread(dac4,768,1,file);
  fclose(file);

  return(1);
}

int cargadac_PAL(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }
  fread(par,1,8,file);
  fseek(file,0,SEEK_END);
  if (strcmp(par,"pal\x1a\x0d\x0a\x00\x00") && (ftell(file)!=768))
  {
    fclose(file);
    return(0);
  }
  if(ftell(file)==768)
  {
    fseek(file,0,SEEK_SET);
    fread(dac4,768,1,file);
  }
  else
  {
    fseek(file,8,SEEK_SET);
    fread(dac4,768,1,file);
  }
  fclose(file);

  return(1);
}

int cargadac_MAP(char *name)
{
  FILE *file;
  char par[16];
  byte * buffer, * temp;
  int n,mal,man;

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }
  fread(par,1,16,file);
  if (strcmp(par,"map\x1a\x0d\x0a\x00\x00"))
  {
    fclose(file);
    return(0);
  }

  if (cargar_paleta) {
    man=*(word*)(par+8);
    mal=*(word*)(par+10);
    fseek(file,0,SEEK_END);
    n=ftell(file);
    if ((buffer=(byte *)malloc(n))==NULL) {
      fclose(file);
      return(0);
    }
    memset(buffer,0,n);
    
    fseek(file,0,SEEK_SET);
    if (fread(buffer,1,n,file)!=n) {
      free(buffer);
      fclose(file);
      return(0);
    }
    if ((temp=(byte*)malloc(man*mal+man))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }
	memset(temp,0,man*mal+man);
	
    swap(man,map_an); swap(mal,map_al);
    descomprime_MAP(buffer,temp,0);

    memcpy(paleta_original,dac4,768);

    quitar_colores(temp,map_an*map_al);

    swap(man,map_an); swap(mal,map_al);
    free(buffer);
    free(temp);

  } else {
    fseek(file,48,SEEK_SET);
    fread(dac4,768,1,file);
  }

  fclose(file);
  return(1);
}

int cargadac_PCX(char *name)
{
  FILE *file;
  int x,n;
  int man,mal;
  byte *temp;
  byte *buffer;
  pcx_header header;

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }
  if( fread((byte *)&header,1,sizeof(pcx_header),file) != sizeof(pcx_header) )
  {
    fclose(file);
    return(0);
  }

  if(header.encoding!=1) {
    fclose(file);
    return(0);
  }

  if(header.bits_per_pixel==8 && header.color_planes==1 && !cargar_paleta)
  {
    fseek(file,0,SEEK_END);
    fseek(file,ftell(file)-768,SEEK_SET);
    fread(dac4,768,1,file);
    for(x=0;x<768;x++) dac4[x]=dac4[x]/4;
  }
  else if(header.bits_per_pixel==8 && (header.color_planes==3 || header.color_planes==1))
  {
    man=header.xmax-header.xmin+1;
    mal=header.ymax-header.ymin+1;
    fseek(file,0,SEEK_END); n=ftell(file);
    if ((buffer=(byte *)malloc(n))==NULL) {
      fclose(file);
      return(0);
    }
    fseek(file,0,SEEK_SET);
    if (fread(buffer,1,n,file)!=n) {
      free(buffer);
      fclose(file);
      return(0);
    }
    if ((temp=(byte*)malloc(man*mal+man))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }

    swap(man,map_an); swap(mal,map_al);
    cargar_paleta=1;
    descomprime_PCX(buffer,temp,0);

    memcpy(paleta_original,dac4,768);

    if (header.color_planes==1) quitar_colores(temp,map_an*map_al);

    cargar_paleta=0;
    swap(man,map_an); swap(mal,map_al);
    free(buffer);
    free(temp);

  }
  else if(header.bits_per_pixel==1 && header.color_planes==4)
  {
    memcpy(dac4, header.palette16, 48);
    memset(&dac4[48], 0, 720);
    for (x=0; x<48; x++) dac4[x] /= 4;
  }
  else
  {
    fclose(file);
    return(0);
  }

  fclose(file);

  return(1);
}

int cargadac_BMP(char *name)
{
  FILE *file;
  bmpfile_header        FileHeader;
  bmpinfo_header        InfoHeader;
  rgb_quad              Bmpdac[256];
  byte *                CopiaBuffer,*buffer;
  int x,y;
  int n,man,mal;
  byte * temp;

  CopiaBuffer=(byte *)malloc(14+40+1024);
  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44]; dialogo((voidReturnType)err0);
    free(CopiaBuffer);
    return(0);
  }
  fread(CopiaBuffer,14+40+1024,1,file);

  FileHeader.bfType=*((unsigned short*)CopiaBuffer);
  FileHeader.bfSize=*((unsigned int*)(CopiaBuffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(CopiaBuffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(CopiaBuffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(CopiaBuffer+10));
  buffer=CopiaBuffer+14;
  memcpy(&InfoHeader,buffer,40);
  buffer=CopiaBuffer+54;
  free(CopiaBuffer);
  if(FileHeader.bfType!=0x4D42) {
    fclose(file);
    return(0);
  }
  if(InfoHeader.biBitCount==8 && !cargar_paleta)
  {
    if(InfoHeader.biClrUsed) memcpy(Bmpdac, buffer, 4*InfoHeader.biClrUsed);
    else                     memcpy(Bmpdac, buffer, 4*256);
  }
  else if(InfoHeader.biBitCount==4)
  {
    memset (Bmpdac, 0, 1024);
    if(InfoHeader.biClrUsed) memcpy(Bmpdac, buffer, 4*InfoHeader.biClrUsed);
    else                     memcpy(Bmpdac, buffer, 4*16);
  }
  else
  {
    man = InfoHeader.biWidth;
    mal = InfoHeader.biHeight;
    fseek(file,0,SEEK_END); n=ftell(file);
    if ((buffer=(byte *)malloc(n))==NULL) {
      fclose(file);
      return(0);
    }
    fseek(file,0,SEEK_SET);
    if (fread(buffer,1,n,file)!=n) {
      free(buffer);
      fclose(file);
      return(0);
    }
    if ((temp=(byte*)malloc(man*mal+man))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }

    swap(man,map_an); swap(mal,map_al);
    cargar_paleta=1;
    descomprime_BMP(buffer,temp,0);

    memcpy(paleta_original,dac4,768);

    if (InfoHeader.biBitCount==8) quitar_colores(temp,map_an*map_al);

    cargar_paleta=0;
    swap(man,map_an); swap(mal,map_al);
    free(buffer);
    free(temp);
    fclose(file);
    return(1);
  }
  fclose(file);

  y=0;
  for(x=0;x<256;x++)
  {
    dac4[y++]=Bmpdac[x].rgbRed/4;
    dac4[y++]=Bmpdac[x].rgbGreen/4;
    dac4[y++]=Bmpdac[x].rgbBlue/4;
  }

  return(1);
}

int cargadac_JPG(char *name)
{
#ifdef JPGLIB
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         my_err_mgr;
  int x, y, img_filesize;
  FILE *file;
  byte *buffer=NULL;

  JSAMPARRAY buffer_scanline;
  int row_stride;

  if (strstr(strupr(name),".JP")==NULL) return(0);

  cinfo.err = jpeg_std_error(&my_err_mgr);

  if (setjmp(jmp_error_ptr))
  {
    jpeg_destroy((j_common_ptr) &cinfo);
    if (buffer!=NULL) free(buffer);
    return(0);
  }

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_texto=(char *)texto[44];
    dialogo((voidReturnType)err0);
    return(0);
  }

  fseek(file,0,SEEK_END);
  img_filesize = ftell(file);
  fseek(file,0,SEEK_SET);
  if ((buffer=(byte *)malloc(img_filesize))==NULL)
  {
    v_texto=(char *)texto[45]; dialogo((voidReturnType)err0);
    fclose(file);
    return(0);
  }
  fread(buffer,1,img_filesize,file);
  fclose(file);

  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, buffer, img_filesize);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.quantize_colors=TRUE;
  if (v.click_handler==(voidReturnType)browser2) {
    cinfo.dither_mode=JDITHER_NONE;
    cinfo.two_pass_quantize=TRUE;
  } else {
    cinfo.dither_mode=JDITHER_FS;
    cinfo.two_pass_quantize=TRUE;
  }
  jpeg_start_decompress(&cinfo);

  row_stride = cinfo.output_width * cinfo.output_components;
  buffer_scanline = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  while (cinfo.output_scanline < cinfo.output_height)
  {
    jpeg_read_scanlines(&cinfo, buffer_scanline, 1);
  }

  // Pasa los coeficientes a dac4 (valores de 0 a 63)
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    y=0;
    for(x=0; x<256; x++)
    {
      dac4[y++] = x/4;
      dac4[y++] = x/4;
      dac4[y++] = x/4;
    }
  } else {
    y=0;
    for(x=0; x<256; x++)
    {
      dac4[y++] = cinfo.colormap[0][x] / 4;
      dac4[y++] = cinfo.colormap[1][x] / 4;
      dac4[y++] = cinfo.colormap[2][x] / 4;
    }
  }

  memcpy(paleta_original,dac4,768);

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  free(buffer);
  return 1;

#else
return 0;
#endif
}

