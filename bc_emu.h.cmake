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
#cmakedefine NDEBUG     /* stdlib (assert) */

/* host architecture byte-order */
#cmakedefine LSB
#cmakedefine MSB

/* host architecture information */
#cmakedefine BUILD_OS   "@BUILD_OS@"
#cmakedefine BUILD_CPU  "@BUILD_CPU@"
#cmakedefine VERSION    "@VERSION@"
#cmakedefine CODENAME   "@CODENAME@"
#cmakedefine WIN32

/* modules loading */
#cmakedefine STATIC     /* modules are compiled-in */
#cmakedefine DYNAMIC    /* modules are dl'ed (NOT IMPLEMENTED YET!) */

/* modules configuration */
/* architecture */
#cmakedefine ARCH       "@ARCH@"
#cmakedefine ARCH_PC
#cmakedefine ARCH_NDS

/* emulator */
#cmakedefine EMU        "@EMU@"
#cmakedefine EMU_NULL   /* null emulator (debug) */
#cmakedefine EMU_PCE    /* NEC PCEngine emulator */

/* debugger */
#cmakedefine DBG        "@DBG@"
#cmakedefine DBG_NULL   /* null debugger (debug) */

/* ui */
#cmakedefine UI         "@UI@"
#cmakedefine UI_NULL    /* null user interface (debug) */
#cmakedefine UI_SDL     /* SDL user interface */

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

/* common */
#include "interface.h"

/* build configuration headers */
/* architecture */
#ifdef ARCH_PC                  /* PC */
#include "arch/pc/pc.h"
#endif /* ARCH_PC */

#ifdef ARCH_NDS                 /* Nintendo DS (tm) homebrew */
#include "arch/nds/nds-arm7.h"
#include "arch/nds/nds-arm9.h"
#endif /* ARCH_NDS */

/* emulator */
#ifdef EMU_NULL                 /* NULL emulator (debug) */
#include "emu/null/null.h"
#endif /* EMU_NULL */

#ifdef EMU_PCE                  /* NEC PCEngine emulator */
#include "emu/pce/pce.h"
#endif /* EMU_PCE */

/* debugger */
#ifdef DBG_NULL                 /* NULL debugger (debug) */
#include "dbg/dbg_null/null.h"
#endif /* DBG_NULL */

/* user interface */
#ifdef UI_NULL                  /* NULL user interface (debug) */
#include "emu/null/null.h"
#endif /* UI_NULL */

#ifdef UI_SDL                   /* SDL user interface */
#include "ui/sdl/sdl.h"
#endif /* UI_SDL */

/* -------------------------------------------------------------------------- *
 * Plugin registry                                                            *
 * -------------------------------------------------------------------------- */



/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern uint8* bc_emu_rom;       /* pointer to ROM image */


/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int bc_emu_main(char*, char*, char*);
extern void bc_emu_exit();

#endif /* __BC_EMU_H */
