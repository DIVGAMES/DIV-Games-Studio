#include <stdio.h>

#define GLOBALS
#include "div.h"

// The possible breaking points can be seen on DIV.H (search for Entry-Points)

void post_process_buffer(void) {

  // Example of a breaking point. DIV calls this function to operate over the
  // video buffer, before each dump.

}

void __export divmain(COMMON_PARAMS)
{
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("post_process_buffer",post_process_buffer);
}

void __export divend(COMMON_PARAMS){}

