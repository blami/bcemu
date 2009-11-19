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
 * Emulator module.
 */
typedef struct
{
	char* id;                   /* emulator identifier (e.g. pce) */

	int (*init)(void*, void*);
	void (*shutdown)(); 

} t_emu;

/**
 * UI module.
 */
typedef struct
{
	char* id;                   /* UI identifier (e.g. sdl) */

	int (*init)();
	void (*shutdown)();
	void (*update_audio)();
	void (*update_video)();
	void (*update_input)(void*);

} t_ui;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern t_ui*     module_ui_find(const char*);
extern t_emu*    module_emu_find(const char*);

#endif /* __MODULE_H */
