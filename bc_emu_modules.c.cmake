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
 * List of available emulator modules. List must be terminated by all-NULL
 * entry.
 * \see t_module_emu
 * \code
 * {
 *     "id",        // emulator id
 *     init,        // emulators init() callback (pointer to function)
 *     shutdown     // emulators shutdown() callback
 * };
 * \endcode
 */
t_emu emu_modules_emu[] =
{
#ifdef EMU_PCE
	/* NEC PC Engine emulator */
	{
		"pce",
		pce_init,
		pce_shutdown
	},
#endif /* EMU_PCE */

	{NULL, NULL, NULL}
};

/**
 * List of available UI modules. List must be terminated by all-NULL entry.
 * \see t_module_ui
 * \code
 * {
 *     "id",        // UI id
 *     init,        // init() callback (pointer to function)
 *     shutdown     // shutdown() callback
 *     update_audio // update_audio() callback (takes param from emu)
 *     update_video // update_video() callback (takes param from emu)
 *     update_input // update_input() callback (returns input struct)
 * };
 * \endcode
 */
t_ui emu_modules_ui[] =
{
#ifdef UI_SDL
	/* SDL UI */
	{
		"sdl",
		sdl_init,
		sdl_shutdown,
		sdl_update_audio,
		sdl_update_video,
		sdl_update_input
	},
#endif /* UI_SDL */

	{NULL, NULL, NULL, NULL, NULL, NULL}
};