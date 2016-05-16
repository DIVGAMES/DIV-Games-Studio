# Sample toolchain file for building for Raspberry pi from an Ubuntu Linux system.
#
# mkdir ~/raspidev
# cd ~/raspidev
# git clone https://github.com/raspberrypi/tools.git
# export ARCH=arm
# export CROSS_COMPILE=arm-bcm2708hardfp-linux-gnueabi-
# export CC=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-gcc
# export NM=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-nm
# export LD=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ld
# export CXX=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-g++
# export RANLIB=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ranlib
# export AR=~/raspidev/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-ar
#
# wget http://www.libsdl.org/release/SDL-1.2.15.tar.gz
# tar xf SDL-1.2.15.tar.gz
# 

## build SDL1.2
# cd SDL-1.2.15
# ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=/home/mike/raspidev/SDL_cross
# make
# make install
#

## build zlib
# cd ..
# wget zlib.net/zlib-1.2.8.tar.gz
# tar xf zlib-1.2.8.tar.gz
# cd zlib-1.2.8
# ./configure --prefix=/home/mike/raspidev/SDL_cross
# make
# make install
#
#

## build mikmod
# cd ..
# wget http://netix.dl.sourceforge.net/project/mikmod/libmikmod/3.3.8/libmikmod-3.3.8.tar.gz
# tar xf libmikmod-3.3.8.tar.gz
# cd libmikmod-3.3.8
# ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=/home/mike/raspidev/SDL_cross
# make
# make install

## build SDL_mixer
# cd ..
# wget https://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-1.2.12.tar.gz
# tar xf SDL_mixer-1.2.12.tar.gz
# cd SDL_mixer-1.2.12
# export PATH=~/raspidev/SDL_cross/bin:$PATH
# ./configure --disable-pulseaudio --target=arm-linux --host=arm-linux --prefix=/home/mike/raspidev/SDL_cross
#
### edit Makefile, change the EXTRA_LDFLAGS LINE to tell sdl_mixer where to find libSDL
### < EXTRA_LDFLAGS =  -lSDL -lm
### ---
### > EXTRA_LDFLAGS =  -L$(prefix)/lib -lSDL -lm
#
# make
# make install
#
#
## You should now be able to cross compile / build for pi!

SET(CROSS_DIR "/home/mike/raspidev/")

set(CMAKE_SYSTEM_NAME PI)
SET(PLATFORM "PI")

# set per-build values
SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 1)
SET(HAS_DLL 1)
SET(HAS_JPEG 0)
SET(HAS_DLL 1)
SEt(HAS_FLI 1)

SET(OS_DEFINITIONS " -D_GNU_SOURCE=1 -D_REENTRANT ")
set(TOOLCHAIN_PREFIX ${CROSS_DIR}tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH ~/raspidev/ )
set(CMAKE_SYSTEM_LIBRARY_PATH ${CROSS_DIR}tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/arm-bcm2708-linux-gnueabi/sysroot/usr/ )
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
include_directories(${CROSS_DIR}SDL_cross/include/)

set ( CMAKE_EXE_LINKER_FLAGS "-L${CROSS_DIR}SDL_cross/lib -Wl,-rpath,/home/mike/raspidev/SDL_cross/lib -lSDL -lpthread -ldl")
