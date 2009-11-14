/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* bc_emu.c: Program entry point. */

#include "bc_emu.h"


uint8* bc_emu_rom = NULL;

/*
 * NOTE:
 * This file acts only as holder of arch non-specific code. main() or other
 * de-facto arch-specific routines are part of architecture .c file in arch/
 * directory.
 */

/* -------------------------------------------------------------------------- *
 * Program life-cycle                                                         *
 * -------------------------------------------------------------------------- */

/**
 * Architecture-independent main() called from main() or its equivalent.
 * \param emu_name          emulator name
 * \param ui_name           UI frontend name
 * \param dbg_name          debugger name
 * \returns                 1 if exited normally, otherwise 0
 */
int bc_emu_main(char* emu_name, char* ui_name, char* dbg_name)
{
	debug("entering bc_emu main-loop...");

	return 1;
}

/**
 * Architecture-independent shutdown called after finishing bc_emu_main().
 */
void bc_emu_exit()
{
	debug("exiting bc_emu main-loop...");

	/* cleanup memory */
	if(bc_emu_rom != NULL)
		xfree(bc_emu_rom);
}
