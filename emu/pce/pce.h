/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.h: NEC PCEngine emulator public interface header. */

#ifndef __PCE_H
#define __PCE_H


/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      pce_init();
extern void     pce_reset();
extern void     pce_shutdown();
extern void     pce_frame();

#endif /* __SDL_H */
