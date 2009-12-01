/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.c: NEC PCEngine emulator. */

#include "bc_emu.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/psg.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/rom.h"


t_pce* pce = NULL;

/* -------------------------------------------------------------------------- *
 * Init, shutdown, reset routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize PCE emulator.
 */
int pce_init()
{
	debug("PCE init");

	pce = xmalloc(sizeof(t_pce));
	memset(pce, 0, sizeof(t_pce));

	/* initialize emulator subsystems */
	pce_rom_parse();
	pce_cpu_init();
	pce_psg_init();

	return 1;
}

/**
 * Shutdown PCE emulator.
 */
void pce_shutdown()
{
	debug("PCE shutdown");
	assert(pce);

	/* shutdown emulator subsystems */
	pce_psg_shutdown();
	pce_cpu_shutdown();

	/* clean pce */
	xfree(pce);
}

/**
 * Reset PCE emulator.
 */
void pce_reset()
{
	assert(pce);

	pce_psg_reset();
}

/* -------------------------------------------------------------------------- *
 * PCE                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Run emulation for one frame.
 */
void pce_frame()
{

}
