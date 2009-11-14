/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* module.c: Module interface. */

#include "bc_emu.h"


t_emu*  emu = NULL;     /* emulator module */
t_ui*   ui = NULL;      /* UI module */

/* -------------------------------------------------------------------------- *
 * Module lookup functions                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Find specified emulator module in bc_modules_emu array.
 * \param emu_id        desired emulator identifier
 * \return              pointer to selected emulator struct, if n/a NULL
 * \see bc_modules_emu
 */
t_emu* module_emu_find(const char* emu_id)
{
	int i = 0;
	t_emu* retval_emu = NULL;
	debug("finding emulator module...");

	if(emu_id == NULL)
		return NULL;

	/* find specified emulator */
	while(1)
	{
		if(bc_emu_modules_emu[i].id == NULL)
		{
			debug("emulator `%s' module isn't available!", emu_id);
			break;  /* not found */
		}
		else if(strncmp(bc_emu_modules_emu[i].id, emu_id, strlen(emu_id)) == 0)
		{
			debug("emulator module `%s' found", emu_id);
			retval_emu = &bc_emu_modules_emu[i];
			break;  /* found */
		}
		i++;

		/* never-ending cycle fuse */
		if(i > 100)
			fatal("unterminated emu_list[]!");
		}
	}

	return retval_emu;
}

/**
 * Find specified UI module in bc_modules_ui array.
 * \param ui_id             UI identifier
 * \return                  pointer to selected UI struct, if n/a NULL
 * \see bc_modules_ui
 */
t_ui* module_ui_find(const char* ui_id)
{
	int i = 0;
	t_ui* retval_ui = NULL;
	debug("finding UI module...");

	if(ui_id == NULL)
		return NULL;

	/* find specified UI */
	while(1)
	{
		if(bc_emu_modules_ui[i].id == NULL)
		{
			debug("UI module `%s' isn't available!", ui_id);
			break;  /* not found */
		}
		else if(strncmp(bc_emu_modules_ui[i].id, ui_id, strlen(ui_id)) == 0)
		{
			debug("UI module `%s' found", ui_id);
			retval_ui = &bc_emu_modules_ui[i];
			break;  /* found */
		}
		i++;

		/* never ending cycle fuse */
		if(i > 100)
			fatal("unterminated ui_list[]!");
	}

	return retval_ui;
}
