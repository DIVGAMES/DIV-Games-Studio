#include <math.h>
#include <stdio.h>
#include <mem.h>

#define GLOBALS
#include "div.h"

char pal[1024];
char palette[256];
char sort[256];

int r,g,b;

char find_ord(char *);

void sort_palette(void);
void flip(void);

void ss_init(void)
{
  int n;
  sort_palette();
  for (n=0;n<256;n++)
    sort[palette[n]]=n;
}

void ss_frame(void)
{
  int m,n,y,counter=0;
  char c1,c2;

  for (m=0;m<32;m++) {
    for (n=0;n<wide;n++) {
      y=div_rand(0,height-2);
      c1=(int)*(buffer+n+y*wide);
      c2=(int)*(buffer+n+(y+1)*wide);
      if (sort[c1]>sort[c2]) {
        *(buffer+n+(y+1)*wide)=c1;
        *(buffer+n+y*wide)=c2;
        counter++;
      } else {
        if (div_rand(0,1)) {
          if (n>0) {
            c2=(int)*(buffer+n+(y+1)*wide-1);
            if (sort[c1]>sort[c2]) {
              *(buffer+n+(y+1)*wide-1)=c1;
              *(buffer+n+y*wide)=c2;
            }
          }
        } else {
          if (n<wide-1) {
            c2=(int)*(buffer+n+(y+1)*wide+1);
            if (sort[c1]>sort[c2]) {
              *(buffer+n+(y+1)*wide+1)=c1;
              *(buffer+n+y*wide)=c2;
            }
          }
        }
      }
    }
  }
  if (counter<wide/2) flip();
}

void sort_palette(void) {
  char c;
  int n;

  n=0; do {
    pal[n*4+3]=(pal[n*4]=palette[n*3])+(pal[n*4+1]=palette[n*3+1])+(pal[n*4+2]=palette[n*3+2]);
  } while (++n<256);

  r=0; g=0; b=0;

  n=0; do {
    c=find_ord(pal);
    r=pal[c*4]; g=pal[c*4+1]; b=pal[c*4+2];
    pal[c*4]=255; palette[n]=c;
  } while (++n<256);

}

char find_ord(char * dac) {

  int dmin,dif,r2,g2,b2;
  char *pal,*endpal,*color;

  pal=dac; endpal=dac+1024; dmin=65536;
  if (r<0) r=0; else if (r>63) r=63;
  if (g<0) g=0; else if (g>63) g=63;
  if (b<0) b=0; else if (b>63) b=63;
  do {
    if (*pal!=255) {
      dif=(r-(int)*pal)*(r-(int)*pal); pal++;
      dif+=(g-(int)*pal)*(g-(int)*pal); pal++;
      dif+=(b-(int)*pal)*(b-(int)*pal); pal+=2;
      if (dif<dmin) { dmin=dif; color=pal-4; }
    } else pal+=4;
  } while (pal<endpal);

  return((color-dac)/4);
}

void flip(void) {
  int n;
  for (n=0;n<=height/2;n++) {
    memcpy(pal,buffer+n*wide,wide);
    memcpy(buffer+n*wide,buffer+(height-n-1)*wide,wide);
    memcpy(buffer+(height-n-1)*wide,pal,wide);
  }
}

void __export divmain(COMMON_PARAMS)
{
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("ss_init",ss_init);
  DIV_export("ss_frame",ss_frame);
}

