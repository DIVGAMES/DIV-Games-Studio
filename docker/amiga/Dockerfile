FROM amigadev/crosstools:m68k-amigaos
RUN apt-get update && apt install -y vim-nox ack
WORKDIR /tmp
RUN wget http://aminet.net/dev/lib/SDL_Mixer-Amiga-1.2.12.lha
RUN lha xw=mixer SDL_r-Amiga-1.2.12.lha
RUN cp -r mixer/include mixer/lib /opt/m68k-amigaos/

RUN wget http://aminet.net/dev/gg/zlib-bin.lha
RUN lha xw=zlib_bin zlib-bin.lha
RUN cp -r zlib_bin/include zlib_bin/lib /opt/m68k-amigaos
WORKDIR /src
