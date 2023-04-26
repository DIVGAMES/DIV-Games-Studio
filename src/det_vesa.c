#include "global.h"

//═════════════════════════════════════════════════════════════════════════════
//  Detection of available video modes
//═════════════════════════════════════════════════════════════════════════════
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

//═════════════════════════════════════════════════════════════════════════════

int compare_mode(void const *aa, void const *bb) {
	struct _modos m1 = *(struct _modos *)aa;
	struct _modos m2 = *(struct _modos *)bb;
	return (m1.alto * m1.ancho - m2.alto * m2.ancho);
}

void detectar_vesa(void) { // Detects available video modes
	short *modelist;
	int n;

	OSDEP_VMode **modes;
	int i;

	num_modos=0;


	modes=OSDEP_ListModes();

	/* Check is there are any modes available */
	if(modes == 0) {
		fprintf(stdout, "No Modes available. Setting to defaults\n");
		// none, just list some standard ones for use in windowed mode.
		modos[0].ancho=320; modos[0].alto=240; modos[0].modo=1;
		modos[1].ancho=640; modos[1].alto=480; modos[1].modo=1;
		modos[2].ancho=800; modos[2].alto=600; modos[2].modo=1;
		modos[3].ancho=1024; modos[3].alto=768; modos[3].modo=1;
		modos[4].ancho=1280; modos[4].alto=1024; modos[4].modo=1;
		modos[5].ancho=1920; modos[5].alto=1080; modos[5].modo=1;
		modos[6].ancho=1280; modos[6].alto=720; modos[6].modo=1;
		modos[7].ancho=376; modos[7].alto=282; modos[7].modo=1;
		// vga_an = 640;
		// vga_al = 480;
		// big = 1;
		// big2 = 2;
		num_modos = 8;
	} else {
		for(i=0;modes[i];++i) {
			modos[i].ancho=modes[i]->w; modos[i].alto=modes[i]->h; modos[i].modo=1;
		}
		num_modos=i-1;
		free(modes);

	}
	
	sprintf(marcavga,"SDL Video Driver");

	fprintf(stdout, "Detected %d video modes:\n", num_modos);
	qsort((void*)&(modos[0].ancho),num_modos,sizeof(struct _modos),compare_mode);
}
