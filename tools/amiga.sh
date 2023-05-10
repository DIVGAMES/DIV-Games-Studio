#!/usr/bin/bash
mkdir -p amiga
cd amiga
cmake .. -DUSE_SDL=1 -DTARGETOS=AMIGA
make d-AMIGA divdbg-AMIGA div-AMIGA