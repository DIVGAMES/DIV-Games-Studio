
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Module containing the read/write code for graphic formats
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
//      Constants
// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

#define BI_RGB      0
#define BI_RLE8     1
#define BI_RLE4     2

#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

// ââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      Structures
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
// DESC: Returns TRUE if the file loaded into the buffer is of type XYZ, and also 
// returns the width / height of the bitmap in the variables map_width / p

// PROT: void decompresses_XYZ (byte * buffer, byte * map vent int);
// DESC: The function receives the loaded file in buffer and another buffer
// Map_an * map_height + map_width bytes (map), you have to unpack this
// latest.
// "Vent" indicates whether to charge the additional information window
// (Such as checkpoints, see already defined formats)
// If the file has palette, it must be stored in the buffer
// 768 bytes called dac4 []

// PROT: int record_XYZ (byte * Map, FILE * f);
// DESC: You must save the bitmap to the file. Besides the buffer itself
// Containing the map, you have access to the following information:
// - map_width, map_height (map width and height in pixels)
// - Dac [768] (palette file, 256 RGB values between 0 and 63)
// When the file being saved is a MAP (own format) you also have
// - map_code (number associated with the map, long integer, 4 bytes)
// - map center x / y (center of the map)
// - map description [32] (descriptive comment of the map)
// The function returns TRUE if the file has been recorded without any problems,
// Or FALSE if there was not enough disk space.
// At the start, "f" should be assumed to be a binary file open for writing
// Must be written on with fwrite, and in the end should not close

// NOTE: Any function that requires additional memory for your
// Job, you can ask to malloc, and if you can not get the memory
// Required, then you must create an informative error window
// As follows: I = text v_text [45]; dialogue ((voidReturnType) Err0);

// NOTE 2: The only format that returns a value from decompress is the JPG. NOTE !!!

// - Finally you must call the three functions created in divhandl.cpp
// And divbrow.cpp (look, for example, "MAP" on these two sources) and put
// New prototypes Global.cpp (where n is the above) 
// It also has to be added to divsetup.cpp and divpalet.cpp (preparing Skin)

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      MAP Format
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

int is_MAP (byte * buffer) {
	if (!strcmp((char *)buffer,"map\x1a\x0d\x0a")) {
		map_width=*(word*)(buffer+8);
		map_height=*(word*)(buffer+10);
		return(1);
	} else 
		return(0);
}

void decompress_MAP (byte * buffer, byte * map, int win) {
	
	short npoints;
	if (win) {
		memcpy(&Code,buffer+12,4);
		memcpy(Description,buffer+16,32);
	}

	memcpy(dac4,buffer+48,768);
	memcpy(&npoints,buffer+1392,2);
	
	if (win) {
		memcpy(rules,buffer+816,sizeof(rules));
		if(npoints!=0)
			memcpy(v_map->points,buffer+1394,npoints*4);
	}
	
	memcpy(map,buffer+1394+(npoints*4),map_width*map_height);
}

int save_MAP (byte * map, FILE * f) {

	word x,npoints=0;
	int y;
	int i;

	fwrite("map\x1a\x0d\x0a\x00\x00",8,1,f);      // +000 Header and version
	x=map_width; fwrite(&x,2,1,f);                   // +008 Width
	x=map_height; fwrite(&x,2,1,f);                   // +010 Height
	y=window[v_window].map->Code; fwrite(&y,4,1,f);// +012 Code

	fwrite(window[v_window].map->description,32,1,f);// +016 Description
	fwrite(dac,768,1,f);                          // +048 Palette
	fwrite(rules,1,sizeof(rules),f);            // +816 Color rules

	npoints=0;

	for(i=511;i>=0;i-=2)
		if(window[v_window].map->points[i]!=-1) {
				npoints=(i+1)/2;
				i=-1;
		}

	fwrite(&npoints,2,1,f);                     // +1392 Number of points
	fwrite(&window[v_window].map->points,npoints,4,f);

	y=map_width*map_height;

	do { 
		if (y>=768) {
			fwrite(map,768,1,f); y-=768; map+=768;
		} else {
			fwrite(map,y,1,f); y=0;
		} 
	} while (y);

	return 0;
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      PCX format
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

int is_PCX(byte *buffer)
{
	int matches=0;

	if(buffer[2]==1) {

		if(buffer[3]==8 && (buffer[65]==1 || buffer[65]==3)) 
			matches=1;
		if(buffer[3]==1 && (buffer[65]==4)) 
			matches=1;
	}

	map_width=*(word*)(buffer+8)-*(word*)(buffer+4)+1;
	map_height=*(word*)(buffer+10)-*(word*)(buffer+6)+1;

	return(matches);
}

extern byte * sample;
extern byte new_palette[768];
extern int num_colors;
void create_palette(void);
void browser2(void);
byte * decompress_rle(byte * buffer,unsigned int bytes_line,unsigned int last_byte,byte * pDest);

int load_palette=0;

void decompress_PCX(byte *buffer, byte *map, int win)
{
  unsigned int ctr;
//unsigned int fExit=0;
  unsigned int pixel=0, pixel_line=0;
  unsigned int last_byte,bytes_line;
  char ch, rep;
  pcx_header header;
  byte *pSrc, *pDest, *pSrcLine, *AuxPtr;
  int  rgb_color, x, y;
  byte rgb_blue, rgb_green, rgb_red;
  byte color16;
  int  bit;
  rgb_quad Pcxdac[256];
  byte *old_sample;

  memcpy((byte *)&header,buffer,sizeof(pcx_header));
  buffer+=128;                                  // Start of the image

  map_width = header.xmax - header.xmin + 1;
  map_height = header.ymax - header.ymin + 1;

  if((!map_width && !map_height) || map_width<0 || map_height<0) return;

  memset (map, 0, map_width * map_height);

  last_byte  = header.bytes_per_line * header.color_planes * map_height ;
  bytes_line = header.bytes_per_line * header.color_planes;

  // If it is a 24bpp image
  if(header.color_planes==3)
  {
    if ((pDest=(byte *)malloc((map_width+1)*(map_height+1)*3))==NULL)
    {
      v_text=(char *)texts[45];
      dialog((voidReturnType)err0);
      return;
    }
    memset (pDest, 0, (map_width+1)*(map_height+1)*3);
    pSrc = pDest;
  }
  // If it is a 4bpp image
  else if(header.color_planes==4)
  {
    if ((pDest=(byte *)malloc(last_byte))==NULL)
    {
      v_text=(char *)texts[45];
      dialog((voidReturnType)err0);
      return;
    }
    memset (pDest, 0, last_byte);
    pSrc = pDest;
  }
  else pDest = map;

  if(header.bits_per_pixel!=8 || header.color_planes!=1) // If it is not 8bpp
  {
    do {
      ch=*buffer++;                               // Copy one by default.
      if((ch&192)==192) {                         // If RLE then
        rep=(ch&63);                              // rep = number of times to copy.
        ch=*buffer++;
      } else rep=1;
      pixel+=rep;                                 // Make sure we do not go out of bounds.
      if(pixel>last_byte) {
        rep-=pixel-last_byte;
        for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
        break;
      }
      for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
    } while(1);
  }

  if(header.bits_per_pixel==8 && header.color_planes==1)
  {
    do {
      ch=*buffer++;                               // Copy one by default.
      if((ch&192)==192) {                         // If RLE then
        rep=(ch&63);                              // rep = number of times to copy.
        ch=*buffer++;
      } else rep=1;
      pixel+=rep;                                 // Make sure we do not go out of bounds.
      pixel_line+=rep;
      if(pixel>last_byte) {
        rep-=pixel-last_byte;
        for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
        break;
      }
      if(pixel_line==bytes_line) {
        pixel_line=0;
        rep-=bytes_line-map_width;
      }
      for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
    } while(1);

    memcpy(dac4, buffer, 768);
    for (ctr=0; ctr<768; ctr++) dac4[ctr] /= 4;
  }
  else if(header.bits_per_pixel==1 && header.color_planes==4)
  {
    // Convert from 4 to 8
    pDest    = map;
    AuxPtr   = pSrc;
    pSrcLine = pSrc;
    for (y=0; y<map_height; y++)                    // For each line...
    {
      pSrc  = pSrcLine;
      pixel = map_width;
      for (x=0; x<header.bytes_per_line; x++) // For each byte...
      {
        for (bit=7; bit>=0 && pixel; bit--)        // For each pixel...
        {
          color16  = 0;
          color16 |= (((*(pSrc)                         & (1 << bit)) >> bit) );
          color16 |= (((*(pSrc+header.bytes_per_line)   & (1 << bit)) >> bit) << 1);
          color16 |= (((*(pSrc+header.bytes_per_line*2) & (1 << bit)) >> bit) << 2);
          color16 |= (((*(pSrc+header.bytes_per_line*3) & (1 << bit)) >> bit) << 3);
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
    for (ctr=0; ctr<48; ctr++) dac4[ctr] /= 4;
  }
  else                                          // If it is 24bpp.
  {

    old_sample=sample;
    sample=(byte*)malloc(32768);

    if ((v.click_handler==(voidReturnType)browser2 && !load_palette) || sample==NULL) {

      if (sample!=NULL) free(sample);
      sample=old_sample;

      // Create the 3-3-2 palette
      for(rgb_color=0; rgb_color<256; rgb_color++) {
        Pcxdac[rgb_color].rgbRed   = (rgb_color & 0xE0) >> 5;
        Pcxdac[rgb_color].rgbGreen = (rgb_color & 0x1C) >> 2;
        Pcxdac[rgb_color].rgbBlue  = (rgb_color & 0x03);
      }

      // Convert from 24 to 8
      pDest    = map;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_height; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_width; x++)
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

      // Move the coefficients to dac4 (values from 0 to 63)
      for(x=0; x<256; x++) {
        dac4[x*3+0] = Pcxdac[x].rgbRed   *  9;
        dac4[x*3+1] = Pcxdac[x].rgbGreen *  9;
        dac4[x*3+2] = Pcxdac[x].rgbBlue  * 21;
      }

    } else {

      memset(sample,0,32768);

      // Convert from 24 to 8
      pDest    = map;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_height; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_width; x++)
        { // For each pixel...
          rgb_red   = *(pSrc);
          rgb_green = *(pSrc + header.bytes_per_line);
          rgb_blue  = *(pSrc + header.bytes_per_line * 2);
          sample[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)]=1;
          pDest ++;
          pSrc  ++;
        }
        pSrcLine += header.bytes_per_line * 3;
      }

      pSrc=AuxPtr;

      create_palette();

      // Convert from 24 to 8
      pDest    = map;
      AuxPtr   = pSrc;
      pSrcLine = pSrc;
      for (y=0; y<map_height; y++)
      { // For each line...
        pSrc = pSrcLine;
        for (x=0; x<map_width; x++)
        { // For each pixel...
          rgb_red   = *(pSrc);
          rgb_green = *(pSrc + header.bytes_per_line);
          rgb_blue  = *(pSrc + header.bytes_per_line * 2);
          *pDest = sample[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)];
          pDest ++;
          pSrc  ++;
        }
        pSrcLine += header.bytes_per_line * 3;
      }

      free(sample); free(pSrc=AuxPtr);
      sample=old_sample;

      memcpy(&dac4[0],&new_palette[0],768);

    }

  }
  if (win) {
    for(x=0;x<512;x++) v_map->points[x]=-1;
    Code=0;
    Description[0]=0;
  }
}

int save_PCX(byte *map,FILE *f) {
  byte p[768];
  int x;
  byte *cbuffer;
  struct pcx_struct pcx;
  int ptr=0;
  int cptr=0;
  int Overflow=0;
  char ActPixel;
  char cntPixel=0;
  char Smallpalette=12;

        pcx.header.manufacturer=10;
        pcx.header.version=5;
        pcx.header.encoding=1;
        pcx.header.bits_per_pixel=8;
        pcx.header.xmin=0;
        pcx.header.ymin=0;
        pcx.header.xmax=map_width-1;
        pcx.header.ymax=map_height-1;
        pcx.header.hres=map_width;
        pcx.header.vres=map_height;
        pcx.header.color_planes=1;
        pcx.header.bytes_per_line=map_width;
        pcx.header.palette_type=0;
        pcx.header.Hresol=map_width;
        pcx.header.Vresol=map_height;

        if ((cbuffer=(unsigned char *)malloc(map_width*map_height*2))==NULL)
        {
                //Error allocating memory.
                v_text=(char *)texts[45]; dialog((voidReturnType)err0);
                return(1);
        }
        ActPixel=map[ptr];
        while (ptr < map_width*map_height)
        {
                while((map[ptr]==ActPixel) && (ptr<map_width*map_height))
                {
                        cntPixel++;
                        Overflow++;
                        ptr++;
                        if(Overflow==map_width)
                        {
                                Overflow=0;
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
                ActPixel=map[ptr];
                cntPixel=0;
        }

        fwrite(&pcx.header,1,sizeof(pcx_header),f);
        fwrite(cbuffer,1,cptr,f);
        fwrite(&Smallpalette,1,1,f);
        for (x=0;x<768;x++) p[x]=dac[x]*4;
        fwrite(p,1,768,f);
        free(cbuffer);
        return(0);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//  Optimized RLE decompression routine
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

byte * decompress_rle(byte * buffer,unsigned int bytes_line,unsigned int last_byte,byte * pDest) {

  unsigned int ctr;
  unsigned int pixel=0, pixel_line=0;
  char ch, rep;

  do {
    ch=*buffer++;                               // Copy one by default.
    if((ch&192)==192) {                         // If RLE then
      rep=(ch&63);                              // rep = number of times to copy.
      ch=*buffer++;
    } else rep=1;
    pixel+=rep;                                 // Make sure we do not go out of bounds.
    pixel_line+=rep;
    if(pixel>last_byte) {
      rep-=pixel-last_byte;
      for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
      break;
    }
    if(pixel_line==bytes_line) {
      pixel_line=0;
      if(bytes_line&1) buffer++;
    }
    for(ctr=0;ctr<rep;ctr++) *pDest++=ch;
  } while(1);

  return(buffer);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      BMP format
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

int is_BMP(byte *buffer)
{
  bmpfile_header FileHeader;
  bmpinfo_header InfoHeader;
  byte           *CopyBuffer;

  FileHeader.bfType=*((unsigned short*)buffer);
  FileHeader.bfSize=*((unsigned int*)(buffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(buffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(buffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(buffer+10));

  if(FileHeader.bfType!=0x4D42) return(0);
  CopyBuffer=buffer+14;
  memcpy(&InfoHeader,CopyBuffer,40);
//if(InfoHeader.biCompression) if(InfoHeader.biCompression!=BI_RLE8) return(0);
  if(InfoHeader.biBitCount!=4 &&
     InfoHeader.biBitCount!=8 &&
     InfoHeader.biBitCount!=24) return(0);

  map_width=InfoHeader.biWidth;
  map_height=InfoHeader.biHeight;

  return(1);
}

void decompress_BMP(byte *buffer, byte *map, int win)
{
  bmpfile_header FileHeader;
  bmpinfo_header InfoHeader;
  rgb_quad       Bmpdac[256];
  byte bEOL;    // 1 if end of line reached.
  byte bEOF=0;  // 1 if end of file reached.
  byte *pSrc, *pSrcLine, *pDest;
  byte *AuxPtr;
  int x, y, fixmap_width, rgb_color, num_colors;
  byte rgb_red, rgb_green, rgb_blue, RunLength, pixel;
  byte *old_sample;

  FileHeader.bfType=*((unsigned short*)buffer);
  FileHeader.bfSize=*((unsigned int*)(buffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(buffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(buffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(buffer+10));

  pSrc=buffer+14;
  memcpy(&InfoHeader,pSrc,40);
  map_width = InfoHeader.biWidth;
  map_height = InfoHeader.biHeight;

  if((!map_width && !map_height) || map_width<0 || map_height<0) return;

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

  fixmap_width=map_width; if(map_width%4) fixmap_width+=4-(map_width%4);

  pDest = map;

  memset (pDest, 0, map_width * map_height);

  // Decompress the BMP if necessary

  if(InfoHeader.biBitCount==8)
  {
    if(InfoHeader.biCompression==BI_RLE8)
    {
      for (y=0; y<map_height && !bEOF; y++)
      { // For each line...
        pDest = &map[map_width*map_height-map_width*(y+1)];
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
              case 2:  // Delta (Not supported)
                bEOL = 1;
                bEOF = 1;
                break;
              default: // Absolute copy of bytes (from 3 to 255)
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
      for(y=0;y<map_height;y++)
      {
        memcpy(map+y*map_width,pSrc+((map_height-1)-y)*fixmap_width,map_width);
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
      for (y=0; y<map_height && !bEOF; y++)
      { // For each line...
        pDest = &map[map_width*map_height-map_width*(y+1)];
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
              case 2:  // Delta (Not supported)
                bEOL = 1;
                bEOF = 1;
                break;
              default: // Absolute copy of bytes (from 3 to 255)
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
      fixmap_width /= 2;
      if(map_width & 1) fixmap_width += 2;
      for(y=0;y<map_height;y++)
      {
        for(x=0; x<fixmap_width; x++)
        {
          pixel = pSrc[((map_height-1)-y)*fixmap_width+x];
          map[y*map_width+x*2]   = (pixel >> 4);
          map[y*map_width+x*2+1] = (pixel  & 0x0F);
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
  else if(InfoHeader.biBitCount==24) // 24 bpp BMP
  {
    old_sample=sample;
    sample=(byte*)malloc(32768);

    if ((v.click_handler==(voidReturnType)browser2&&!load_palette) || sample==NULL) {

      if (sample!=NULL) free(sample);
      sample=old_sample;

      // Create the 3-3-2 palette
      for(rgb_color=0; rgb_color<256; rgb_color++)
      {
        Bmpdac[rgb_color].rgbRed   = (rgb_color & 0xE0) >> 5;
        Bmpdac[rgb_color].rgbGreen = (rgb_color & 0x1C) >> 2;
        Bmpdac[rgb_color].rgbBlue  = (rgb_color & 0x03);
      }

      // Convert from 24 to 8
      fixmap_width=((map_width*3)/4)*4;
      if(fixmap_width != map_width*3) fixmap_width+=4;
      for (y=0; y<map_height; y++)
      { // For each line...
        pDest = &map[map_width*map_height-map_width*(y+1)];
        pSrc  = pSrcLine;
        for (x=0; x<map_width; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          *pDest = (rgb_red&0xE0) | ((rgb_green&0xE0)>>3) | ((rgb_blue&0xC0)>>6);
          pDest ++;
          pSrc += 3;
        }
        pSrcLine += fixmap_width;
      }

      // Move the coefficients to dac4 (values from 0 to 63)
      y=0;
      for(x=0; x<256; x++)
      {
        dac4[y++] = Bmpdac[x].rgbRed   *  9;
        dac4[y++] = Bmpdac[x].rgbGreen *  9;
        dac4[y++] = Bmpdac[x].rgbBlue  * 21;
      }

    } else {

      memset(sample,0,32768);

      // Convert from 24 to 8
      AuxPtr=pSrc;
      fixmap_width=((map_width*3)/4)*4;
      if(fixmap_width != map_width*3) fixmap_width+=4;
      for (y=0; y<map_height; y++)
      { // For each line...
        pSrc  = pSrcLine;
        for (x=0; x<map_width; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          sample[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)]=1;
          pSrc += 3;
        }
        pSrcLine += fixmap_width;
      } pSrc=pSrcLine=AuxPtr;

      create_palette();

      // Convert from 24 to 8
      fixmap_width=((map_width*3)/4)*4;
      if(fixmap_width != map_width*3) fixmap_width+=4;
      for (y=0; y<map_height; y++)
      { // For each line...
        pDest = &map[map_width*map_height-map_width*(y+1)];
        pSrc  = pSrcLine;
        for (x=0; x<map_width; x++)
        { // For each pixel...
          rgb_red   = ((rgb_quad *)pSrc)->rgbRed;
          rgb_green = ((rgb_quad *)pSrc)->rgbGreen;
          rgb_blue  = ((rgb_quad *)pSrc)->rgbBlue;
          *pDest = sample[((rgb_red&0xF8)<<7) | ((rgb_green&0xF8)<<2) | ((rgb_blue&0xF8)>>3)];
          pDest ++;
          pSrc += 3;
        }
        pSrcLine += fixmap_width;
      }

      free(sample);
      sample=old_sample;
      memcpy(&dac4[0],&new_palette[0],768);

    }
  }

  if (win) {
    for(x=0;x<512;x++) v_map->points[x]=-1;
    Code=0;
    Description[0]=0;
  }
}

int save_BMP(byte *map,FILE *f)
{
bmpfile_header     FileHeader;
bmpinfo_header     InfoHeader;
rgb_quad           Bmpdac[256];
int x,y=0;

byte pad[4]={0,0,0,0};
int pad_width = map_width + 4-(map_width%4);

//BITMAPFILE_HEADER
        FileHeader.bfType=0x4D42;
        FileHeader.bfSize=1078+pad_width*map_height;
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
        InfoHeader.biWidth=map_width;
        InfoHeader.biHeight=map_height;
        InfoHeader.biPlanes=1;
        InfoHeader.biBitCount=8;
        InfoHeader.biCompression=0;
        InfoHeader.biSizeImage=pad_width*map_height;
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
        for(x=0;x<map_height;x++) {
			fwrite(map+((map_height-1)-x)*map_width,1,map_width,f);
			if(pad_width!=map_width)
				fwrite(pad,1,pad_width-map_width,f);
		}
return(1);
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//      JPG format
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

int is_JPG(byte *buffer, int img_filesize)
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
  map_width = cinfo.output_width;
  map_height = cinfo.output_height;
  jpeg_destroy_decompress(&cinfo);

  return(1);
#else
return (0);
#endif

}

int decompress_JPG(byte *buffer, byte *map, int win, int img_filesize)
{
	
#ifdef JPGLIB
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr         my_err_mgr;
  
  JSAMPARRAY buffer_scanline;
  int row_stride;
  int x, y;

  win=win;
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

  map_width = cinfo.output_width;
  map_height = cinfo.output_height;

  if((!map_width && !map_height) || map_width<0 || map_height<0)
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
    memcpy(&map[(cinfo.output_scanline-1)*cinfo.output_width],
           buffer_scanline[0], row_stride);
  }
  // Move the coefficients to dac4 (values from 0 to 63)

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

  if (win) {
    for(x=0;x<512;x++) v_map->points[x]=-1;
    Code=0;
    Description[0]=0;
  }

  return(1);
#else 
return (0);
#endif
}

//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ
//  Functions to load the palette from a file
//âââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââââ

// Remove from dac4 the colors not used in the map
// NOTE, whoever calls this function must save and restore "original_palette"

void remove_colors(byte * buffer,int len) {
  byte pal[256],*end;
  int n;

  memset(pal,0,256);

  end=buffer+len;
  do {
    pal[*buffer++]=1;
  } while (buffer<end);

  for (n=0;n<256;n++) {
    if (!pal[n]) {
      dac4[n*3+0]=0;
      dac4[n*3+1]=0;
      dac4[n*3+2]=0;
    }
  }
}

int loaddac_FNT(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
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

int loaddac_FPG(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
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

int loaddac_PAL(char *name)
{
  FILE *file;
  char par[8];

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
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

int loaddac_MAP(char *name)
{
  FILE *file;
  char par[16];
  byte * buffer, * temp;
  int n,mh,mw;

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
    return(0);
  }
  fread(par,1,16,file);
  if (strcmp(par,"map\x1a\x0d\x0a\x00\x00"))
  {
    fclose(file);
    return(0);
  }

  if (load_palette) {
    mw=*(word*)(par+8);
    mh=*(word*)(par+10);
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
    if ((temp=(byte*)malloc(mw*mh+mw))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }
	memset(temp,0,mw*mh+mw);
	
    swap(mw,map_width); swap(mh,map_height);
    decompress_MAP(buffer,temp,0);

    memcpy(original_palette,dac4,768);

    remove_colors(temp,map_width*map_height);

    swap(mw,map_width); swap(mh,map_height);
    free(buffer);
    free(temp);

  } else {
    fseek(file,48,SEEK_SET);
    fread(dac4,768,1,file);
  }

  fclose(file);
  return(1);
}

int loaddac_PCX(char *name)
{
  FILE *file;
  int x,n;
  int mw,mh;
  byte *temp;
  byte *buffer;
  pcx_header header;

  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
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

  if(header.bits_per_pixel==8 && header.color_planes==1 && !load_palette)
  {
    fseek(file,0,SEEK_END);
    fseek(file,ftell(file)-768,SEEK_SET);
    fread(dac4,768,1,file);
    for(x=0;x<768;x++) dac4[x]=dac4[x]/4;
  }
  else if(header.bits_per_pixel==8 && (header.color_planes==3 || header.color_planes==1))
  {
    mw=header.xmax-header.xmin+1;
    mh=header.ymax-header.ymin+1;
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
    if ((temp=(byte*)malloc(mw*mh+mw))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }

    swap(mw,map_width); swap(mh,map_height);
    load_palette=1;
    decompress_PCX(buffer,temp,0);

    memcpy(original_palette,dac4,768);

    if (header.color_planes==1) remove_colors(temp,map_width*map_height);

    load_palette=0;
    swap(mw,map_width); swap(mh,map_height);
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

int loaddac_BMP(char *name)
{
  FILE *file;
  bmpfile_header        FileHeader;
  bmpinfo_header        InfoHeader;
  rgb_quad              Bmpdac[256];
  byte *                CopyBuffer,*buffer;
  int x,y;
  int n,mw,mh;
  byte * temp;

  CopyBuffer=(byte *)malloc(14+40+1024);
  file=fopen(name,"rb");
  if(file==NULL)
  {
    v_text=(char *)texts[44]; dialog((voidReturnType)err0);
    free(CopyBuffer);
    return(0);
  }
  fread(CopyBuffer,14+40+1024,1,file);

  FileHeader.bfType=*((unsigned short*)CopyBuffer);
  FileHeader.bfSize=*((unsigned int*)(CopyBuffer+2));
  FileHeader.bfReserved1=*((unsigned short*)(CopyBuffer+6));
  FileHeader.bfReserved2=*((unsigned short*)(CopyBuffer+8));
  FileHeader.bfOffBits=*((unsigned int *)(CopyBuffer+10));
  buffer=CopyBuffer+14;
  memcpy(&InfoHeader,buffer,40);
  buffer=CopyBuffer+54;
  free(CopyBuffer);
  if(FileHeader.bfType!=0x4D42) {
    fclose(file);
    return(0);
  }
  if(InfoHeader.biBitCount==8 && !load_palette)
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
    mw = InfoHeader.biWidth;
    mh = InfoHeader.biHeight;
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
    if ((temp=(byte*)malloc(mw*mh+mw))==NULL) {
      free(buffer);
      fclose(file);
      return(0);
    }

    swap(mw,map_width); swap(mh,map_height);
    load_palette=1;
    decompress_BMP(buffer,temp,0);

    memcpy(original_palette,dac4,768);

    if (InfoHeader.biBitCount==8) remove_colors(temp,map_width*map_height);

    load_palette=0;
    swap(mw,map_width); swap(mh,map_height);
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

int loaddac_JPG(char *name)
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
    v_text=(char *)texts[44];
    dialog((voidReturnType)err0);
    return(0);
  }

  fseek(file,0,SEEK_END);
  img_filesize = ftell(file);
  fseek(file,0,SEEK_SET);
  if ((buffer=(byte *)malloc(img_filesize))==NULL)
  {
    v_text=(char *)texts[45]; dialog((voidReturnType)err0);
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

  // Move the coefficients to dac4 (values from 0 to 63)
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

  memcpy(original_palette,dac4,768);

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  free(buffer);
  return 1;

#else
return 0;
#endif
}

