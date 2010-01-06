/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.h: SDL user interface frontend header. */

#ifndef __SDL_H
#define __SDL_H

#include "SDL/SDL.h"


/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      sdl_init();
extern void     sdl_shutdown();
extern void     sdl_update_video();
extern void     sdl_update_audio();
extern void     sdl_update_input();
extern void     sdl_frame_begin();
extern void     sdl_frame_end();

#endif /* __SDL_H */
