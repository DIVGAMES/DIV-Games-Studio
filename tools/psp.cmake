# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
SET(PSP=1)
SET(PLATFORM "PSP")
set(TOOLCHAIN_PREFIX psp)
#set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs

set(CMAKE_FIND_ROOT_PATH /home/mike/pspdev)
set(PSPSDK ${CMAKE_FIND_ROOT_PATH}/psp/sdk)
SET(PSPDEV ${CMAKE_FIND_ROOT_PATH}/psp )

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(OS_LIBS "z" "m"  "SDL" "GL" "pspvfpu" "psphprm" "pspaudio" "c" "g" "pspgu" "psprtc") 
SET(OS_LIBS ${OS_LIBS} "psplibc" "pspnet" "pspnet_inet" "pspnet_apctl" "pspnet_resolver" "psputility" "pspuser" "pspkernel")
SET(OS_LIBS ${OS_LIBS} "pspdebug" "pspdisplay" "pspge" "pspctrl" "pspsdk") 

set (OS_LINK_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib  -L${CMAKE_FIND_ROOT_PATH}/psp/lib ")
SET( OS_DEFINITIONS "-I${PSPSDK}/include/" )


SET(OSDEP "src/shared/osdep/psp.c" )

