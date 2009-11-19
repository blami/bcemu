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
SET(CODENAME "cinnamon")

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
	${CMAKE_BINARY_DIR}/bc_emu_modules.c
	module.c
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

# on x86_64 enable -m32
IF(UNIX AND (BUILD_CPU STREQUAL "x86_64"))
OPTION(BUILD_32BIT      "Build 32bit binary (-m32)"     OFF)
IF(BUILD_32BIT)
	MESSAGE(WARNING "You have selected 32bit binary output on x86_64 host.
	Remember that you will need 32bit versions of devel packages and operating
	system kernel support.")
	SET(LINK_FLAGS "-m32")
ENDIF(BUILD_32BIT)
ENDIF(UNIX AND (BUILD_CPU STREQUAL "x86_64"))

# target architecture
SET(ARCH    "pc"    CACHE STRING "Target architecture")
IF(ARCH     STREQUAL "pc")
	# PC/CLI
	SET(ARCH_PC ON)
	SET(SRC ${SRC}
		"arch/pc/pc.c"
		"arch/pc/xgetopt.c"
	)
ELSEIF(ARCH STREQUAL "nds")
	# Nintendo DS (tm)
	MESSAGE(FATAL_ERROR "Architecture not supported yet.")
ELSE(ARCH   STREQUAL "pc")
	# Unknown architecture (error)
	MESSAGE(FATAL_ERROR "Architecture `${ARCH}' does not exist. See INSTALL.")
ENDIF(ARCH  STREQUAL "pc")


### EMULATOR OPTIONS ###
SET(EMU)
# NEC PCEngine
OPTION(EMU_PCE          "Enable NEC PCEngine emulator"          ON)
IF(EMU_PCE)
	SET(EMU "pce ${EMU}")
	ADD_LIBRARY(emu_pce STATIC
		emu/pce/pce.c
		emu/pce/cpu_huc6280.c
	)
	SET(LIBS ${LIBS} ${SDL_LIBRARY} emu_pce)
ENDIF(EMU_PCE)


### USER INTERFACE OPTIONS ###
SET(UI)
# SDL (only pc support)
OPTION(UI_SDL           "Enable SDL user interface"             ON)
IF(UI_SDL AND (ARCH STREQUAL "pc"))
	SET(UI "sdl ${UI}")
	FIND_PACKAGE(SDL REQUIRED)
	ADD_LIBRARY(ui_sdl STATIC
		ui/sdl/sdl.c
	)
	SET(LIBS ${LIBS} ${SDL_LIBRARY} ui_sdl)
ELSEIF(UI_SDL AND NOT (ARCH STREQUAL "pc"))
	MESSAGE(WARNING "SDL not supported on this architecture. Disabled.")
ENDIF(UI_SDL AND (ARCH STREQUAL "pc"))


### CONFIGURATION OUTPUT ###
# compile-time configuration (bc_emu.h):
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/bc_emu.h.cmake
	${CMAKE_BINARY_DIR}/bc_emu.h
	@ONLY
)
# modules configuration (bc_emu_modules.c)
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/bc_emu_modules.c.cmake
	${CMAKE_BINARY_DIR}/bc_emu_modules.c
	@ONLY
)

########################################################################
#                             EXECUTABLE                               #
########################################################################
# Final executable:
LINK_LIBRARIES(${LIBS})

ADD_EXECUTABLE(bc_emu ${SRC})
SET_TARGET_PROPERTIES(bc_emu PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
