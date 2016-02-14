# OSX cmake include

SET (PLATFORM "OSX")
SET (TARGETOS "OSX")
FILE(GLOB OSX_SOURCES "src/osx/*.c")
SET(OSDEP ${OSDEP} ${OSX_SOURCES} "src/shared/osdep.c" "src/shared/unzip.c")
INCLUDE_DIRECTORIES("src/osx")
