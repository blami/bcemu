########################################################################
# bc_emu: video game emulator                                          #
# Copyright © 2008-2010, Ondrej Balaz <ondra@blami.net>                #
#                                                                      #
# This software is distributed under MIT license. See LICENSE.         #
########################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(bc_emu C)

########################################################################
#                               MODULES                                #
########################################################################
INCLUDE (TestBigEndian)

########################################################################
#                            PROJECT FILES                             #
########################################################################
# source files:
SET(SRC
	bc_emu.c
	xmalloc.c
)
# library files:
SET(LIBS
)
SET(TARGET_LIBS
)
# compiler flags:
SET(FLAGS
)

# include directories:
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
#                        CONFIGURATION OPTIONS                         #
########################################################################
### AUTO TESTS ###
# target arch endianness
TEST_BIG_ENDIAN(BIG_ENDIAN)
IF(NOT ${BIG_ENDIAN})
	SET(LSB "1")
ELSE(NOT ${BIG_ENDIAN})
	SET(MSB "1")
ENDIF(NOT ${BIG_ENDIAN})

# version
FILE(READ "${CMAKE_SOURCE_DIR}/.gitversion" VERSION)
SET(VERSION             "git-${VERSION}")

# build environment
SET(BUILD_OS            "${CMAKE_SYSTEM}")
SET(BUILD_CPU           "${CMAKE_SYSTEM_PROCESSOR}")


### GLOBAL OPTIONS ###
# build with debug code
OPTION(DEBUG            "Debug build"                   OFF)
IF(DEBUG)
	# gcc
	IF(CMAKE_COMPILER_IS_GNUCC)
		ADD_DEFINITIONS(-g)
	ENDIF(CMAKE_COMPILER_IS_GNUCC)
ELSE(DEBUG)
	SET(NDEBUG "1")
ENDIF(DEBUG)

# target architecture
SET(ARCH    "pc_cli"    CACHE STRING "Target architecture")
IF(ARCH     STREQUAL "pc_cli")
	# PC/CLI
	SET(ARCH_PC_CLI ON)
	SET(SRC ${SRC}
		"arch/pc/pc_cli.c"
		"arch/pc/getopt.c"
		"arch/pc/getopt_long.c"
	)
ELSEIF(ARCH STREQUAL "nds")
	# Nintendo DS (tm)
	MESSAGE(FATAL_ERROR "Architecture not supported yet.")
ELSE(ARCH   STREQUAL "pc_cli")
	# Unknown architecture (error)
	MESSAGE(FATAL_ERROR "Architecture `${ARCH}' does not exist. See INSTALL.txt")
ENDIF(ARCH  STREQUAL "pc_cli")


### EMULATOR OPTIONS ###
# null
OPTION(EMU_NULL         "Enable null emulator (debug)"          OFF)

# NEC PCEngine
OPTION(EMU_PCE          "Enable NEC PCEngine emulator"          ON)


### DEBUGGER TOOL OPTIONS ###
# null
OPTION(DBG_NULL         "Enable null debugger (debug)"          OFF)


### USER INTERFACE OPTIONS ###
# null
OPTION(UI_NULL          "Enable null user interface (debug)"    OFF)
IF(UI_NULL)
ENDIF(UI_NULL)

# SDL
OPTION(UI_SDL           "Enable SDL user interface"             ON)
IF(UI_SDL AND (ARCH STREQUAL "pc_cli"))
	FIND_PACKAGE(SDL REQUIRED)
	ADD_LIBRARY(ui_sdl STATIC
		ui/sdl/sdl.c
	)
	SET(LIBS ${LIBS} ${SDL_LIBRARY} ui_sdl)
ELSEIF(UI_SDL AND NOT (ARCH STREQUAL "pc_cli"))
	MESSAGE(WARNING "SDL not supported on this architecture. Disabled.")
ENDIF(UI_SDL AND (ARCH STREQUAL "pc_cli"))


### CONFIGURATION OUTPUT ###
# compile-time configuration (bc_emu.h):
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/bc_emu.h.cmake
	${CMAKE_BINARY_DIR}/bc_emu.h
	@ONLY
)

########################################################################
#                             EXECUTABLE                               #
########################################################################
# Final executable:
LINK_LIBRARIES(${LIBS})

ADD_EXECUTABLE(bc_emu ${SRC})
SET_TARGET_PROPERTIES(bc_emu PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
