
#include <stdio.h>

#define GLOBALS
#include "div.h"

void ss_init(void) { // Funci¢n de inicializaci¢n del salvapantallas.
}

void ss_frame(void) { // Pone "wide" puntos en negro, aleatoriamente
  int x, y, n = 0;

  for (n = 0; n < wide; n++) {
    x = div_rand(0, wide - 1);
    y = div_rand(0, height - 1);
    *(buffer + x + y * wide) = 0;
  }
}

void __export divmain(COMMON_PARAMS) {
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("ss_init", ss_init);
  DIV_export("ss_frame", ss_frame);

  ss_time = 1500; // Llamar al salvapantallas tras 15 segundos de inactividad.
}

void __export divlibrary(LIBRARY_PARAMS) {}
void __export divend(COMMON_PARAMS) {}
