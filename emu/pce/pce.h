/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.h: NEC PCEngine emulator header. */

#ifndef __PCE_H
#define __PCE_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * NEC PCEngine emulator.
 */
typedef struct
{
	uint8 ram[0x8000];          /* device RAM */
	uint8 rom[0x100000];        /* device ROM */

} t_pce;

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce*   pce;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      pce_init();
extern void     pce_shutdown();

#endif /* __SDL_H */