/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdlgl.h: SDL OpenGL accelerated user interface frontend header. */

#ifndef __SDLGL_H
#define __SDLGL_H

#include "SDL/SDL.h"


/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      sdlgl_init();
extern void     sdlgl_shutdown();
extern void     sdlgl_update_video();
extern void     sdlgl_update_audio();
extern void     sdlgl_update_input();

#endif /* __SDLGL_H */
