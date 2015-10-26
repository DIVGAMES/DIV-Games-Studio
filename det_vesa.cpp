/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/
#include "global.h"
//#include "inc\vesa.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Detecciขn de los modos vesa
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
/*
struct _modos {
  short ancho;
  short alto;
  short modo;
};
struct _modos modos[32];
int num_modos;
int VersionVesa;
char marcavga[128];
*/
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int compare_mode(void const *aa, void const *bb) {
  short *a,*b;
  a=(short*)aa;
  b=(short*)bb;
  return((int)(*a)*10000+*(a+1)>(int)(*b)*10000+*(b+1));
}

void detectar_vesa(void) { // Detecta los modos de vกdeo disponibles
#ifdef NOTYET
	short *modelist;
  int n;

  num_modos=6;
  modos[0].ancho=320; modos[0].alto=200; modos[0].modo=0;
  modos[1].ancho=320; modos[1].alto=240; modos[1].modo=0;
  modos[2].ancho=320; modos[2].alto=400; modos[2].modo=0;
  modos[3].ancho=360; modos[3].alto=240; modos[3].modo=0;
  modos[4].ancho=360; modos[4].alto=360; modos[4].modo=0;
  modos[5].ancho=376; modos[5].alto=282; modos[5].modo=0;
  modos[5].ancho=376; modos[5].alto=282; modos[5].modo=0;

	if (vbeInit() != 0) {
    VersionVesa=0;
    strcpy(marcavga,"VESA driver not found.");
    return;
	}

  VersionVesa=VbeInfoBlock.VbeVersion; //VV.VVh
  sprintf(marcavga,"VESA %d.%d %s %dK",VersionVesa>>8,VersionVesa&0xFF,VbeInfoBlock.OemStringPtr,VbeInfoBlock.TotalMemory*64);

	modelist = (short *) (VbeInfoBlock.VideoModePtr);
	while (*modelist != -1) {
		if (vbeGetModeInfo(*modelist) == 0) {
      if (ModeInfoBlock.BitsPerPixel==8 && num_modos<32) {
        for (n=0;n<num_modos;n++) {
          if (modos[n].ancho==ModeInfoBlock.XResolution && modos[n].alto==ModeInfoBlock.YResolution)
            break;
        }
        if (n==num_modos) {
          modos[num_modos].ancho=ModeInfoBlock.XResolution;
          modos[num_modos].alto=ModeInfoBlock.YResolution;
          modos[num_modos].modo=*modelist;
          num_modos++;
        } else {
          if (modos[n].ancho!=320 || modos[n].alto!=200)
            modos[n].modo=*modelist;
        }
      }
		}
		modelist++;
	}

  qsort((void*)&(modos[0].ancho),num_modos,sizeof(struct _modos),compare_mode);
#endif
}
