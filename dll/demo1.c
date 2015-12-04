#include <stdio.h>

#define GLOBALS
#include "div.h"

// Los posibles puntos de ruptura se ven en DIV.H (Buscar Entry-Points)

void post_process_buffer(void) {

  // Ejemplo de un punto de ruptura, DIV llamar  a esta funci¢n para que
  // pueda operar sobre el buffer de v¡deo antes de cada volcado.

}

void __export divmain(COMMON_PARAMS)
{
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer",post_process_buffer);
}

void __export divend(COMMON_PARAMS){}

