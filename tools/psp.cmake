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

#TARGET_LINK_LIBRARIES ("gcc kernel c syscall")
#-mwindows /usr/i686-w64-mingw32/lib/x64/SDL_mixer.lib")
#set ( CMAKE_EXE_LINKER_FLAGS "-lmingw32 -lSDLmain  -lSDL -mwindows")


set (CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib  -L${CMAKE_FIND_ROOT_PATH}/psp/lib ")
#set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-specs=${PSPSDK}/lib/prxspecs -Wl,-q,-T${PSPSDK}/lib/linkfile.prx")
#-lpspuser -lpsplibc -lpspuser -lpspkernel -lGL -lg -lc ")

#set (CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib -L${CMAKE_FIND_ROOT_PATH}/psp/lib -Wl,-rpath,${PSPSDK}/psp/sdk/-lc -lpspgu -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lc -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser -lpspkernel")

#set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib -L -Wl,-rpath,${PSPSDK}/psp/sdk/lib -lc -lpspgu -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lc -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser -lpspkernel ")

#-lSDL -lm -lc -lglut -lGL -liberty -lSDLmain -lpspgu -lpspkernel -lpspuser -lpspctrl -lpspctrl_driver -lpspdebugkb -lpspctrl_driver -lSDL -lglut -lGL -lstdc++ -lpsp-sim")

#set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/psp/sdk/lib -lSDL -lSDL_ttf -lSDL_image -lSDLmain -ljpeg -lpng -lz -lm -lSDL -lstdc++ -lgcc -liberty -lc -lg -lpspaudio -lpsplibc -lSDL -lSDL_ttf -lSDL_image -lpspwlan -lpsppower -lGL -l freetype -ljpeg -lpng -lz -lm -lSDL -lpspgu -l psphprm -lpspaudio -lstdc++ -lpspvfpu -lpsprtc -lpspuser -lpspkernel")

#set ( CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_FIND_ROOT_PATH}/gsKit/lib/ -L${PS2SDK}/ee/lib/  -Wl,-rpath,${PS2SDK}/ee/lib  -mno-crt0 -T${PS2SDK}/ee/startup/linkfile ${PS2SDK}/ee/startup/crt0.o -Wl,--whole-archive ${PS2SDK}/ee/lib/libc.a -Wl,--no-whole-archive ${PS2DEV}/ee/ee/lib/libc.a -lstdc++ -lc -lkernel" )

#-L${CMAKE_FIND_ROOT_PATH}ps2sdk/ports/lib -L${CMAKE_FIND_ROOT_PATH}ps2sdk/ee/lib/ -Wl,-rpath,${CMAKE_FIND_ROOT_PATH}ps2sdk/ee/lib  -lc -lkernel" )
ADD_DEFINITIONS( -I${PSPSDK}/include/ -DPSP)
#-I${CMAKE_FIND_ROOT_PATH}ee/ee/sys-include/ -I${CMAKE_FIND_ROOT_PATH}ee/ee/include/ -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/ee/include  -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/common/include -I${CMAKE_FIND_ROOT_PATH}/ps2sdk/ports/include -I${CMAKE_FIND_ROOT_PATH}/ee/ee/sys-include/sys/  -I${CMAKE_FIND_ROOT_PATH}/usr/include -DPS2)
#-DMIXER) 

SET(OSDEP "src/shared/osdep/psp.c" )

