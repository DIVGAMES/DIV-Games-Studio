#include <stdlib.h>
//#include <conio.h>
#include <string.h>

#include "global.h"
#include "ifs.h"

//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
//  Tabla de conversi줻 de chars Windows a chars MSDOS
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

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
char OEM2ANSI[256]={     // Tabla antigua
//Diferencias
                 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,  0,  0,  0,164,  0,  0,  0,  0,
               182,167,  0,  0,  0,  0,  0,  0,  0,  0,
                 0,  0,
//Diferencias
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
//Diferencias
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
//Diferencias
*/

FNTHEADER FNTheader = { "DIVFNT", 0x1A };
IFSHEADER IFSheader;
TABLAFNT tablaFNT[256];
TABLAIFS tablaIFS[256];
IFS ifs;

char *bodyTexBuffer, *outTexBuffer, *shadowTexBuffer;

char *ptrLetra, *ptrBody, *ptrOutline, *ptrShadow;
char *Buffer=NULL, *Buffer2=NULL, *Buffer3=NULL;
char *outBuffer=NULL, *shadowBuffer=NULL;
FILE *fichIFS=NULL, *fichFNT=NULL;
short despY,Alto,Ancho, sizeIFS, incY;
unsigned short anchoreal,altoreal, anchoBody, altoBody, anchoOutline, altoOutline;


void CloseAndFreeAll()
{
//    fcloseall();
    if (fichIFS!=NULL) { fclose(fichIFS); fichIFS=NULL; }
    if (fichFNT!=NULL) { fclose(fichFNT); fichFNT=NULL; }
    if (Buffer) free(Buffer);
    if (Buffer2) free(Buffer2);
    if (Buffer3) free(Buffer3);
    if (shadowBuffer) free(shadowBuffer);
    if (outBuffer) free (outBuffer);
}


short initStruct()
{

  long  posicion;
    if ((fichIFS=fopen(ifs.ifsName,"rb"))==NULL)
        return(IFS_OPEN_ERROR);
    if (fread(&IFSheader,sizeof(IFSheader),1,fichIFS)<1)
        return(IFS_FORMAT_ERROR);
    if (strcmp((char *)IFSheader.id,"IFS"))
        return(IFS_FORMAT_ERROR);
    if (ifs.tamX<8 || ifs.tamX>128 || ifs.tamY<8 || ifs.tamY>128)
        return(IFS_PARAM_ERROR);

    if (ifs.tamX==8 && ifs.tamY==8) { posicion=IFSheader.offset8; sizeIFS=8; }
    else if (ifs.tamX<=10 && ifs.tamY<=10) { posicion=IFSheader.offset10; sizeIFS=10; }
    else if (ifs.tamX<=12 && ifs.tamY<=12) { posicion=IFSheader.offset12; sizeIFS=12; }
    else if (ifs.tamX<=14 && ifs.tamY<=14) { posicion=IFSheader.offset14; sizeIFS=14; }
    else { posicion=IFSheader.offset128; sizeIFS=128; }

    if (fseek(fichIFS,posicion,SEEK_SET))
        return (IFS_FORMAT_ERROR);
    if (fread(&tablaIFS,sizeof(tablaIFS),1,fichIFS)<1)
        return(IFS_FORMAT_ERROR);

    if (ifs.brillo>4) ifs.brillo=0;

    return (0);
}


short CargaLetra(short letra)
{
    long offset;
    short x,y,j,t;
    char rtbyte, error=0;
    short pixels;

    offset=tablaIFS[letra].desp;
    if (fseek(fichIFS,offset,SEEK_SET)) error=1;
    if (fread(&Alto,2,1,fichIFS)<1) error=1;
    if (fread(&despY,2,1,fichIFS)<1) error=1;
    if (fread(&Ancho,2,1,fichIFS)<1) error=1;
    if (fread(&pixels,2,1,fichIFS)<1) error=1;
    if (error) return (IFS_READ_ERROR);
    t=Ancho*8-pixels;
    Alto+=despY;

    if (!Ancho || !Alto) return 0;

    Buffer=(char *) realloc (Buffer, Alto*Ancho*8+10);
    if (Buffer==NULL) return (IFS_MEM_ERROR);
    memset(Buffer, 0, Alto*Ancho*8+10);

    for (y=0; y<Alto-despY; y++)
        for (x=0; x<Ancho; x++)
        {
            if (fread(&rtbyte,1,1,fichIFS)<1) return (IFS_READ_ERROR);
            for (j=0; j<8; j++)
                if (rtbyte & (1<<j))
                    Buffer[((y+despY)*pixels + x*8+j)-t]=255;
        }

    anchoreal=Ancho=pixels;
    altoreal=Alto;
    return (0);
}


short escalar()
{
    float ancho=Ancho, alto=Alto, ancho2, alto2;
    float factX,factY,x,y, f;
    float x1,x2,y1,y2;
    float porcien,acum;
    long pixel;
    int ix,iy,ixx,iyy;
    short cx=0,cy=0,flag=0;
    char *ptr=Buffer, *p;

    ancho2= (float) ancho * ifs.tamX / sizeIFS;    // ancho real en relaci줻
                                                     // al tama쨚 original
    alto2= (float) alto * ifs.tamY / sizeIFS;

    if (ifs.tamX==sizeIFS && ifs.tamY==sizeIFS)         // no hace falta escalar
    {
        ptrLetra=Buffer;
        return (0);
    }

    factX=ancho/ancho2;
    factY=alto/alto2;

    if (factX>=2.0 || factY>=2.0)           // escalar por medias
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

        f=anchoreal=Ancho/factX;
        if (f-(int)f > 0.0) anchoreal++;
        f=altoreal=Alto/factY;
        if (f-(int)f > 0.0) altoreal++;

        Buffer2=(char *) realloc (Buffer2, (altoreal+3) * (anchoreal+3));
        if (Buffer2==NULL)
            return (IFS_MEM_ERROR);
        memset(Buffer2, 0, (altoreal+3) * (anchoreal+3));
        ptr=Buffer2;

        for (iy=0; iy<alto; iy+=factY)
        {
            for (ix=0; ix<ancho; ix+=factX)
            {
                pixel=0;
                for (iyy=iy; iyy<iy+(int)factY; iyy++)
                    for (ixx=ix; ixx<ix+(int)factX; ixx++)
                    pixel+=Buffer[iyy*Ancho + ixx];
                pixel/=(factX*factY);
                if (flag)   // filtrar anti-alias
                    ptr[cy*anchoreal+cx++]= pixel > 128 ? 255 : 0;
                else
                    ptr[cy*anchoreal+cx++]=pixel;
            }
            cx=0; cy++;
        }
        if (flag)
        {
            ptrLetra=Buffer2;
            return (0);
        }
        ancho=anchoreal;
        alto=altoreal;
        factX=(float) ancho/ancho2;
        factY=(float) alto/alto2;

    }

    anchoreal=(int)ancho2;
    if (ancho2-(int)ancho2 > 0.5) anchoreal++;
    altoreal=(int)alto2;

    Buffer3=(char *) realloc(Buffer3,(anchoreal+3) * (altoreal+3));
    if (Buffer3==NULL)
        return (IFS_MEM_ERROR);
    memset(Buffer3,0,(anchoreal+3)*(altoreal+3));
    cx=0;cy=0;
    for (y=0.5; y<=alto+0.9; y+=factY)
    {
        for (x=0.5; x<=ancho+0.9; x+=factX)
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
            p=ptr+(int)y1*(int)ancho+(int)x1;

            porcien=2-((x-x1)*(x-x1)+(y-y1)*(y-y1));
            acum=porcien;
            pixel=*p*porcien; p+=(int)ancho;

            porcien=2-((x-x1)*(x-x1)+(y-y2)*(y-y2));
            acum+=porcien;
            pixel+=*p*porcien; p-=(int)ancho-1;

            porcien=2-((x-x2)*(x-x2)+(y-y1)*(y-y1));
            acum+=porcien;
            pixel+=*p*porcien; p+=(int)ancho;

            porcien=2-((x-x2)*(x-x2)+(y-y2)*(y-y2));
            acum+=porcien;
            pixel+=*p*porcien;

            pixel=(float)pixel/acum;
            Buffer3[cy*anchoreal+cx++]= pixel > 128 ? 255 : 0;
        }
        cx=0;cy++;
    }
    ptrLetra=Buffer3;
    return (0);
}


short PintaOutline()
{
  short x,y,i,cont, cont2;
  short anchoTotal=anchoBody+4+ifs.outline*2;
  short altoTotal=altoBody+4+ifs.outline*2;
  short anchoOut=anchoBody+ifs.outline*2;
  short altoOut=altoBody+1+ifs.outline*2;
  char *tmpBuffer, *ptr, *ptr2, *pun, blanco, negro, gris;

    tmpBuffer=(char *)malloc (anchoTotal*(altoTotal+1));
    outBuffer=(char *)realloc (outBuffer, anchoOut*altoOut);
    if (tmpBuffer==NULL || outBuffer==NULL)
        return (IFS_MEM_ERROR);
    memset(tmpBuffer, 0, anchoTotal*(altoTotal+1));
    memset(outBuffer, 0, anchoOut*altoOut);
    ptr=ptrBody;
    ptrOutline=ptr2=tmpBuffer+ifs.outline+2+(ifs.outline+2)*anchoTotal;
    for (y=0; y<altoBody; y++)
    {
        memcpy(ptr2, ptr, anchoBody);
        ptr2+=anchoTotal;
        ptr+=anchoBody;
    }

    switch (ifs.brillo)
    {
        case 1:
        case 3: blanco=192; negro=64; gris=128; break;
        case 2:
        case 4: blanco=64; negro=192; gris=128; break;
    }

    pun=outBuffer+ifs.outline+ifs.outline*anchoOut;

    for (i=1; i<=ifs.outline; i++)
    {
        for (y=0; y<altoreal+i*2; y++)
        {
            ptr=ptrOutline-i+(y-i)*anchoTotal;
            ptr2=pun-i+(y-i)*anchoOut;
            for (x=0; x<anchoreal+i*2; x++, ptr++, ptr2++)
            if (!*ptr)
            {
              cont=0;
              if (*(ptr+1)>=255-i) cont+=1;
              if (*(ptr+anchoTotal)>=255-i) cont+=2;
              if (*(ptr-1)>=255-i) cont+=4;
              if (*(ptr-anchoTotal)>=255-i) cont+=8;
              if (cont)
              {
                *ptr=254-i;
                if (!ifs.brillo) *ptr2=128;
                else if (ifs.brillo==1 || ifs.brillo==2)
                switch(cont)
                {
                  case 2:       if (*(ptr-1-anchoTotal)>=255-i)
                            *ptr2=negro;
                        else *ptr2=blanco; break;
                  case 1:
                  case 2+1:
                  case 4+2+1:
                  case 8+2+1:   *ptr2=blanco; break;

                  case 4:
                  case 8:
                  case 8+4:
                  case 8+4+1:
                  case 8+4+2:   *ptr2=negro; break;

                  case 4+1:
                  case 8+2:
                  case 8+4+2+1: *ptr2=gris; break;

                  case 4+2:
                  case 8+1: cont2=0;
                        if (*(ptr-1-anchoTotal)>=255-i) cont2+=1;
                        if (*(ptr+1+anchoTotal)>=255-i) cont2+=2;
                        switch(cont2) {
                        case 0: if (cont==4+2) {
                              if (*(ptr-2-2*anchoTotal)>=255-i || *(ptr-3-3*anchoTotal)>=255-i)
                                *ptr2=negro;
                              else
                                *ptr2=gris;
                            } else
                            if (*(ptr-1-2*anchoTotal)<255-i)
                                *ptr2=blanco;
                            else if (*(ptr+2)<255-i)
                                *ptr2=negro;
                            else
                                *ptr2=gris;
                            break;
                        case 1: *ptr2=negro; break;
                        case 2: *ptr2=blanco; break;
                        case 2+1: *ptr2=gris; break;
                        }
                        break;
                }
                else if (ifs.brillo==3 || ifs.brillo==4)
                switch(cont)
                {
                  case 1:
                  case 8:
                  case 8+1:
                  case 4+8+1:
                  case 8+2+1:   *ptr2=negro; break;

                  case 4:
                  case 2:
                  case 2+4:
                  case 2+4+1:
                  case 8+4+2:   *ptr2=blanco; break;

                  case 4+1:
                  case 8+2:
                  case 8+4+2+1: *ptr2=gris; break;

                  case 1+2:
                  case 8+4: cont2=0;
                        if (*(ptr-1+anchoTotal)>=255-i) cont2+=1;
                        if (*(ptr+1-anchoTotal)>=255-i) cont2+=2;
                        switch(cont2) {
                        case 0: if (cont==1+2) {
                              if (*(ptr+2-2*anchoTotal)>=255-i)
                                *ptr2=negro;
                              else
                                *ptr2=gris;
                            } else
                            if (*(ptr+1-2*anchoTotal)<255-i)
                                *ptr2=blanco;
                            else if (*(ptr-2)<255-i)
                                *ptr2=negro;
                            else
                                *ptr2=gris;
                            break;
                        case 1: *ptr2=blanco; break;
                        case 2: *ptr2=negro; break;
                        case 2+1: *ptr2=gris; break;
                        }
                        break;
                }
              }
            }
        }
    }

    ptrLetra=outBuffer;
    anchoreal+=(ifs.outline*2);
    altoreal+=(ifs.outline*2);
    free (tmpBuffer);
    return (0);
}


short PintaSombra()
{
  char *ptr, *ptr2;
  short x, y;
  short absSombraX=abs(ifs.sombraX), absSombraY=abs(ifs.sombraY);

    shadowBuffer=(char *)realloc (shadowBuffer, (anchoreal+absSombraX+1) * (altoreal+absSombraY+1));
    if (shadowBuffer==NULL) return (IFS_MEM_ERROR);
    memset (shadowBuffer, 0, (anchoreal+absSombraX+1)*(altoreal+absSombraY+1));

    if (ifs.sombraY>0) {
      ptr2=shadowBuffer + absSombraY * (anchoreal+absSombraX);
    } else ptr2=shadowBuffer;
    if (ifs.sombraX>0) ptr2+=absSombraX;

/*
    if (ifs.sombraX < 0 && ifs.sombraY < 0)
        ptr2=shadowBuffer;
    else if (ifs.sombraX > 0 && ifs.sombraY > 0)
            ptr2=shadowBuffer + absSombraX + absSombraY * (anchoreal+absSombraX);
    else if (ifs.sombraX > 0)
            ptr2=shadowBuffer + absSombraX;
    else
        ptr2=shadowBuffer + absSombraY*(anchoreal+absSombraX);
*/

    ptrShadow=ptr2;

    ptr=ptrOutline;
    for (y=0; y<altoOutline; y++)
    {
        for (x=0; x<anchoOutline; x++, ptr++, ptr2++)
            if (*ptr) *ptr2=ifs.shadowTexColor;
        ptr2+=absSombraX;
    }

    ptrLetra=shadowBuffer;
    anchoreal+=absSombraX;
    altoreal+=absSombraY;
    return (0);
}


void texturarBody()
{
    char *ptr;
    short ix,iy,xx,yy;
    float factX, factY, x, y;

    if (!ifs.bodyTexAncho || !ifs.bodyTexAlto) return;

    ptr=ptrBody+incY*anchoBody;

    if (ifs.bodyTexAncho==1 && ifs.bodyTexAlto==1)    // textura=1 color
    {
        for (iy=0; iy<altoBody-incY; iy++)
            for (ix=0; ix<anchoBody; ix++, ptr++)
                if (*ptr) *ptr=ifs.bodyTexColor;
        return;
    }

    if (ifs.bodyTexModo==1)                           // escalar textura
    {
        factX=(float) ifs.bodyTexAncho/anchoBody;
        factY=(float) ifs.bodyTexAlto/(altoBody-incY);

        for (y=0, yy=0; yy<altoBody-incY; y+=factY, yy++)
            for (x=0, xx=0; xx<anchoBody; x+=factX, xx++, ptr++)
            if (*ptr) *ptr=bodyTexBuffer[(int)y*ifs.bodyTexAncho+(int)x];
    }
    else                                                // textura en tile
        for (yy=0, iy=0; yy<altoBody-incY; yy++)
        {
            for (xx=0, ix=0; xx<anchoBody; xx++, ptr++)
            {
                if (*ptr) *ptr=bodyTexBuffer[iy*ifs.bodyTexAncho+ix];
                if (++ix>=ifs.bodyTexAncho) ix=0;
            }
            if (++iy>=ifs.bodyTexAlto) iy=0;
        }
}


void texturarOutline()
{
  char *ptr, color;
  short x, y , ix, iy;
  float factX, factY, fx, fy;

    ptr=ptrOutline+incY*anchoOutline;

    if (ifs.outTexAncho<2 && ifs.outTexAlto<2)
    {
        for (y=0; y<altoOutline-incY; y++)
            for (x=0; x<anchoOutline; x++, ptr++)
                if (*ptr)
                {
                    if (*ptr<128) *ptr=ifs.oscuros[ifs.outTexColor];
                    else if (*ptr>128) *ptr=ifs.claros[ifs.outTexColor];
                    else *ptr=ifs.outTexColor;
                }
        return;
    }

    if (ifs.outTexModo==1)                     // escalar textura
    {
        factX=(float) ifs.outTexAncho/anchoOutline;
        factY=(float) ifs.outTexAlto/(altoOutline-incY);

        for (y=0, fy=0; y<altoOutline-incY; y++, fy+=factY)
            for (x=0, fx=0; x<anchoOutline; x++, fx+=factX, ptr++)
                if (*ptr)
                {
                    color=outTexBuffer[(int)fy*ifs.outTexAncho+(int)fx];
                    if (*ptr<128) *ptr=ifs.oscuros[color];
                    else if (*ptr>128) *ptr=ifs.claros[color];
                    else *ptr=color;
                }

    }
    else
        for (y=0, iy=0; y<altoOutline-incY; y++)        // textura en tile
        {
            for (x=0, ix=0; x<anchoOutline; x++, ptr++)
            {
                if (*ptr)
                {
                    color=outTexBuffer[iy*ifs.outTexAncho+ix];
                    if (*ptr<128) *ptr=ifs.oscuros[color];
                    else if (*ptr>128) *ptr=ifs.claros[color];
                    else *ptr=color;
                }
                if (++ix>=ifs.outTexAncho) ix=0;
            }
            if (++iy>=ifs.outTexAlto) iy=0;
        }
}


void texturarSombra()
{
  char *ptr;
  short x,y,ix,iy;
  float factX, factY, fx, fy;
  short absSombraX=abs(ifs.sombraX);

    if (ifs.shadowTexAncho<2 && ifs.shadowTexAlto<2) return;

    ptr=ptrShadow+incY*(anchoOutline+absSombraX);

    if (ifs.shadowTexModo==1)                     // escalar textura
    {
        factX=(float) ifs.shadowTexAncho/anchoOutline;
        factY=(float) ifs.shadowTexAlto/(altoOutline-incY);

        for (y=0, fy=0; y<altoOutline-incY; y++, fy+=factY)
        {
            for (x=0, fx=0; x<anchoOutline; x++, fx+=factX, ptr++)
            if (*ptr) *ptr=shadowTexBuffer[(int)fy*ifs.shadowTexAncho+(int)fx];
            ptr+=absSombraX;
        }
    }
    else
    for (y=0, iy=0; y<altoOutline-incY; y++)            // textura en tile
    {
        for (x=0, ix=0; x<anchoOutline; x++, ptr++)
        {
            if (*ptr) *ptr=shadowTexBuffer[iy*ifs.shadowTexAncho+ix];
            if (++ix>=ifs.shadowTexAncho) ix=0;
        }
        if (++iy>=ifs.shadowTexAlto) iy=0;
        ptr+=absSombraX;
    }
}


void unirOutlineConBody(void) { // Le a쨅de el body al outline
  char *ptr, *ptr2, c, color, realcolor;
  short x,y,n;

  ptr=ptrBody-anchoBody-1;
  ptr2=ptrOutline+ifs.outline-1+(ifs.outline-1)*anchoOutline;
  for (y=-1; y<=altoBody; y++) {
    for (x=-1; x<=anchoBody; x++, ptr++, ptr2++) {

      if (x>=0 && y>=0 && x<anchoBody && y<altoBody) {
        if (*ptr) {
          *ptr2=*ptr;
          continue;
        }
      }
      if ((realcolor=*ptr2)) {
        color=0; n=0;
        if (y<altoBody && y>=0) {
          if (x>0) if ((c=*(ptr-1))) {
            color=c; n++;
          }
          if (x<anchoBody-1) if ((c=*(ptr+1))) {
          if (color) color=*(ghost+color*256+c); else color=c; n++;
          }
        }
        if (x<anchoBody && x>=0) {
          if (y<altoBody-1) if ((c=*(ptr+anchoBody))) {
            if (color) color=*(ghost+color*256+c); else color=c; n++;
          }
          if (y>0) if ((c=*(ptr-anchoBody))) {
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


void unirSombraConResto()
{
  char *ptr, *ptr2;
  short x,y;
  short absSombraX=abs(ifs.sombraX), absSombraY=abs(ifs.sombraY);

    ptr=ptrOutline;

    if (ifs.sombraY<0) {
      ptr2=shadowBuffer + absSombraY*anchoreal;
    } else ptr2=shadowBuffer;
    if (ifs.sombraX<0) ptr2+=absSombraX;

/*
    if (ifs.sombraX < 0 && ifs.sombraY < 0)
        ptr2=shadowBuffer + absSombraX + absSombraY*anchoreal;
    else if (ifs.sombraX > 0 && ifs.sombraY > 0)
        ptr2=shadowBuffer;
    else if (ifs.sombraX > 0)
        ptr2=shadowBuffer + absSombraY*anchoreal;
    else
        ptr2=shadowBuffer + absSombraX;
*/

    for (y=0; y<altoOutline; y++)
    {
        for (x=0; x<anchoOutline; x++, ptr++, ptr2++)
            if (*ptr) *ptr2=*ptr;
        ptr2+=absSombraX;
    }
}

int Jorge_Crea_el_font(int GenCode)
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


    if ((fichFNT=fopen(ifs.fntName,"wb"))==NULL)
    {
        CloseAndFreeAll();
        return (IFS_OPEN_ERROR);
    }
    fwrite("fnt\x1a\x0d\x0a\x00\x00",8,1,fichFNT);
    fwrite(dac,768,1,fichFNT);
    fwrite(reglas,sizeof(reglas),1,fichFNT);
    fwrite(&GenCode,1,4,fichFNT);

    if (fwrite(tablaFNT,sizeof(tablaFNT),1,fichFNT)<1) {
        error=1;
	}

    for (x=0, error=0; x<256 && !error; x++)
    {

        Progress((char *)texto[217],x,256);

        tablaFNT[x].ancho=tablaFNT[x].alto=0;
        tablaFNT[x].offset=0;
        if (ifs.tabla[x])
        {
            anchoreal=0;
            altoreal=0;
            incY=0;
//            if (OEM2ANSI[x]!=0)
            if (x!=0)
            {
//             if ((ret=CargaLetra(OEM2ANSI[x])))
             if ((ret=CargaLetra(x)))
             {
                CloseAndFreeAll();
                return (ret);
             }
             if (Alto && Ancho)
             {
                if ((ret=escalar()))
                {
                    Progress((char *)texto[217],256,256);
                    CloseAndFreeAll();
                    return (ret);
                }
                ptrOutline=ptrBody=ptrLetra;
                anchoOutline=anchoBody=anchoreal;
                altoOutline=altoBody=altoreal;

                stop=0;
                for (incY=0; !stop && incY<altoreal; incY++)
                    for (j=0; !stop && j<anchoreal; j++)
                        if (ptrLetra[incY*anchoreal+j]) stop=1;
                incY--;
//                incY=altoBody*despY/Alto;

                if (ifs.outline)
                {
                    if ((ret=PintaOutline()))
                    {
                        Progress((char *)texto[217],256,256);
                        CloseAndFreeAll();
                        return (ret);
                    }
                    ptrOutline=ptrLetra;
                    anchoOutline=anchoreal;
                    altoOutline=altoreal;
                    texturarBody();
                    texturarOutline();
                    unirOutlineConBody();
                }
               else
                texturarBody();

                if (ifs.sombraX || ifs.sombraY)
                {
                    if ((ret=PintaSombra()))
                    {
                        Progress((char *)texto[217],256,256);
                        CloseAndFreeAll();
                        return (ret);
                    }
                    texturarSombra();
                    unirSombraConResto();
                }
            }

            if (anchoreal<=0 || incY>=altoreal) {
              tablaFNT[x].ancho=0;
              tablaFNT[x].alto=0;
              tablaFNT[x].incY=0;
              tablaFNT[x].offset=0;
            } else {
              tablaFNT[x].ancho=anchoreal;
              tablaFNT[x].alto=altoreal-incY;
              tablaFNT[x].incY=incY;
              tablaFNT[x].offset=ftell(fichFNT);
              if (fwrite(ptrLetra+incY*anchoreal,tablaFNT[x].ancho * tablaFNT[x].alto,1,fichFNT)<1) error=1;
            }

           }
        }
    }

    if (error)
    {
        Progress((char *)texto[217],256,256);
        CloseAndFreeAll();
        return (IFS_WRITE_ERROR);
    }

//    tablaFNT[' '].ancho=ifs.tamX;

    fseek(fichFNT,8+768+sizeof(reglas)+4,SEEK_SET);
    fwrite(&tablaFNT,sizeof(tablaFNT),1,fichFNT);

    Progress((char *)texto[217],256,256);
    CloseAndFreeAll();
    return(0);
}

void GetCharSize(int WhatChar,int *ancho,int *alto)
{
FILE *fichFnt;
        *ancho=4;
        *alto=1;

        if ((fichFnt=fopen("PREVIEW.FNT","rb"))==NULL)
                return;
        fseek(fichFnt,8+768+sizeof(reglas)+4,SEEK_SET);
        if (fread(tablaFNT,sizeof(tablaFNT),1,fichFnt)<1)
        {
                fclose(fichFnt);
                return;
        }
        *alto=tablaFNT[WhatChar].incY+tablaFNT[WhatChar].alto;
        if(WhatChar==32)
                *ancho=tablaFNT[WhatChar].ancho/2;
        else
                *ancho=tablaFNT[WhatChar].ancho+1;
        fclose(fichFnt);
return;
}
void GetCharSizeBuffer(int WhatChar,int *ancho,int *alto,char *buffer)
{
        *ancho=4;
        *alto=1;

        memcpy(tablaFNT,buffer+8+768+sizeof(reglas)+4,sizeof(tablaFNT));
        *alto=tablaFNT[WhatChar].incY+tablaFNT[WhatChar].alto;
        *ancho=tablaFNT[WhatChar].ancho+1;
return;
}
int ShowChar(int WhatChar,int cx,int cy,char *ptr,int an)
{
int y,iy;
FILE *fichFnt;
char *rawBuffer;
        if ((fichFnt=fopen("PREVIEW.FNT","rb"))==NULL)
                return 4;
        fseek(fichFnt,8+768+sizeof(reglas)+4,SEEK_SET);
        if (fread(tablaFNT,sizeof(tablaFNT),1,fichFnt)<1)
        {
                fclose(fichFnt);
                return 4;
        }
        if (tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto)
        {
            rawBuffer=(char *) malloc(tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto);
            if (rawBuffer==NULL)
            {
                fclose(fichFnt);
                if(WhatChar==32)
                        return (tablaFNT[WhatChar].ancho/2);
                else
                        return (tablaFNT[WhatChar].ancho+1);
            }
            if (fseek(fichFnt,tablaFNT[WhatChar].offset,SEEK_SET))
            {
                free(rawBuffer);
                fclose(fichFnt);
                if(WhatChar==32)
                        return (tablaFNT[WhatChar].ancho/2);
                else
                        return (tablaFNT[WhatChar].ancho+1);
            }
            if (fread(rawBuffer,tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto,1,fichFnt)<1)
            {
                free(rawBuffer);
                fclose(fichFnt);
                if(WhatChar==32)
                        return (tablaFNT[WhatChar].ancho/2);
                else
                        return (tablaFNT[WhatChar].ancho+1);

            }
            iy=tablaFNT[WhatChar].incY;
            for (y=0; y<tablaFNT[WhatChar].alto; y++)
                memcpy(ptr+((cy+iy)*an+cx)+y*an,rawBuffer+y*tablaFNT[WhatChar].ancho,tablaFNT[WhatChar].ancho);

            fclose(fichFnt);
            free(rawBuffer);
            if(WhatChar==32)
                return (tablaFNT[WhatChar].ancho/2);
            else
                return (tablaFNT[WhatChar].ancho+1);
        } else fclose(fichFnt);
if(WhatChar==32)
        return (tablaFNT[WhatChar].ancho/2);
else
        return (tablaFNT[WhatChar].ancho+1);

}

int ShowCharBuffer(int WhatChar,int cx,int cy,char *ptr,int an,char *buffer)
{
int y,iy,x,c;
char *rawBuffer;
        memcpy(tablaFNT,buffer+8+768+sizeof(reglas)+4,sizeof(tablaFNT));
        if (tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto)
        {
            rawBuffer=(char *) malloc(tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto);
            if (rawBuffer==NULL)
            {
                if(WhatChar==32)
                        return (tablaFNT[WhatChar].ancho/2);
                else
                        return (tablaFNT[WhatChar].ancho+1);
            }
            memcpy(rawBuffer,buffer+tablaFNT[WhatChar].offset,tablaFNT[WhatChar].ancho*tablaFNT[WhatChar].alto);
            iy=tablaFNT[WhatChar].incY;
            for (y=0; y<tablaFNT[WhatChar].alto; y++) {
                for (x=0; x<tablaFNT[WhatChar].ancho; x++) {
                  if ((c=rawBuffer[y*tablaFNT[WhatChar].ancho+x])) {
                    ptr[(cy+iy)*an+cx+x+y*an]=c;
                  }
                }
//                memcpy(ptr+((cy+iy)*an+cx)+y*an,rawBuffer+y*tablaFNT[WhatChar].ancho,tablaFNT[WhatChar].ancho);
            }
            free(rawBuffer);
            if(WhatChar==32)
                return (tablaFNT[WhatChar].ancho/2);
            else
                return (tablaFNT[WhatChar].ancho+1);
        }
return (tablaFNT[WhatChar].ancho+1);
}

void ConvertFntToPal(char *buffer)
{
char DacFnt[768];
int acum=0,x,a,b;
byte xlat[256];
        memcpy(DacFnt,buffer+8,768);

        for(x=0;x<768;x++) if(dac[x]!=DacFnt[x]) acum=1;
        if(!acum)
                return;

        create_dac4();
        xlat[0]=0;
        for (x=1;x<256;x++)
                xlat[x]=find_color_not0(DacFnt[x*3],DacFnt[x*3+1],DacFnt[x*3+2]);

        memcpy(tablaFNT,buffer+8+768+sizeof(reglas)+4,sizeof(tablaFNT));

        for(x=0;x<256;x++)
        {
                if (tablaFNT[x].ancho*tablaFNT[x].alto)
                        for(a=0;a<tablaFNT[x].alto;a++)
                                for(b=0;b<tablaFNT[x].ancho;b++)
                                {
                                        acum=*(buffer+tablaFNT[x].offset+a*tablaFNT[x].ancho+b);
                                        *(buffer+tablaFNT[x].offset+a*tablaFNT[x].ancho+b)=xlat[acum];
                                }
        }
        memcpy(buffer+8,dac,768);
}
