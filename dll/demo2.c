#include <math.h>
#include <stdio.h>

#define GLOBALS
#include "div.h"

// Importante: Para cada funci¢n se debe indicar el retval(int), y hacer
// siempre un getparm() por cada par metro de llamada (el retval() es
// imprescindible incluso si la funci¢n no necesita devolver un valor).

void addsqrt() { // Funci¢n tonta, que devuelve la raiz cuadrada de una suma
  int a = getparm();
  int b = getparm();
  int c = sqrt(a + b);

  retval(c);
}

void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("ADDSQRT", addsqrt,
             2); // Se indica el nombre que tendr  la funci¢n
                 // en el lenguaje, el puntero a la misma y
                 // el n£mero de par metros que tiene.
}

void __export divmain(COMMON_PARAMS) { GLOBAL_IMPORT(); }
