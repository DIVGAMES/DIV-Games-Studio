/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

#include "inter.h"
#ifdef DOS
//#include "..\inc\vesa.h"
#endif

//═════════════════════════════════════════════════════════════════════════════
// Detección de los modos vesa
//═════════════════════════════════════════════════════════════════════════════
int compare_mode(void const *aa, void const *bb) {
	int *a, *b;

	a = (int *)aa;
	b = (int *)bb;

	return(*a * 10000 + *(a + 1) > *b * 10000 + *(b + 1));
}

// Detecta los modos de vídeo disponibles
void detectar_vesa(void) {
	short *modelist;
	int n;

	num_video_modes = 6;

	video_modes[0].ancho = 320;
	video_modes[0].alto  = 200;
	video_modes[0].modo  = 320200;

	video_modes[1].ancho = 320;
	video_modes[1].alto  = 240;
	video_modes[1].modo  = 320240;

	video_modes[2].ancho = 320;
	video_modes[2].alto  = 400;
	video_modes[2].modo  = 320400;

	video_modes[3].ancho = 360;
	video_modes[3].alto  = 240;
	video_modes[3].modo  = 360240;

	video_modes[4].ancho = 360;
	video_modes[4].alto  = 360;
	video_modes[4].modo  = 360360;

	video_modes[5].ancho = 376;
	video_modes[5].alto  = 282;
	video_modes[5].modo  = 376282;

#ifdef NOTYET
	if(vbeInit() != 0) {
		VersionVesa = 0;

		return;
	}

	VersionVesa = VbeInfoBlock.VbeVersion; //VV.VVh

	modelist = (short *) (VbeInfoBlock.VideoModePtr);
	while(*modelist != -1) {
		if(vbeGetModeInfo(*modelist) == 0) {
			if(ModeInfoBlock.BitsPerPixel == 8 && num_video_modes < 32) {
				for(n = 0; n < num_video_modes; n++) {
					if(video_modes[n].ancho == ModeInfoBlock.XResolution &&
					   video_modes[n].alto  == ModeInfoBlock.YResolution) {
						break;
					}
				}
				if(n == num_video_modes) {
					video_modes[num_video_modes].ancho = ModeInfoBlock.XResolution;
					video_modes[num_video_modes].alto  = ModeInfoBlock.YResolution;
					video_modes[num_video_modes].modo  = *modelist;

					num_video_modes++;
				}
				else {
					if(video_modes[n].ancho != 320 ||
					   video_modes[n].alto  != 200) {
						video_modes[n].modo = *modelist;
					}
				}
			}
		}

		modelist++;
	}
#endif

	qsort((void *)&(video_modes[0].ancho),
	      num_video_modes,
	      sizeof(t_video_modes),
	      compare_mode);
}
