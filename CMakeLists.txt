# CMAKE lists file for FUZE BASIC V2.0+
# (c) 2015 Mike Green and the FUZE Teame

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(div2015)

#INCLUDE ( tools/git.cmake )
#INCLUDE( tools/welcome.cmake )
#INCLUDE( tools/defaults.cmake )
# The name of our binary
IF(NOT DEFINED TARGET)
	SET(TARGET "div") 
ENDIF()

#IF(NOT EXISTS build.conf)
#	MESSAGE("-- NO BUILD.CONF FOUND!")
#	MESSAGE("-- Creating a default one")
#	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy tools/build.conf.default build.conf)
#	IF(EXISTS build.conf)
#		MESSAGE("-- done")
#	ENDIF()
#ENDIF()

#INCLUDE( build.conf )

# Check platform
# Currently detects osx and pi
#IF(NOT DEFINED PLATFORM OR PLATFORM STREQUAL "AUTO")
#	INCLUDE (tools/platform.cmake)
#ENDIF()

IF(NOT DEFINED PLATFORM OR PLATFORM STREQUAL "AUTO")
#	MESSAGE("WHOOPS! NO PLATFORM DETECTED!")
	SET(PLATFORM "LINUX")
ENDIF()

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL REQUIRED sdl)
PKG_SEARCH_MODULE(SDLIMAGE REQUIRED SDL_image>=1.2.0)
PKG_SEARCH_MODULE(SDLMIXER REQUIRED SDL_mixer>=1.2.0)

include_directories(${SDL_INCLUDE_DIRS})
include_directories(${SDLIMAGE_INCLUDE_DIRS})
include_directories(${SDLMIXER_INCLUDE_DIRS})

set(EXTRA_LIBS ${EXTRA_LIBS} ${SDL_LIBRARIES} ${SDLIMAGE_LIBRARIES} ${SDLMIXER_LIBRARIES})


# include our SDL stuff
# this includes TTF
#IF(SDL GREATER 0)
#	MESSAGE("-- Including SDL(${SDL})")
#	SET(TARGET ${TARGET}-SDL${SDL})
#	INCLUDE (tools/sdl.cmake)
#ENDIF()


# generate keywordTokens.h from
# new script that doesn't rely
# on csh
#INCLUDE ( tools/dxtokens.cmake )

# default definitions
ADD_DEFINITIONS( -D_GNU_SOURCE )

#INCLUDE_DIRECTORIES(visor )
FILE(GLOB DIV_SOURCES "*.c")
FILE(GLOB VISOR_SOURCES "visor/*.c")

#INCLUDE (tools/plugins.cmake)

ADD_DEFINITIONS( -D${PLATFORM} )

SET(TARGET ${TARGET}-${PLATFORM} ) #-${GIT_BRANCH}-${GIT_COMMIT_HASH})

# make this a c++ app
#IF(CPLUSPLUS) 
SET_SOURCE_FILES_PROPERTIES(${DIV_SOURCES} ${VISOR_SOURCES} ${SDLGFX_SOURCES} ${PLAT_SOURCES} PROPERTIES LANGUAGE CXX )
ADD_DEFINITIONS( -fpermissive  -w -DFUZEBUILD="${TARGET}") 
SET (TARGET ${TARGET}-CXX)
#ENDIF()

ADD_EXECUTABLE(
	${TARGET} 
	global.h
	${VISOR_SOURCES}
	${DIV_SOURCES} 
)
 
TARGET_LINK_LIBRARIES(${TARGET} ${EXTRA_LIBS} ${SDLGFX_LIBS} -lm -lz)

#ADD_CUSTOM_COMMAND(
#	TARGET ${TARGET}
#	COMMENT "-- Linking Executable ${TARGET}"
#	PRE_LINK
#)

ADD_CUSTOM_COMMAND(
	TARGET ${TARGET}
	DEPENDS ${TARGET}
	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET} div
	COMMENT "-- COPYING ${TARGET} to div"
	POST_BUILD
)
#IF(DEBUG EQUAL 0) 
#ADD_CUSTOM_COMMAND(
#	TARGET ${TARGET}
#	DEPENDS ${TARGET}
#	COMMAND ${CMAKE_COMMAND} -E copy ${TARGET} fuzebasic-3/fuzebasic-3.0.0/fuze
#	COMMENT "-- Some package prep.."
#	POST_BUILD
#)
#ENDIF()

#IF(GTK EQUAL 3) 
#ENDIF()
