# Sample toolchain file for building for Windows from an Ubuntu Linux system.
#
# Typical usage:
#    *) install cross compiler: `sudo apt-get install mingw-w64 g++-mingw-w64`
#    *) cd build
#    *) cmake -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-Ubuntu-mingw64.cmake ..

set(CMAKE_SYSTEM_NAME Windows)

SET(PLATFORM "WINDOWS")

# set per-build values

# SDL 0, 1 or 2

SET(HAS_SDL 1)
SET(HAS_SDLTTF 0)
SET(HAS_SDLIMAGE 0)
SET(HAS_ZLIB 1)
SET(HAS_SDLMIXER 1)
SET(HAS_DLL 0)
SET(HAS_JPEG 0)
SET(HAS_MODE8 1)
SET(HAS_NEWMODE8 0)
SET(HAS_GIT 0)

IF(CPUARCH STREQUAL "64")
	MESSAGE(STATUS "Buiding Windows 64bit")
	SET(OS_DEFINITIONS " -mwindows -Dmain=SDL_main ")
#	SET(OS_LIBS mingw32 SDLmain gdi32)
	set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
ELSE() # Default 32 bits
	MESSAGE(STATUS "Buiding Windows 32bit")
	SET(OS_DEFINITIONS " -m32 -mwindows -Dmain=SDL_main ")
#	SET(OS_LIBS mingw32 SDLmain m dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid)
#gdi32  ole32 oleaut32 imm32 winmm )
	set(TOOLCHAIN_PREFIX i686-w64-mingw32)
ENDIF()

IF(HAS_SDL EQUAL 2)
    SET(OS_LIBS mingw32 SDL2main dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid)
    SET(OS_LINK_FLAGS " -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lFLAC -lvorbisfile -lvorbis -logg -lpthread") 
ELSE()
    SET(OS_LIBS mingw32 SDLmain )
ENDIF()


# cross compilers to use for C and C++
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# target environment on the build host system
#   set 1st to dir with the cross compiler's C/C++ headers/libs
set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})


#SET(CMAKE_INCLUDE_PATH /usr/${TOOLCHAIN_PREFIX}/include ) 
#SET(CMAKE_SYSTEM_LIBRARY_PATH /usr/${TOOLCHAIN_PREFIX}/lib ) 
#SET(CMAKE_SYSTEM_INCLUDE_PATH /usr/${TOOLCHAIN_PREFIX}/include ) 
#SET(SDLDIR /usr/${TOOLCHAIN_PREFIX}/include/SDL )

# modify default behavior of FIND_XXX() commands to
# search for headers/libs in the target environment and
# search for programs in the build host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#SET(OS_LINK_FLAGS " -static-libgcc -static-libstdc++ -mwindows ")


SET(EXT ".exe")

#TARGET_LINK_LIBRARIES ("mingw32 SDLmain  SDL SDL_Mixer")
#-mwindows /usr/i686-w64-mingw32/lib/x64/SDL_mixer.lib")
#set ( CMAKE_EXE_LINKER_FLAGS "-lmingw32 -lSDLmain  -lSDL -static-libgcc -static-libstdc++ -mwindows " )

#ADD_DEFINITIONS( -mwindows -Dmain=SDL_main -DZLIB -DWIN32 -DMIXER)
# -DDIVDLL) 

SET(OSDEP "src/win/osdepwin.c" )
