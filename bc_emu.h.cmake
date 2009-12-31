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
 *
 * Also don't forget add your module definition into bc_emu_modules.c
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

/* architecture */
#cmakedefine ARCH       "@ARCH@"
#cmakedefine ARCH_PC
#cmakedefine ARCH_NDS

/* modules configuration */
/* emulator */
#cmakedefine EMU        "@EMU@"
#cmakedefine EMU_NULL   /* null emulator (debug) */
#cmakedefine EMU_PCE    /* NEC PCEngine emulator */

/* ui */
#cmakedefine UI         "@UI@"
#cmakedefine UI_NULL    /* null user interface (debug) */
#cmakedefine UI_SDL     /* SDL user interface */
#cmakedefine UI_SDLGL   /* SDL OpenGL user interface */

/* -------------------------------------------------------------------------- *
 * Shared headers                                                             *
 * -------------------------------------------------------------------------- */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <assert.h>             /* assert() */

/* common helpers */
#include "debug.h"
#include "xmalloc.h"
#include "xtypes.h"

/* common */
#include "module.h"             /* module types */
#include "interface.h"          /* emulator/UI interface */

/* architecture */
#ifdef ARCH_PC                  /* PC */
#include "arch/pc/pc.h"
#endif /* ARCH_PC */

/* module-specific headers */
/* emulator */
#ifdef EMU_PCE                  /* NEC PCEngine emulator */
#include "emu/pce/pce.h"
#endif /* EMU_PCE */

/* user interface */
#ifdef UI_SDL                   /* SDL user interface */
#include "ui/sdl/sdl.h"
#endif /* UI_SDL */

#ifdef UI_SDLGL                 /* SDL OpenGL user interface */
#include "ui/sdlgl/sdlgl.h"
#endif /* UI_SDLGL */

/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * Architecture independent type representing ROM image.
 */
typedef struct
{
	int size;               /**< ROM image size in bytes */
	uint8* data;            /**< ROM image data */

} t_rom;


/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_rom*   emu_rom;
extern char*    emu_progname;

/* bc_emu_modules.c */
extern t_emu    emu_modules_emu[];
extern t_ui     emu_modules_ui[];

/* FIXME remove these */
extern t_video* emu_video;
extern t_audio* emu_audio;
extern t_input* emu_input;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      emu_main(char*, char*);
extern void     emu_exit();

#endif /* __BC_EMU_H */
