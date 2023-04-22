FROM amigadev/crosstools:m68k-amigaos
RUN apt-get update && apt install -y vim-nox ack
WORKDIR /tmp
RUN wget http://aminet.net/dev/lib/SDL_Mixer-Amiga-1_2_12.lha
RUN lha x SDL_Mixer-Amiga-1_2_12.lha
RUN cp -r mixer/include mixer/lib /opt/m68k-amigaos/

# RUN wget http://aminet.net/dev/gg/zlib-bin.lha
# RUN lha xw=zlib_bin zlib-bin.lha
# RUN cp -r zlib_bin/include zlib_bin/lib /opt/m68k-amigaos

# RUN wget http://aminet.net/dev/lib/zlib_68k.lha
# RUN lha x zlib_68k.lha
# RUN cp -r zlib_68k/nofpu/include zlib_68k/nofpu/lib /opt/m68k-amigaos


RUN wget http://aminet.net/dev/lib/zlib-source.lha
RUN lha xw=zlib zlib-source.lha
WORKDIR /tmp/zlib/src
RUN make
RUN cp libz.a /opt/m68k-amigaos/lib
RUN cp zconf.h zlib.h /opt/m68k-amigaos/include/

WORKDIR /src
