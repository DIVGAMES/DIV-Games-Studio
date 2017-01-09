# Nintendo 3DS Cmake ruleset
#
# Requires devkitpro and additional portlibs
# 
# Uses SDL and SDL_mixer from https://github.com/nop90/SDL-3DS 
#
# Uses portlibs: https://github.com/devkitPro/3ds_portlibs.git
#
# git clone https://github.com/nop90/SDL-3DS
# cd SDL-3DS/SDL
# # Remove -fno-short-enums from Makefile
# make
# make install
# cd -
# # portlibs must be installed before SDL_mixer can be built
# git clone https://github.com/devkitPro/3ds_portlibs.git
# cd 3ds_portlibs
# make zlib
# make install-zlib
# make libogg
# make libmad
# make install
# cd ..
#
# Grab precompiled vorbis libs from 3ds-vgmstream git repo
# git clone https://github.com/TricksterGuy/3ds-vgmstream.git
# cp -R 3ds-vgmstream/libs/vorbis/lib/*.a $DEVKITPRO/portlibs/armv6k/lib
# cp -R 3ds-vgmstream/libs/vorbis/include/vorbis/ $DEVKITPRO/portlibs/armv6k/include/
# cd SDL-3DS/SDL_mixer
# # Remove -fno-short-enums from Makefile
# make
# make install





# SDL_mixer requires vorbislib

set(CMAKE_SYSTEM_NAME Linux)
SET(PLATFORM "N3DS")
SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 0)
SET(HAS_DLL 0)
SET(HAS_MODE8 1)

IF(NOT DEFINED ENV{DEVKITARM})
MESSAGE( FATAL_ERROR "Please set DEVKITARM in your environment.\nexport DEVKITARM=<path to>devkitARM" )
ENDIF()

set(TOOLCHAIN_PREFIX arm-none-eabi)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}-ld)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH $ENV{DEVKITPRO})

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CFLAGS "-march=armv6k -mtune=mpcore -mfloat-abi=hard")
#set(CFLAGS "${CFLAGS} -mword-relocations -fno-short-enums -fomit-frame-pointer -ffast-math -fno-pic" )
set(CFLAGS "${CFLAGS} -DARM11 -D_3DS")


set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}lib -L${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/lib -L$ENV{DEVKITPRO}/libctru/lib -Wl,-rpath,$ENV{DEVKITPRO}/libctru/lib -specs=3dsx.specs ${CFLAGS} -Wl,-Map,divrun-N3DS.map")
#set (CMAKE_EXE_LINKER_FLAGS "${CFLAGS}")

ADD_DEFINITIONS(  -I${CMAKE_FIND_ROOT_PATH}/include -DN3DS -I${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/ -I/$ENV{DEVKITPRO}/libctru/include/)

SET (OS_DEFINITIONS ${CFLAGS})

SET(SDL_INCLUDE_DIR ${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/)
SET(SDL_LIBRARY sf2d citro3d sfil ctru png turbojpeg) 

SET(SDL_MIXER_INCLUDE_DIR ${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/)
SET(SDL_MIXER_LIBRARY SDL_mixer mad vorbis ogg vorbisfile m)


#SET(OSDEP GLOB ("src/shared/osdep/3ds/*.c") )
FILE(GLOB OSDEP "src/shared/osdep/3ds/*.c")
SET(OSDEP ${OSDEP} "src/shared/osdep.c" )

