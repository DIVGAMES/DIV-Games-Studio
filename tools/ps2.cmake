# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
SET(PS2=1)
SET(PLATFORM "PS2")
set(TOOLCHAIN_PREFIX ee)
#set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH /usr/local/ps2dev/)
set(PS2SDK ${CMAKE_FIND_ROOT_PATH}/ps2sdk)
SET(PS2DEV ${CMAKE_FIND_ROOT_PATH} )
# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#TARGET_LINK_LIBRARIES ("gcc kernel c syscall")
#-mwindows /usr/i686-w64-mingw32/lib/x64/SDL_mixer.lib")
#set ( CMAKE_EXE_LINKER_FLAGS "-lmingw32 -lSDLmain  -lSDL -mwindows")
set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/gsKit/lib/ -L${PS2SDK}/ee/lib/  -Wl,-rpath,${PS2SDK}/ee/lib  -mno-crt0 -T${PS2SDK}/ee/startup/linkfile ${PS2SDK}/ee/startup/crt0.o -Wl,--whole-archive ${PS2SDK}/ee/lib/libc.a -Wl,--no-whole-archive ${PS2DEV}/ee/ee/lib/libc.a -lstdc++ -lc -lkernel" )
#-L${CMAKE_FIND_ROOT_PATH}ps2sdk/ports/lib -L${CMAKE_FIND_ROOT_PATH}ps2sdk/ee/lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}ps2sdk/ee/lib  -lc -lkernel" )
ADD_DEFINITIONS( -D_EE  -I${PS2SDK}/ee/include -I${PS2SDK}/common/include -I${PS2SDK}/ports/include -DPS2)
#-I${CMAKE_FIND_ROOT_PATH}ee/ee/sys-include/ -I${CMAKE_FIND_ROOT_PATH}ee/ee/include/ -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/ee/include  -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/common/include -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/ports/include -I${CMAKE_FIND_ROOT_PATH}/ee/ee/sys-include/sys/  -I${CMAKE_FIND_ROOT_PATH}/usr/include -DPS2)
#-DMIXER) 

SET(OSDEP "src/shared/run/v.c" )

