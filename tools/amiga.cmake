# Sample toolchain file for building for Amiga from any system with docker installed
# 
# From the docker/amiga directory:
# 
# docker build -t m68k-amigaos .
# docker run -v ${PWD}../../..:/src -it m68k-amigaos bash /src//tools/amiga.sh


set(CMAKE_SYSTEM_NAME Amiga)
SET(AMIGA=1)
SET(PLATFORM "AMIGA")
set(TOOLCHAIN_PREFIX m68k-amigaos)

SET(HAS_SDL 1)
SET(HAS_RAYLIB 0)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 1)
SET(HAS_DLL 0)
SET(HAS_JPEG 0)
SET(HAS_FLI 1)
SET(HAS_MODE8 0)
SET(HAS_GIT 0)
SET(HAS_NET 0)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# set the target CPU to 68020
set(M68K_CPU "68020" CACHE STRING "Target CPU model")
set(M68K_CRT "ixemul" CACHE STRING "Target std lib")
# find AmigaOS
#find_package(AmigaOS REQUIRED)

# add the source file
# add_executable(SDLProgram main2.c)

# set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m${M68K_CPU} -fomit-frame-pointer")

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

ADD_DEFINITIONS( -DAMIGAOS -I/opt/m68k-amigaos/include)
#-I${CMAKE_FIND_ROOT_PATH}/include)
#-DGCW_SOFTSTRETCH)	
#-DMIXER) 

set ( CMAKE_EXE_LINKER_FLAGS "-L/opt/m68k-amigaos/lib -Wl,-rpath,/opt/m68k-amigaos/lib")
#  -lSDL -lpthread -lSDL_mixer_nomad")
set(EXTRA_LIBS ${EXTRA_LIBS} SDL_mixer_nomad pthread )

#FILE(GLOB OSX_SOURCES "src/osx/*.c")
# SET(OSDEP ${OSDEP} ${OSX_SOURCES} "src/shared/osdep.c" "src/shared/unzip.c")
# INCLUDE_DIRECTORIES("src/osx")
SET(OSDEP "src/shared/osdep.c"  "src/shared/osdep/amiga.c" )
#  "src/osx/fmemopen.c")

