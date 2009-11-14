/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* bc_emu_modules.c: Generated list of modules. */

/*
 * WARNING:
 * don't modify this file directly (bc_emu_modules.c). This file is generated
 * from `bc_emu_modules.c.cmake' by CMake build system.
 *
 * To compile modules using static libraries you will also need to add header
 * files to bc_emu.h.cmake.
 */

#include "bc_emu.h"


/* -------------------------------------------------------------------------- *
 * Module registry                                                            *
 * -------------------------------------------------------------------------- */

/**
 * List of available emulator modules.
 */
t_emu bc_emu_modules_emu[] =
{
#ifdef EMU_NULL
	/* null emulator */
	{"null",
		emu_null_init,
		emu_null_shutdown
	},
#endif /* EMU_NULL */
#ifdef EMU_PCE
	/* NEC PC Engine emulator */
	{"pce",
		emu_pce_init,
		emu_pce_shutdown
	},
#endif /* EMU_PCE */
	{NULL, NULL, NULL}
};

/**
 * List of available UI modules.
t_ui bc_emu_modules_ui[] =
{
#ifdef UI_SDL
	/* SDL UI */
	{"sdl", 
		ui_sdl_init,
		ui_sdl_shutdown
	},
#endif /* UI_SDL */
	{NULL, NULL, NULL}
};
