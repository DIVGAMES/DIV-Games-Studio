#include <stdlib.h>
//#include <conio.h>
#include <string.h>

#include "global.h"
#include "ifs.h"

//─────────────────────────────────────────────────────────────────────────────
//  Conversion table from Windows chars to MSDOS chars
//─────────────────────────────────────────────────────────────────────────────

char OEM2ANSI[256]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,164,0,0,0,0,182,167,0,0,0,0,0,0,0,0,0,0,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  199,252,233,226,228,224,229,231,234,235,232,239,238,236,196,197,201,230,198,244,246,242,251,249,255,214,220,248,163,216,215,131,
  225,237,243,250,241,209,170,186,191,174,172,189,188,161,171,187,95,95,95,166,166,193,194,192,169,166,166,43,43,162,165,43,
  43,45,45,43,45,43,227,195,43,43,45,45,166,45,43,164,240,208,202,203,200,105,205,206,207,43,43,95,95,166,204,95,
  211,223,212,210,245,213,181,254,222,218,219,217,253,221,175,180,173,177,95,190,182,167,247,184,176,168,183,185,179,178,95,32};

/*
char OEM2ANSI[256]={     // Old table
//Differences
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,164,  0,  0,  0,  0,
               182,167,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,
//Differences
                32, 33, 34, 35, 36, 37, 38, 39,
                40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
                70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
                90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
               100,101,102,103,104,105,106,107,108,109,
               110,111,112,113,114,115,116,117,118,119,
               120,121,122,123,124,125,126,127,
//Differences
               199,252,
               233,226,228,224,229,231,234,235,232,239,
               238,236,196,197,201,230,198,244,246,242,
               251,239,255,214,220,162,163,165,  0,131,
               225,237,243,250,241,209,170,186,191,  0,
                 0,189,188,161,181,187,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,223,  0,  0,  0,  0,
               181,  0,  0,  0,  0,167,  0,248,  0,  0,
                 0,177,  0,  0,  0,  0,247,  0,176,149,
                 0,  0,  0,178,  0,  0};
//Differences
*/

FNTHEADER FNTheader = { "DIVFNT", 0x1A };
IFSHEADER IFSheader;
FNTTABLE tableFNT[256];
IFSTABLE tableIFS[256];
IFS ifs;

char *bodyTexBuffer, *outTexBuffer, *shadowTexBuffer;

char *ptrLetter, *ptrBody, *ptrOutline, *ptrShadow;
char *Buffer=NULL, *Buffer2=NULL, *Buffer3=NULL;
char *outBuffer=NULL, *shadowBuffer=NULL;
FILE *fileIFS=NULL, *fileFNT=NULL;
short dispY,Height,Width, sizeIFS, incY;
unsigned short realwidth,realheight, bodyWidth, bodyHeight, outlineWidth, outlineHeight;


void CloseAndFreeAll()
{
//    fcloseall();
    if (fileIFS!=NULL) { fclose(fileIFS); fileIFS=NULL; }
    if (fileFNT!=NULL) { fclose(fileFNT); fileFNT=NULL; }
    if (Buffer) free(Buffer);
    if (Buffer2) free(Buffer2);
    if (Buffer3) free(Buffer3);
    if (shadowBuffer) free(shadowBuffer);
    if (outBuffer) free (outBuffer);
}


short initStruct()
{

  long  position;
    if ((fileIFS=fopen(ifs.ifsName,"rb"))==NULL)
        return(IFS_OPEN_ERROR);
    if (fread(&IFSheader,sizeof(IFSheader),1,fileIFS)<1)
        return(IFS_FORMAT_ERROR);
    if (strcmp((char *)IFSheader.id,"IFS"))
        return(IFS_FORMAT_ERROR);
    if (ifs.sizeX<8 || ifs.sizeX>128 || ifs.sizeY<8 || ifs.sizeY>128)
        return(IFS_PARAM_ERROR);

    if (ifs.sizeX==8 && ifs.sizeY==8) { position=IFSheader.offset8; sizeIFS=8; }
    else if (ifs.sizeX<=10 && ifs.sizeY<=10) { position=IFSheader.offset10; sizeIFS=10; }
    else if (ifs.sizeX<=12 && ifs.sizeY<=12) { position=IFSheader.offset12; sizeIFS=12; }
    else if (ifs.sizeX<=14 && ifs.sizeY<=14) { position=IFSheader.offset14; sizeIFS=14; }
    else { position=IFSheader.offset128; sizeIFS=128; }

    if (fseek(fileIFS,position,SEEK_SET))
        return (IFS_FORMAT_ERROR);
    if (fread(&tableIFS,sizeof(tableIFS),1,fileIFS)<1)
        return(IFS_FORMAT_ERROR);

    if (ifs.brightness>4) ifs.brightness=0;

    return (0);
}


short LoadLetter(short letter)
{
    long offset;
    short x,y,j,t;
    char rtbyte, error=0;
    short pixels;

    offset=tableIFS[letter].disp;
    if (fseek(fileIFS,offset,SEEK_SET)) error=1;
    if (fread(&Height,2,1,fileIFS)<1) error=1;
    if (fread(&dispY,2,1,fileIFS)<1) error=1;
    if (fread(&Width,2,1,fileIFS)<1) error=1;
    if (fread(&pixels,2,1,fileIFS)<1) error=1;
    if (error) return (IFS_READ_ERROR);
    t=Width*8-pixels;
    Height+=dispY;

    if (!Width || !Height) return 0;

    Buffer=(char *) realloc (Buffer, Height*Width*8+10);
    if (Buffer==NULL) return (IFS_MEM_ERROR);
    memset(Buffer, 0, Height*Width*8+10);

    for (y=0; y<Height-dispY; y++)
        for (x=0; x<Width; x++)
        {
            if (fread(&rtbyte,1,1,fileIFS)<1) return (IFS_READ_ERROR);
            for (j=0; j<8; j++)
                if (rtbyte & (1<<j))
                    Buffer[((y+dispY)*pixels + x*8+j)-t]=255;
        }

    realwidth=Width=pixels;
    realheight=Height;
    return (0);
}


short scalar()
{
    float width=Width, height=Height, width2, height2;
    float factX,factY,x,y, f;
    float x1,x2,y1,y2;
    float percent,accum;
    long pixel;
    int ix,iy,ixx,iyy;
    short cx=0,cy=0,flag=0;
    char *ptr=Buffer, *p;

    width2= (float) width * ifs.sizeX / sizeIFS;    // actual width relative
                                                     // to the original size
    height2= (float) height * ifs.sizeY / sizeIFS;

    if (ifs.sizeX==sizeIFS && ifs.sizeY==sizeIFS)         // no need to scale
    {
        ptrLetter=Buffer;
        return (0);
    }

    factX=width/width2;
    factY=height/height2;

    if (factX>=2.0 || factY>=2.0)           // scale by averages
    {
        if (factX-(int)factX==0.0 && factY-(int)factY==0.0) flag=1;

        if (factX<2.0)
            factX=1.0;
        else if (factX<4.0)
            factX=2.0;
            else if (factX<8.0)
                factX=4.0;
             else if (factX<16.0)
                factX=8;

        if (factY<2.0)
            factY=1.0;
        else if (factY<4.0)
            factY=2.0;
            else if (factY<8.0)
                factY=4.0;
             else if (factY<16.0)
                factY=8;

        f=realwidth=Width/factX;
        if (f-(int)f > 0.0) realwidth++;
        f=realheight=Height/factY;
        if (f-(int)f > 0.0) realheight++;

        Buffer2=(char *) realloc (Buffer2, (realheight+3) * (realwidth+3));
        if (Buffer2==NULL)
            return (IFS_MEM_ERROR);
        memset(Buffer2, 0, (realheight+3) * (realwidth+3));
        ptr=Buffer2;

        for (iy=0; iy<height; iy+=factY)
        {
            for (ix=0; ix<width; ix+=factX)
            {
                pixel=0;
                for (iyy=iy; iyy<iy+(int)factY; iyy++)
                    for (ixx=ix; ixx<ix+(int)factX; ixx++)
                    pixel+=Buffer[iyy*Width + ixx];
                pixel/=(factX*factY);
                if (flag)   // anti-alias filter
                    ptr[cy*realwidth+cx++]= pixel > 128 ? 255 : 0;
                else
                    ptr[cy*realwidth+cx++]=pixel;
            }
            cx=0; cy++;
        }
        if (flag)
        {
            ptrLetter=Buffer2;
            return (0);
        }
        width=realwidth;
        height=realheight;
        factX=(float) width/width2;
        factY=(float) height/height2;

    }

    realwidth=(int)width2;
    if (width2-(int)width2 > 0.5) realwidth++;
    realheight=(int)height2;

    Buffer3=(char *) realloc(Buffer3,(realwidth+3) * (realheight+3));
    if (Buffer3==NULL)
        return (IFS_MEM_ERROR);
    memset(Buffer3,0,(realwidth+3)*(realheight+3));
    cx=0;cy=0;
    for (y=0.5; y<=height+0.9; y+=factY)
    {
        for (x=0.5; x<=width+0.9; x+=factX)
        {
            ix=x; x1=ix+0.5;
            if (x-ix>=0.5) {
                x1=ix+0.5;
                x2=ix+1.5;
            } else {
                x1=ix-0.5;
                x2=ix+0.5;
            }

            iy=y; y1=iy+0.5;
            if (y-iy>=0.5) {
                y1=iy+0.5;
                y2=iy+1.5;
            } else {
                y1=iy-0.5;
                y2=iy+0.5;
            }
            p=ptr+(int)y1*(int)width+(int)x1;

            percent=2-((x-x1)*(x-x1)+(y-y1)*(y-y1));
            accum=percent;
            pixel=*p*percent; p+=(int)width;

            percent=2-((x-x1)*(x-x1)+(y-y2)*(y-y2));
            accum+=percent;
            pixel+=*p*percent; p-=(int)width-1;

            percent=2-((x-x2)*(x-x2)+(y-y1)*(y-y1));
            accum+=percent;
            pixel+=*p*percent; p+=(int)width;

            percent=2-((x-x2)*(x-x2)+(y-y2)*(y-y2));
            accum+=percent;
            pixel+=*p*percent;

            pixel=(float)pixel/accum;
            Buffer3[cy*realwidth+cx++]= pixel > 128 ? 255 : 0;
        }
        cx=0;cy++;
    }
    ptrLetter=Buffer3;
    return (0);
}


short DrawOutline()
{
  short x,y,i,cont, cont2;
  short totalWidth=bodyWidth+4+ifs.outline*2;
  short totalHeight=bodyHeight+4+ifs.outline*2;
  short outWidth=bodyWidth+ifs.outline*2;
  short outHeight=bodyHeight+1+ifs.outline*2;
  char *tmpBuffer, *ptr, *ptr2, *pun, white, black, grey;

    tmpBuffer=(char *)malloc (totalWidth*(totalHeight+1));
    outBuffer=(char *)realloc (outBuffer, outWidth*outHeight);
    if (tmpBuffer==NULL || outBuffer==NULL)
        return (IFS_MEM_ERROR);
    memset(tmpBuffer, 0, totalWidth*(totalHeight+1));
    memset(outBuffer, 0, outWidth*outHeight);
    ptr=ptrBody;
    ptrOutline=ptr2=tmpBuffer+ifs.outline+2+(ifs.outline+2)*totalWidth;
    for (y=0; y<bodyHeight; y++)
    {
        memcpy(ptr2, ptr, bodyWidth);
        ptr2+=totalWidth;
        ptr+=bodyWidth;
    }

    switch (ifs.brightness)
    {
        case 1:
        case 3: white=192; black=64; grey=128; break;
        case 2:
        case 4: white=64; black=192; grey=128; break;
    }

    pun=outBuffer+ifs.outline+ifs.outline*outWidth;

    for (i=1; i<=ifs.outline; i++)
    {
        for (y=0; y<realheight+i*2; y++)
        {
            ptr=ptrOutline-i+(y-i)*totalWidth;
            ptr2=pun-i+(y-i)*outWidth;
            for (x=0; x<realwidth+i*2; x++, ptr++, ptr2++)
            if (!*ptr)
            {
              cont=0;
              if (*(ptr+1)>=255-i) cont+=1;
              if (*(ptr+totalWidth)>=255-i) cont+=2;
              if (*(ptr-1)>=255-i) cont+=4;
              if (*(ptr-totalWidth)>=255-i) cont+=8;
              if (cont)
              {
                *ptr=254-i;
                if (!ifs.brightness) *ptr2=128;
                else if (ifs.brightness==1 || ifs.brightness==2)
                switch(cont)
                {
                  case 2:       if (*(ptr-1-totalWidth)>=255-i)
                            *ptr2=black;
                        else *ptr2=white; break;
                  case 1:
                  case 2+1:
                  case 4+2+1:
                  case 8+2+1:   *ptr2=white; break;

                  case 4:
                  case 8:
                  case 8+4:
                  case 8+4+1:
                  case 8+4+2:   *ptr2=black; break;

                  case 4+1:
                  case 8+2:
                  case 8+4+2+1: *ptr2=grey; break;

                  case 4+2:
                  case 8+1: cont2=0;
                        if (*(ptr-1-totalWidth)>=255-i) cont2+=1;
                        if (*(ptr+1+totalWidth)>=255-i) cont2+=2;
                        switch(cont2) {
                        case 0: if (cont==4+2) {
                              if (*(ptr-2-2*totalWidth)>=255-i || *(ptr-3-3*totalWidth)>=255-i)
                                *ptr2=black;
                              else
                                *ptr2=grey;
                            } else
                            if (*(ptr-1-2*totalWidth)<255-i)
                                *ptr2=white;
                            else if (*(ptr+2)<255-i)
                                *ptr2=black;
                            else
                                *ptr2=grey;
                            break;
                        case 1: *ptr2=black; break;
                        case 2: *ptr2=white; break;
                        case 2+1: *ptr2=grey; break;
                        }
                        break;
                }
                else if (ifs.brightness==3 || ifs.brightness==4)
                switch(cont)
                {
                  case 1:
                  case 8:
                  case 8+1:
                  case 4+8+1:
                  case 8+2+1:   *ptr2=black; break;

                  case 4:
                  case 2:
                  case 2+4:
                  case 2+4+1:
                  case 8+4+2:   *ptr2=white; break;

                  case 4+1:
                  case 8+2:
                  case 8+4+2+1: *ptr2=grey; break;

                  case 1+2:
                  case 8+4: cont2=0;
                        if (*(ptr-1+totalWidth)>=255-i) cont2+=1;
                        if (*(ptr+1-totalWidth)>=255-i) cont2+=2;
                        switch(cont2) {
                        case 0: if (cont==1+2) {
                              if (*(ptr+2-2*totalWidth)>=255-i)
                                *ptr2=black;
                              else
                                *ptr2=grey;
                            } else
                            if (*(ptr+1-2*totalWidth)<255-i)
                                *ptr2=white;
                            else if (*(ptr-2)<255-i)
                                *ptr2=black;
                            else
                                *ptr2=grey;
                            break;
                        case 1: *ptr2=white; break;
                        case 2: *ptr2=black; break;
                        case 2+1: *ptr2=grey; break;
                        }
                        break;
                }
              }
            }
        }
    }

    ptrLetter=outBuffer;
    realwidth+=(ifs.outline*2);
    realheight+=(ifs.outline*2);
    free (tmpBuffer);
    return (0);
}


short DrawShadow()
{
  char *ptr, *ptr2;
  short x, y;
  short absShadowX=abs(ifs.shadowX), absShadowY=abs(ifs.shadowY);

    shadowBuffer=(char *)realloc (shadowBuffer, (realwidth+absShadowX+1) * (realheight+absShadowY+1));
    if (shadowBuffer==NULL) return (IFS_MEM_ERROR);
    memset (shadowBuffer, 0, (realwidth+absShadowX+1)*(realheight+absShadowY+1));

    if (ifs.shadowY>0) {
      ptr2=shadowBuffer + absShadowY * (realwidth+absShadowX);
    } else ptr2=shadowBuffer;
    if (ifs.shadowX>0) ptr2+=absShadowX;

/*
    if (ifs.shadowX < 0 && ifs.shadowY < 0)
        ptr2=shadowBuffer;
    else if (ifs.shadowX > 0 && ifs.shadowY > 0)
            ptr2=shadowBuffer + absShadowX + absShadowY * (realwidth+absShadowX);
    else if (ifs.shadowX > 0)
            ptr2=shadowBuffer + absShadowX;
    else
        ptr2=shadowBuffer + absShadowY*(realwidth+absShadowX);
*/

    ptrShadow=ptr2;

    ptr=ptrOutline;
    for (y=0; y<outlineHeight; y++)
    {
        for (x=0; x<outlineWidth; x++, ptr++, ptr2++)
            if (*ptr) *ptr2=ifs.shadowTexColor;
        ptr2+=absShadowX;
    }

    ptrLetter=shadowBuffer;
    realwidth+=absShadowX;
    realheight+=absShadowY;
    return (0);
}


void textureBody()
{
    char *ptr;
    short ix,iy,xx,yy;
    float factX, factY, x, y;

    if (!ifs.bodyTexWidth || !ifs.bodyTexHeight) return;

    ptr=ptrBody+incY*bodyWidth;

    if (ifs.bodyTexWidth==1 && ifs.bodyTexHeight==1)    // texture=1 color
    {
        for (iy=0; iy<bodyHeight-incY; iy++)
            for (ix=0; ix<bodyWidth; ix++, ptr++)
                if (*ptr) *ptr=ifs.bodyTexColor;
        return;
    }

    if (ifs.bodyTexMode==1)                           // scale texture
    {
        factX=(float) ifs.bodyTexWidth/bodyWidth;
        factY=(float) ifs.bodyTexHeight/(bodyHeight-incY);

        for (y=0, yy=0; yy<bodyHeight-incY; y+=factY, yy++)
            for (x=0, xx=0; xx<bodyWidth; x+=factX, xx++, ptr++)
            if (*ptr) *ptr=bodyTexBuffer[(int)y*ifs.bodyTexWidth+(int)x];
    }
    else                                                // tiled texture
        for (yy=0, iy=0; yy<bodyHeight-incY; yy++)
        {
            for (xx=0, ix=0; xx<bodyWidth; xx++, ptr++)
            {
                if (*ptr) *ptr=bodyTexBuffer[iy*ifs.bodyTexWidth+ix];
                if (++ix>=ifs.bodyTexWidth) ix=0;
            }
            if (++iy>=ifs.bodyTexHeight) iy=0;
        }
}


void textureOutline()
{
  char *ptr, color;
  short x, y , ix, iy;
  float factX, factY, fx, fy;

    ptr=ptrOutline+incY*outlineWidth;

    if (ifs.outTexWidth<2 && ifs.outTexHeight<2)
    {
        for (y=0; y<outlineHeight-incY; y++)
            for (x=0; x<outlineWidth; x++, ptr++)
                if (*ptr)
                {
                    if (*ptr<128) *ptr=ifs.dark[ifs.outTexColor];
                    else if (*ptr>128) *ptr=ifs.light[ifs.outTexColor];
                    else *ptr=ifs.outTexColor;
                }
        return;
    }

    if (ifs.outTexMode==1)                     // scale texture
    {
        factX=(float) ifs.outTexWidth/outlineWidth;
        factY=(float) ifs.outTexHeight/(outlineHeight-incY);

        for (y=0, fy=0; y<outlineHeight-incY; y++, fy+=factY)
            for (x=0, fx=0; x<outlineWidth; x++, fx+=factX, ptr++)
                if (*ptr)
                {
                    color=outTexBuffer[(int)fy*ifs.outTexWidth+(int)fx];
                    if (*ptr<128) *ptr=ifs.dark[color];
                    else if (*ptr>128) *ptr=ifs.light[color];
                    else *ptr=color;
                }

    }
    else
        for (y=0, iy=0; y<outlineHeight-incY; y++)        // tiled texture
        {
            for (x=0, ix=0; x<outlineWidth; x++, ptr++)
            {
                if (*ptr)
                {
                    color=outTexBuffer[iy*ifs.outTexWidth+ix];
                    if (*ptr<128) *ptr=ifs.dark[color];
                    else if (*ptr>128) *ptr=ifs.light[color];
                    else *ptr=color;
                }
                if (++ix>=ifs.outTexWidth) ix=0;
            }
            if (++iy>=ifs.outTexHeight) iy=0;
        }
}


void textureShadow()
{
  char *ptr;
  short x,y,ix,iy;
  float factX, factY, fx, fy;
  short absShadowX=abs(ifs.shadowX);

    if (ifs.shadowTexWidth<2 && ifs.shadowTexHeight<2) return;

    ptr=ptrShadow+incY*(outlineWidth+absShadowX);

    if (ifs.shadowTexMode==1)                     // scale texture
    {
        factX=(float) ifs.shadowTexWidth/outlineWidth;
        factY=(float) ifs.shadowTexHeight/(outlineHeight-incY);

        for (y=0, fy=0; y<outlineHeight-incY; y++, fy+=factY)
        {
            for (x=0, fx=0; x<outlineWidth; x++, fx+=factX, ptr++)
            if (*ptr) *ptr=shadowTexBuffer[(int)fy*ifs.shadowTexWidth+(int)fx];
            ptr+=absShadowX;
        }
    }
    else
    for (y=0, iy=0; y<outlineHeight-incY; y++)            // tiled texture
    {
        for (x=0, ix=0; x<outlineWidth; x++, ptr++)
        {
            if (*ptr) *ptr=shadowTexBuffer[iy*ifs.shadowTexWidth+ix];
            if (++ix>=ifs.shadowTexWidth) ix=0;
        }
        if (++iy>=ifs.shadowTexHeight) iy=0;
        ptr+=absShadowX;
    }
}


void joinOutlineWithBody(void) { // Add the body to the outline
  char *ptr, *ptr2, c, color, realcolor;
  short x,y,n;

  ptr=ptrBody-bodyWidth-1;
  ptr2=ptrOutline+ifs.outline-1+(ifs.outline-1)*outlineWidth;
  for (y=-1; y<=bodyHeight; y++) {
    for (x=-1; x<=bodyWidth; x++, ptr++, ptr2++) {

      if (x>=0 && y>=0 && x<bodyWidth && y<bodyHeight) {
        if (*ptr) {
          *ptr2=*ptr;
          continue;
        }
      }
      if ((realcolor=*ptr2)) {
        color=0; n=0;
        if (y<bodyHeight && y>=0) {
          if (x>0) if ((c=*(ptr-1))) {
            color=c; n++;
          }
          if (x<bodyWidth-1) if ((c=*(ptr+1))) {
          if (color) color=*(ghost+color*256+c); else color=c; n++;
          }
        }
        if (x<bodyWidth && x>=0) {
          if (y<bodyHeight-1) if ((c=*(ptr+bodyWidth))) {
            if (color) color=*(ghost+color*256+c); else color=c; n++;
          }
          if (y>0) if ((c=*(ptr-bodyWidth))) {
            if (color) color=*(ghost+color*256+c); else color=c; n++;
          }
        }
        switch(n) {
          case 0:
            break;
          case 1:
            color=*(ghost+realcolor*256+color);
            realcolor=*(ghost+realcolor*256+color);
            break;
          case 2:
            realcolor=*(ghost+realcolor*256+color);
            break;
          case 3:
          case 4:
            realcolor=*(ghost+realcolor*256+color);
            realcolor=*(ghost+realcolor*256+color);
            break;
        }
        *ptr2=realcolor;
      }

    }
    ptr-=2;
    ptr2+=ifs.outline*2-2;
  }
}


void joinShadowWithRest()
{
  char *ptr, *ptr2;
  short x,y;
  short absShadowX=abs(ifs.shadowX), absShadowY=abs(ifs.shadowY);

    ptr=ptrOutline;

    if (ifs.shadowY<0) {
      ptr2=shadowBuffer + absShadowY*realwidth;
    } else ptr2=shadowBuffer;
    if (ifs.shadowX<0) ptr2+=absShadowX;

/*
    if (ifs.shadowX < 0 && ifs.shadowY < 0)
        ptr2=shadowBuffer + absShadowX + absShadowY*realwidth;
    else if (ifs.shadowX > 0 && ifs.shadowY > 0)
        ptr2=shadowBuffer;
    else if (ifs.shadowX > 0)
        ptr2=shadowBuffer + absShadowY*realwidth;
    else
        ptr2=shadowBuffer + absShadowX;
*/

    for (y=0; y<outlineHeight; y++)
    {
        for (x=0; x<outlineWidth; x++, ptr++, ptr2++)
            if (*ptr) *ptr2=*ptr;
        ptr2+=absShadowX;
    }
}

int Jorge_Creates_the_font(int GenCode)
{
  char error=0, stop;
  short x, ret;
  short j;

    Buffer=NULL; Buffer2=NULL; Buffer3=NULL;
    outBuffer=NULL; shadowBuffer=NULL;
    if ((ret=initStruct()))
    {
        CloseAndFreeAll();
        return (ret);
    }


    if ((fileFNT=fopen(ifs.fntName,"wb"))==NULL)
    {
        CloseAndFreeAll();
        return (IFS_OPEN_ERROR);
    }
    fwrite("fnt\x1a\x0d\x0a\x00\x00",8,1,fileFNT);
    fwrite(dac,768,1,fileFNT);
    fwrite(rules,sizeof(rules),1,fileFNT);
    fwrite(&GenCode,1,4,fileFNT);

    if (fwrite(tableFNT,sizeof(tableFNT),1,fileFNT)<1) {
        error=1;
	}

    for (x=0, error=0; x<256 && !error; x++)
    {

        Progress((char *)texts[217],x,256);

        tableFNT[x].width=tableFNT[x].height=0;
        tableFNT[x].offset=0;
        if (ifs.table[x])
        {
            realwidth=0;
            realheight=0;
            incY=0;
//            if (OEM2ANSI[x]!=0)
            if (x!=0)
            {
//             if ((ret=LoadLetter(OEM2ANSI[x])))
             if ((ret=LoadLetter(x)))
             {
                CloseAndFreeAll();
                return (ret);
             }
             if (Height && Width)
             {
                if ((ret=scalar()))
                {
                    Progress((char *)texts[217],256,256);
                    CloseAndFreeAll();
                    return (ret);
                }
                ptrOutline=ptrBody=ptrLetter;
                outlineWidth=bodyWidth=realwidth;
                outlineHeight=bodyHeight=realheight;

                stop=0;
                for (incY=0; !stop && incY<realheight; incY++)
                    for (j=0; !stop && j<realwidth; j++)
                        if (ptrLetter[incY*realwidth+j]) stop=1;
                incY--;
//                incY=bodyHeight*dispY/Height;

                if (ifs.outline)
                {
                    if ((ret=DrawOutline()))
                    {
                        Progress((char *)texts[217],256,256);
                        CloseAndFreeAll();
                        return (ret);
                    }
                    ptrOutline=ptrLetter;
                    outlineWidth=realwidth;
                    outlineHeight=realheight;
                    textureBody();
                    textureOutline();
                    joinOutlineWithBody();
                }
               else
                textureBody();

                if (ifs.shadowX || ifs.shadowY)
                {
                    if ((ret=DrawShadow()))
                    {
                        Progress((char *)texts[217],256,256);
                        CloseAndFreeAll();
                        return (ret);
                    }
                    textureShadow();
                    joinShadowWithRest();
                }
            }

            if (realwidth<=0 || incY>=realheight) {
              tableFNT[x].width=0;
              tableFNT[x].height=0;
              tableFNT[x].incY=0;
              tableFNT[x].offset=0;
            } else {
              tableFNT[x].width=realwidth;
              tableFNT[x].height=realheight-incY;
              tableFNT[x].incY=incY;
              tableFNT[x].offset=ftell(fileFNT);
              if (fwrite(ptrLetter+incY*realwidth,tableFNT[x].width * tableFNT[x].height,1,fileFNT)<1) error=1;
            }

           }
        }
    }

    if (error)
    {
        Progress((char *)texts[217],256,256);
        CloseAndFreeAll();
        return (IFS_WRITE_ERROR);
    }

//    tableFNT[' '].width=ifs.sizeX;

    fseek(fileFNT,8+768+sizeof(rules)+4,SEEK_SET);
    fwrite(&tableFNT,sizeof(tableFNT),1,fileFNT);

    Progress((char *)texts[217],256,256);
    CloseAndFreeAll();
    return(0);
}

void GetCharSize(int WhatChar,int *width,int *height)
{
FILE *fileFnt;
        *width=4;
        *height=1;

        if ((fileFnt=fopen("PREVIEW.FNT","rb"))==NULL)
                return;
        fseek(fileFnt,8+768+sizeof(rules)+4,SEEK_SET);
        if (fread(tableFNT,sizeof(tableFNT),1,fileFnt)<1)
        {
                fclose(fileFnt);
                return;
        }
        *height=tableFNT[WhatChar].incY+tableFNT[WhatChar].height;
        if(WhatChar==32)
                *width=tableFNT[WhatChar].width/2;
        else
                *width=tableFNT[WhatChar].width+1;
        fclose(fileFnt);
return;
}
void GetCharSizeBuffer(int WhatChar,int *width,int *height,char *buffer)
{
        *width=4;
        *height=1;

        memcpy(tableFNT,buffer+8+768+sizeof(rules)+4,sizeof(tableFNT));
        *height=tableFNT[WhatChar].incY+tableFNT[WhatChar].height;
        *width=tableFNT[WhatChar].width+1;
return;
}
int ShowChar(int WhatChar,int cx,int cy,char *ptr,int w)
{
int y,iy;
FILE *fileFnt;
char *rawBuffer;
        if ((fileFnt=fopen("PREVIEW.FNT","rb"))==NULL)
                return 4;
        fseek(fileFnt,8+768+sizeof(rules)+4,SEEK_SET);
        if (fread(tableFNT,sizeof(tableFNT),1,fileFnt)<1)
        {
                fclose(fileFnt);
                return 4;
        }
        if (tableFNT[WhatChar].width*tableFNT[WhatChar].height)
        {
            rawBuffer=(char *) malloc(tableFNT[WhatChar].width*tableFNT[WhatChar].height);
            if (rawBuffer==NULL)
            {
                fclose(fileFnt);
                if(WhatChar==32)
                        return (tableFNT[WhatChar].width/2);
                else
                        return (tableFNT[WhatChar].width+1);
            }
            if (fseek(fileFnt,tableFNT[WhatChar].offset,SEEK_SET))
            {
                free(rawBuffer);
                fclose(fileFnt);
                if(WhatChar==32)
                        return (tableFNT[WhatChar].width/2);
                else
                        return (tableFNT[WhatChar].width+1);
            }
            if (fread(rawBuffer,tableFNT[WhatChar].width*tableFNT[WhatChar].height,1,fileFnt)<1)
            {
                free(rawBuffer);
                fclose(fileFnt);
                if(WhatChar==32)
                        return (tableFNT[WhatChar].width/2);
                else
                        return (tableFNT[WhatChar].width+1);

            }
            iy=tableFNT[WhatChar].incY;
            for (y=0; y<tableFNT[WhatChar].height; y++)
                memcpy(ptr+((cy+iy)*w+cx)+y*w,rawBuffer+y*tableFNT[WhatChar].width,tableFNT[WhatChar].width);

            fclose(fileFnt);
            free(rawBuffer);
            if(WhatChar==32)
                return (tableFNT[WhatChar].width/2);
            else
                return (tableFNT[WhatChar].width+1);
        } else fclose(fileFnt);
if(WhatChar==32)
        return (tableFNT[WhatChar].width/2);
else
        return (tableFNT[WhatChar].width+1);

}

int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int w,char *buffer)
{
int y,iy,x,c;
char *rawBuffer;
        memcpy(tableFNT,buffer+8+768+sizeof(rules)+4,sizeof(tableFNT));
        if (tableFNT[WhatChar].width*tableFNT[WhatChar].height)
        {
            rawBuffer=(char *) malloc(tableFNT[WhatChar].width*tableFNT[WhatChar].height);
            if (rawBuffer==NULL)
            {
                if(WhatChar==32)
                        return (tableFNT[WhatChar].width/2);
                else
                        return (tableFNT[WhatChar].width+1);
            }
            memcpy(rawBuffer,buffer+tableFNT[WhatChar].offset,tableFNT[WhatChar].width*tableFNT[WhatChar].height);
            iy=tableFNT[WhatChar].incY;
            for (y=0; y<tableFNT[WhatChar].height; y++) {
                for (x=0; x<tableFNT[WhatChar].width; x++) {
                  if ((c=rawBuffer[y*tableFNT[WhatChar].width+x])) {
                    ptr[(cy+iy)*w+cx+x+y*w]=c;
                  }
                }
//                memcpy(ptr+((cy+iy)*w+cx)+y*w,rawBuffer+y*tableFNT[WhatChar].width,tableFNT[WhatChar].width);
            }
            free(rawBuffer);
            if(WhatChar==32)
                return (tableFNT[WhatChar].width/2);
            else
                return (tableFNT[WhatChar].width+1);
        }
return (tableFNT[WhatChar].width+1);
}

void ConvertFntToPal(char *buffer)
{
char DacFnt[768];
int accum=0,x,a,b;
byte xlat[256];
        memcpy(DacFnt,buffer+8,768);

        for(x=0;x<768;x++) if(dac[x]!=DacFnt[x]) accum=1;
        if(!accum)
                return;

        create_dac4();
        xlat[0]=0;
        for (x=1;x<256;x++)
                xlat[x]=find_color_not0(DacFnt[x*3],DacFnt[x*3+1],DacFnt[x*3+2]);

        memcpy(tableFNT,buffer+8+768+sizeof(rules)+4,sizeof(tableFNT));

        for(x=0;x<256;x++)
        {
                if (tableFNT[x].width*tableFNT[x].height)
                        for(a=0;a<tableFNT[x].height;a++)
                                for(b=0;b<tableFNT[x].width;b++)
                                {
                                        accum=*(buffer+tableFNT[x].offset+a*tableFNT[x].width+b);
                                        *(buffer+tableFNT[x].offset+a*tableFNT[x].width+b)=xlat[accum];
                                }
        }
        memcpy(buffer+8,dac,768);
}
