/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/
#include "inter.h"
//#include "..\inc\vesa.h"

//═════════════════════════════════════════════════════════════════════════════
//  Detection of vesa modes
//═════════════════════════════════════════════════════════════════════════════

int compare_mode(void const *aa, void const *bb) {
  int *a,*b;
  a=(int*)aa;
  b=(int*)bb;
  return( *a*10000 + *(a+1) > *b*10000 + *(b+1) );
}

void detect_vesa(void) { // Detect the available video modes
	short *modelist;
  int n;

  num_video_modes=6;
  video_modes[0].width=320; video_modes[0].height=200; video_modes[0].mode=320200;
  video_modes[1].width=320; video_modes[1].height=240; video_modes[1].mode=320240;
  video_modes[2].width=320; video_modes[2].height=400; video_modes[2].mode=320400;
  video_modes[3].width=360; video_modes[3].height=240; video_modes[3].mode=360240;
  video_modes[4].width=360; video_modes[4].height=360; video_modes[4].mode=360360;
  video_modes[5].width=376; video_modes[5].height=282; video_modes[5].mode=376282;

#ifdef NOTYET
	if (vbeInit() != 0) {
    VersionVesa=0;
    return;
	}

  VersionVesa=VbeInfoBlock.VbeVersion; //VV.VVh

	modelist = (short *) (VbeInfoBlock.VideoModePtr);
	while (*modelist != -1) {
		if (vbeGetModeInfo(*modelist) == 0) {
      if (ModeInfoBlock.BitsPerPixel==8 && num_video_modes<32) {
        for (n=0;n<num_video_modes;n++) {
          if (video_modes[n].width==ModeInfoBlock.XResolution && video_modes[n].height==ModeInfoBlock.YResolution)
            break;
        }
        if (n==num_video_modes) {
          video_modes[num_video_modes].width=ModeInfoBlock.XResolution;
          video_modes[num_video_modes].height=ModeInfoBlock.YResolution;
          video_modes[num_video_modes].mode=*modelist;
          num_video_modes++;
        } else {
          if (video_modes[n].width!=320 || video_modes[n].height!=200)
            video_modes[n].mode=*modelist;
        }
      }
		}
		modelist++;
	}
#endif

  qsort((void*)&(video_modes[0].width),num_video_modes,sizeof(struct _video_modes),compare_mode);
}
