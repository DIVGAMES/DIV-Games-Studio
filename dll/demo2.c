#include <math.h>
#include <stdio.h>

#define GLOBALS
#include "div.h"

// Important: For each function you must specify the retval(int), and
// always do a getparm() for each call parameter (the retval() is
// mandatory even if the function does not need to return a value).

void addsqrt() { // Dummy function that returns the square root of a sum
  int a=getparm();
  int b=getparm();
  int c=sqrt(a+b);

  retval(c);
}

void __export divlibrary(LIBRARY_PARAMS) {

  COM_export("ADDSQRT",addsqrt,2); // Specify the name the function will have
                                   // in the language, a pointer to it and
                                   // the number of parameters it takes.
}

void __export divmain(COMMON_PARAMS) {
  GLOBAL_IMPORT();
}

