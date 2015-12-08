#!/bin/bash

rm -rf obj obj_d objd1

make -f Makefile.html dirs R1EMBED="--preload-file $1@" D1HTML_EXE="$2" DIV1RUN=html/$3.js html/$3.js

