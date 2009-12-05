/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.h: NEC PCEngine emulator header. */

#ifndef __PCE_MAIN_H
#define __PCE_MAIN_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * NEC PCEngine emulator.
 */
typedef struct
{
	/* memory */
	uint8 ram[0x8000];          /**< PCE RAM (32k) */
	uint8 vram[0x10000];        /**< PCE video RAM (64k) */
	uint8 satb[0x200];          /**< PCE SATB (sprite attribute table) */
	uint8 rom[0x100000];        /**< PCE HuCard ROM (1M) */

} t_pce;

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce*   pce;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      pce_init();
extern void     pce_reset();
extern void     pce_shutdown();

#endif /* __SDL_H */
