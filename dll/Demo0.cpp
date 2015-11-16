
#include <stdio.h>

#define GLOBALS
#include "div.h"

void ss_init(void) { // Function to start the screen saver.

  ss_time=1500; // Calls the screen saver after 15 seconds of inactivity.

}

void ss_frame(void) { // Puts "wide" points of black, randomly
  int x,y,n=0;

  for (n=0;n<wide;n++) {
    x=div_rand(0,wide-1);
    y=div_rand(0,height-1);
    *(buffer+x+y*wide)=0;
  }
}

void __export divmain(COMMON_PARAMS) {
  AutoLoad();
  GLOBAL_IMPORT();
  DIV_export("ss_init",ss_init);
  DIV_export("ss_frame",ss_frame);
}
