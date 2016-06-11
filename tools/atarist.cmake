# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..

SET(CMAKE_SYSTEM_NAME Linux)
SET(ATARIST=1)
SET(PLATFORM "ATARIST")

SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 0)
SET(HAS_DLL 0)
SET(HAS_JPEG 0)
SET(HAS_MODE8 0)
SET(HAS_NEWMODE8 0)
SET(HAS_GIT 0)

set(TOOLCHAIN_PREFIX m68k-atari-mint)
#set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs

set(CMAKE_FIND_ROOT_PATH /usr/m68k-atari-mint/)
#set(PSPSDK ${CMAKE_FIND_ROOT_PATH}/psp/sdk)
#SET(PSPDEV ${CMAKE_FIND_ROOT_PATH}/psp )

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(OS_LIBS "SDL" "gem")

#SET(OS_LIBS "z" "m"  "SDL" "GL" "pspvfpu" "psphprm" "pspaudio" "c" "g" "pspgu" "psprtc") 
#SET(OS_LIBS ${OS_LIBS} "psplibc" "pspnet" "pspnet_inet" "pspnet_apctl" "pspnet_resolver" "psputility" "pspuser" "pspkernel")
#SET(OS_LIBS ${OS_LIBS} "pspdebug" "pspdisplay" "pspge" "pspctrl" "pspsdk") 

#set (OS_LINK_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib  -L${CMAKE_FIND_ROOT_PATH}/psp/lib ")
#SET( OS_DEFINITIONS "-I${PSPSDK}/include/" )


#SET(OSDEP "src/shared/osdep/psp.c" )

