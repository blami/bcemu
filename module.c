/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* module.c: Module interface. */

#include "bc_emu.h"


/* -------------------------------------------------------------------------- *
 * Module lookup functions                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Find specified emulator module in bcs_emu array.
 * \param emu_id        desired emulator identifier
 * \return              pointer to selected emulator struct, if n/a NULL
 * \see bcs_emu
 */
t_emu* module_emu_find(const char* emu_id)
{
	int i = 0;
	t_emu* retval_emu = NULL;

	if(emu_id == NULL)
		return NULL;

	debug("searching emulator module: `%s'", emu_id);

	/* find specified emulator */
	while(1)
	{
		if(emu_modules_emu[i].id == NULL)
		{
			debug("not found: emulator module `%s'", emu_id);
			break;  /* not found */
		}
		else if(strncmp(emu_modules_emu[i].id, emu_id, strlen(emu_id)) == 0)
		{
			debug("found: emulator module `%s'", emu_id);
			retval_emu = &emu_modules_emu[i];
			break;  /* found */
		}
		i++;
	}

	return retval_emu;
}

/**
 * Find specified UI module in bcs_ui array.
 * \param ui_id             UI identifier
 * \return                  pointer to selected UI struct, if n/a NULL
 * \see bcs_ui
 */
t_ui* module_ui_find(const char* ui_id)
{
	int i = 0;
	t_ui* retval_ui = NULL;

	if(ui_id == NULL)
		return NULL;

	debug("searching UI module: `%s'", ui_id);

	/* find specified UI */
	while(1)
	{
		if(emu_modules_ui[i].id == NULL)
		{
			debug("not found: UI module `%s'", ui_id);
			break;  /* not found */
		}
		else if(strncmp(emu_modules_ui[i].id, ui_id, strlen(ui_id)) == 0)
		{
			debug("found: UI module `%s'", ui_id);
			retval_ui = &emu_modules_ui[i];
			break;  /* found */
		}
		i++;
	}

	return retval_ui;
}
