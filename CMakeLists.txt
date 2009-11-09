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
# compiler flags:
SET(FLAGS
)

# include directories:
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

########################################################################
#                        CONFIGURATION OPTIONS                         #
########################################################################
# auto tests:
# target arch endianness
TEST_BIG_ENDIAN(BIG_ENDIAN)
IF(NOT ${BIG_ENDIAN})
	SET(LSB "1")
ELSE(NOT ${BIG_ENDIAN})
	SET(MSB "1")
ENDIF(NOT ${BIG_ENDIAN})

# options:
# build with debug code
OPTION(DEBUG            "Debug build"                       OFF)
IF(${DEBUG})
	ADD_DEFINITIONS(-g) # TODO: make compiler/os specific
ELSE(${DEBUG})
	SET(NDEBUG "1")
ENDIF(${DEBUG})

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
#LINK_LIBRARIES(${LIBS})

ADD_EXECUTABLE(bc_emu ${SRC})
SET_TARGET_PROPERTIES(bc_emu PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

########################################################################
#                              LIBRARIES                               #
########################################################################

