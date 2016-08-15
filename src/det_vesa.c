#include "global.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Detection of available video modes
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

void detectar_vesa(void) { // Detects available video modes
	short *modelist;
	int n;

	OSDEP_VMode **modes;
	int i;

	num_modos=0;


	modes=OSDEP_ListModes();

	/* Check is there are any modes available */
	if(1) {//modes == 0 || modes == (OSDEP_VMode **)-1) {
		// none, just list some standard ones for use in windowed mode.
		modos[0].ancho=320; modos[0].alto=240; modos[0].modo=1;
		modos[1].ancho=640; modos[1].alto=480; modos[1].modo=1;
		modos[2].ancho=800; modos[2].alto=600; modos[2].modo=1;
		modos[3].ancho=1024; modos[3].alto=768; modos[3].modo=1;
		modos[4].ancho=1280; modos[4].alto=1024; modos[4].modo=1;
		modos[5].ancho=1920; modos[5].alto=1080; modos[5].modo=1;
		modos[6].ancho=1280; modos[6].alto=720; modos[6].modo=1;
		modos[7].ancho=376; modos[7].alto=282; modos[7].modo=1;
	} else {
		for(i=0;modes[i];++i) {
			modos[i].ancho=modes[i]->w; modos[i].alto=modes[i]->h; modos[i].modo=1;
		}
		num_modos=i-1;

	}
	
	sprintf(marcavga,"SDL Video Driver");

	qsort((void*)&(modos[0].ancho),num_modos,sizeof(struct _modos),compare_mode);
}
