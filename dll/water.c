
#include <math.h>
#include <stdio.h>
//#include <mem.h>

#define GLOBALS
#include "div.h"

#define HEIGHT 40

char *water = NULL;

void post_process_buffer(void) {
  static int desp = 3000;
  static int dec = -1;
  static int valor = 0;
  int x, inc;
  FILE *f;

  if (valor == 0) {
    valor = 1;
    water = (char *)div_malloc(HEIGHT * wide);
  }

  if (water == NULL)
    return;

  inc = (height - HEIGHT - 1) * 100;

  for (x = 0; x < HEIGHT; x++) {
    memcpy(water + x * wide, buffer + (inc / 100) * wide, wide);
    inc -= desp / 10;
    desp += dec;
    if (desp < 2500)
      dec = 1;
    if (desp > 3500)
      dec = -1;
  }

  memset(water, 0, wide);
  memcpy(buffer + (height - HEIGHT) * wide, water, wide * HEIGHT);
}

void __export divlibrary(LIBRARY_PARAMS) {}

void __export divmain(COMMON_PARAMS) {
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer", post_process_buffer);
}

void __export divend(COMMON_PARAMS) {
  printf("free'ing mem %x\n", water);
  //	if(water)
  //		div_free(water);

  water = NULL;
}
