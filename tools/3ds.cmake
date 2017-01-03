# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
SET(PLATFORM "N3DS")
SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 1)
SET(HAS_DLL 0)
SET(HAS_MODE8 1)

set(TOOLCHAIN_PREFIX arm-none-eabi)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

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
set(CFLAGS "${CFLAGS} -mword-relocations -fno-short-enums -fomit-frame-pointer -ffast-math" )
set(CFLAGS "${CFLAGS} -DARM11 -D_3DS")

#TARGET_LINK_LIBRARIES ("mingw32 SDLmain  SDL SDL_Mixer")
#-mwindows /usr/i686-w64-mingw32/lib/x64/SDL_mixer.lib")
#set ( CMAKE_EXE_LINKER_FLAGS "-lmingw32 -lSDLmain  -lSDL -mwindows")
set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}lib -L${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/lib -L$ENV{DEVKITPRO}/libctru/lib -Wl,-rpath,$ENV{DEVKITPRO}/libctru/lib -specs=3dsx.specs ${CFLAGS}")
#set (CMAKE_EXE_LINKER_FLAGS "${CFLAGS}")

ADD_DEFINITIONS(  -I${CMAKE_FIND_ROOT_PATH}/include -DN3DS -I${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/ ${CFLAGS})

SET(SDL_INCLUDE_DIR ${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/)
SET(SDL_LIBRARY SDL ctru) 

SET(SDL_MIXER_INCLUDE_DIR ${CMAKE_FIND_ROOT_PATH}/portlibs/armv6k/include/)
SET(SDL_MIXER_LIBRARY SDL_mixer mad vorbis ogg vorbisfile m)
#-DMIXER) 

#SET(OSDEP "src/shared/osdep/n3ds.c" )

