#!/bin/bash
gcc $1 -o $2 -fPIC -funsigned-char `sdl-config --cflags` -shared -w
