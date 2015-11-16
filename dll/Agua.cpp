
#include <math.h>
#include <stdio.h>
//#include mem.h>

#define GLOBALS
#include "c:\div\dll\source\div.h"

#define ALTURA 20

char    *agua=NULL;

void post_process_buffer(void) {
  static int desp=3000;
  static int dec=-1;
  static int valor=0;
  int    x,inc;
//  FILE   *f;

  if(valor==0) {
    valor=1;
    agua=(char *)div_malloc(ALTURA*wide);
  }

  if(agua==NULL) return;

  inc=(height-ALTURA-1)*100;

  for(x=0;x<ALTURA;x++) {
//    memcpy(agua+x*wide,buffer+(inc/100)*wide,wide);
    inc-=desp/10;
    desp+=dec;
    if(desp<2500) dec=1;
    if(desp>3500) dec=-1;
  }

//  memset(agua,0,wide);
//  memcpy(buffer+(height-ALTURA)*wide,agua,wide*ALTURA);
}

//void __declspec (divlibrary)

/*__declspec divmain(COMMON_PARAMS) {
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer",post_process_buffer);
}

void __export divend(COMMON_PARAMS) {
  div_free(agua);
}*/

