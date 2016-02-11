#!/bin/bash
#mipsel-linux-gcc $1 -o $2 -fPIC -funsigned-char `sdl-config --cflags` -shared -w -I/home/mike/div2015/dll/
#gcc $1 -o $2 -fPIC -funsigned-char -shared -I/home/mike/div2015/dll/ -lchipmunk -ggdb3 -O0
#gcc $1 -o $2 -fPIC -funsigned-char -shared -I/home/mike/div2015/dll/ -lchipmunk -O3
gcc $1 -o $2 -funsigned-char -shared -I/home/mike/div2015/dll/ dll/spectrum/*.o `sdl-config --cflags --libs` dll/sdlgfx/SDL_rotozoom.o -ggdb3 -O3 -fPIC 
