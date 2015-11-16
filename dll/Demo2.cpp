#include <math.h>
#include <stdio.h>

#define GLOBALS
#include "div.h"

// Important: For each function you must indicate the retval(int), and always
// make a getparm() for each parameter of the call (the retval(), is always
// needed, even if the function doesn't need to return a value).

void addsqrt() { // Silly function, that returns the squared root of an addition
  int a=getparm();
  int b=getparm();
  int c=sqrt(a+b);

  retval(c);
}

void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("ADDSQRT",addsqrt,2); // The name of the function within the language
                                   // is indicated, as well as it's pointer and the
                                   // number of parameters it has.
}

void __export divmain(COMMON_PARAMS) {
  GLOBAL_IMPORT();
}

