#!/bin/bash
#mipsel-linux-gcc $1 -o $2 -fPIC -funsigned-char `sdl-config --cflags` -shared -w -I/home/mike/div2015/dll/
gcc $1 -o $2 -fPIC -funsigned-char -shared -I/home/mike/div2015/dll/ -lchipmunk -ggdb3 -O0
