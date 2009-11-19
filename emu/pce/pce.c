/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.c: NEC PCEngine emulator. */

#include "bc_emu.h"
#include "pce.h"


t_pce* pce;

/* -------------------------------------------------------------------------- *
 * Init, shutdown, reset routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize PCE emulator.
 */
int pce_init(t_video* video, t_audio* audio)
{
	debug("PCE init");
	assert(video);
	assert(audio);

	pce = xmalloc(sizeof(t_pce));
	memset(pce, 0, sizeof(t_pce));

	/* initialize emulator subsystems */

	return 1;
}

/**
 * Shutdown PCE emulator.
 */
void pce_shutdown()
{
	debug("PCE shutdown");
	assert(pce);

	/* clean pce */
	xfree(pce);
}

/**
 * Reset PCE emulator.
 */
void pce_reset()
{

}
