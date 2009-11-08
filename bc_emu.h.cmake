/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* bc_emu.h: Compile-time configuration settings. */

#ifndef __BC_EMU_H
#define __BC_EMU_H


/*
 * WARNING:
 * don't modify this file directly (bc_emu.h). This file is generated from
 * `bc_emu.h.cmake' by CMake build system.
 */

/* -------------------------------------------------------------------------- *
 * Compile-time configuration                                                 *
 * -------------------------------------------------------------------------- */

/* debug code */
#cmakedefine DEBUG
#cmakedefine NDEBUG     /* disable asserts, etc. */

/* host architecture endianness */
#cmakedefine LSB        /* little endian */
#cmakedefine MSB        /* big endian */


/* -------------------------------------------------------------------------- *
 * Shared headers                                                             *
 * -------------------------------------------------------------------------- */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/* common helpers */
#include "debug.h"
#include "xmalloc.h"
#include "xtypes.h"


#endif /* __BC_PCE_H */
