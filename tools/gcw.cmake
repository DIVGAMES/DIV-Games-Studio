# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..
LIST(APPEND CMAKE_PROGRAM_PATH  "/opt/gcw0-toolchain/usr/bin" )


set(CMAKE_SYSTEM_NAME Linux)
SET(GCW=1)
SET(PLATFORM "GCW")

SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 1)
SET(HAS_DLL 0)
SET(HAS_MODE8 1)



set(TOOLCHAIN_PREFIX mipsel-linux)
#set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH /opt/${TOOLCHAIN_PREFIX})

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#TARGET_LINK_LIBRARIES ("mingw32 SDLmain  SDL SDL_Mixer")
#-mwindows /usr/i686-w64-mingw32/lib/x64/SDL_mixer.lib")
#set ( CMAKE_EXE_LINKER_FLAGS "-lmingw32 -lSDLmain  -lSDL -mwindows")

ADD_DEFINITIONS( -DGCW -DGCW_SOFTSTRETCH )
#-DMIXER) 

#SET(OSDEP "src/osdep/osdep.c" )

