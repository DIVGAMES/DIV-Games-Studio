#include <stdio.h>

#define GLOBALS
#include "div.h"

// The possible breakpoints are listed in DIV.H (search for Entry-Points)

void post_process_buffer(void) {

  // Example of a breakpoint, DIV will call this function so that
  // it can operate on the video buffer before each dump.

}

void __export divmain(COMMON_PARAMS)
{
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer",post_process_buffer);
}

void __export divend(COMMON_PARAMS){}

