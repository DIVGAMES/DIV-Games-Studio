FROM i386/ubuntu:latest
# Sample toolchain file for building for Raspberry pi from an Ubuntu Linux system.

RUN mkdir /raspidev
WORKDIR /raspidev
RUN apt-get update
RUN apt-get install -y git
RUN git clone --depth 1 https://github.com/raspberrypi/tools.git
ENV ARCH=arm
ENV TOOLCHAIN_PATH=/raspidev
ENV CROSS_TOOLCHAIN=arm-bcm2708hardfp-linux-gnueabi-
ENV CC=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-gcc
ENV NM=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-nm
ENV LD=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ld
ENV CXX=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-g++
ENV RANLIB=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ranlib
ENV AR=${TOOLCHAIN_PATH}/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ar

RUN apt-get install -y wget
RUN wget http://www.libsdl.org/release/SDL-1.2.15.tar.gz
RUN tar xf SDL-1.2.15.tar.gz


RUN apt-get install -y build-essential
# build SDL1.2

RUN apt-get install -y automake
WORKDIR SDL-1.2.15
RUN cp /usr/share/automake-1.15/config.guess build-scripts/
RUN ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=${TOOLCHAIN_PATH}/SDL_cross
RUN make
RUN make install

# build zlib
WORKDIR /raspidev
RUN wget zlib.net/zlib-1.2.13.tar.gz
RUN tar xf zlib-1.2.13.tar.gz
WORKDIR zlib-1.2.13
RUN ./configure --prefix=${TOOLCHAIN_PATH}/SDL_cross
RUN make
RUN make install



# # build mikmod
WORKDIR /raspidev
RUN wget https://downloads.sourceforge.net/project/mikmod/outdated_versions/libmikmod/3.3.11/libmikmod-3.3.11.tar.gz
RUN tar xf libmikmod-3.3.11.tar.gz
WORKDIR libmikmod-3.3.11
RUN ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=${TOOLCHAIN_PATH}/SDL_cross
RUN make
RUN make install

# # build SDL_mixer
WORKDIR /raspidev
RUN wget https://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-1.2.12.tar.gz
RUN tar xf SDL_mixer-1.2.12.tar.gz
WORKDIR SDL_mixer-1.2.12
ENV PATH=${TOOLCHAIN_PATH}/SDL_cross/bin:$PATH
RUN cp /usr/share/automake-1.15/config.guess build-scripts/

RUN ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=${TOOLCHAIN_PATH}/SDL_cross
RUN make
RUN make install

RUN apt-get install -y cmake
RUN apt-get install -y pkg-config

RUN ln -s /usr/bin/make /usr/bin/gmake


# CLEANUP


# ## edit Makefile, change the EXTRA_LDFLAGS LINE to tell sdl_mixer where to find libSDL
# ## < EXTRA_LDFLAGS =  -lSDL -lm
# ## ---
# ## > EXTRA_LDFLAGS =  -L$(prefix)/lib -lSDL -lm

# make
# make install


# You should now be able to cross compile / build for pi!
