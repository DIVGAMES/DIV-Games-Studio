#!/bin/bash

echo "Setting up emscripten"

source ~/emsdk_portable/emsdk_env.sh > /dev/null

rm -rf obj obj_d objd1

echo "Building $4 $3.html"

make -f Makefile.html dirs R1EMBED="--preload-file $1@" D1HTML_EXE="$2" DIV1RUN=html/$3.js html/$3.js > /dev/null

rm -rf obj obj_d objd1

echo "Done!"
