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
 * \returns                 1 if exited normally, otherwise 0
 */
int bc_emu_main(char* emu_name, char* ui_name)
{
	debug("entering bc_emu main...");

	/* initialize emulator */
	emu = modules_emu_find(emu_name);
	/* FIXME: asserts here should be handled by arch-dep error handler */
	assert(emu);
	assert(emu->init && emu->shutdown);

	/* initialize UI */
	ui = modules_ui_find(ui_name);
	/* FIXME: asserts here should be handled by arch-dep error handler */
	assert(ui);
	assert(ui->init && ui->shutdown);

	ui->init();

	/* application main-loop */
	debug("entering main-loop...");
	sleep(10);

	/* shutdown ui */
	ui->shutdown();

	return 1;
}

/**
 * Architecture-independent shutdown called after finishing bc_emu_main().
 */
void bc_emu_exit()
{
	debug("exiting bc_emu main...");

	/* cleanup memory */
	if(bc_emu_rom != NULL)
		xfree(bc_emu_rom);
}
