/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* module.h: Module interface. */

#ifndef __MODULE_H
#define __MODULE_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * Structure holding emulator module.
 */
typedef struct struct_mod_emu
{
	char* id;               /* emulator identifier (e.g. pce) */

	int (*init)();
	int (*shutdown)();
} t_emu;

/**
 * Structure holding UI module.
 */
typedef struct struct_mod_ui
{
	char* id;               /* UI identifier (e.g. sdl) */

	int (*init)();
	int (*shutdown)();
} t_ui;

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_emu*   emu;
extern t_ui*    ui;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern t_ui*    module_ui_find(const char*);
extern t_emu*   module_emu_find(const char*);

#endif /* __MODULE_H */
