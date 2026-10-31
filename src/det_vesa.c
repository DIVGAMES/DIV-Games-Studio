#include "global.h"

//═════════════════════════════════════════════════════════════════════════════
//  Detection of available video modes
//═════════════════════════════════════════════════════════════════════════════
/*
struct _modes {
  short width;
  short height;
  short mode;
};
struct _modes video_modes[32];
int num_modes;
int VersionVesa;
char vgabrand[128];
*/

//═════════════════════════════════════════════════════════════════════════════

int compare_mode(void const *aa, void const *bb) {
	short *a,*b;
	a=(short*)aa;
	b=(short*)bb;
	return((int)(*a)*10000+*(a+1)>(int)(*b)*10000+*(b+1));
}

void detect_vesa(void) { // Detects available video modes
	short *modelist;
	int n;

	OSDEP_VMode **modes;
	int i;

	num_modes=0;


	modes=OSDEP_ListModes();

	/* Check is there are any modes available */
	if(modes == 0 || modes == -1) {
		// none, just list some standard ones for use in windowed mode.
		video_modes[0].width=320; video_modes[0].height=240; video_modes[0].mode=1;
		video_modes[1].width=640; video_modes[1].height=480; video_modes[1].mode=1;
		video_modes[2].width=800; video_modes[2].height=600; video_modes[2].mode=1;
		video_modes[3].width=1024; video_modes[3].height=768; video_modes[3].mode=1;
		video_modes[4].width=1280; video_modes[4].height=1024; video_modes[4].mode=1;
		video_modes[5].width=1920; video_modes[5].height=1080; video_modes[5].mode=1;
		video_modes[6].width=1280; video_modes[6].height=720; video_modes[6].mode=1;
		video_modes[7].width=376; video_modes[7].height=282; video_modes[7].mode=1;
	} else {
		for(i=0;modes[i];++i) {
			video_modes[i].width=modes[i]->w; video_modes[i].height=modes[i]->h; video_modes[i].mode=1;
		}
		num_modes=i-1;

	}
	
	sprintf(vgabrand,"SDL Video Driver");

	qsort((void*)&(video_modes[0].width),num_modes,sizeof(struct _modes),compare_mode);
}
