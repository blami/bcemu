/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.h: SDL user interface frontend header. */

#ifndef __SDL_MAIN_H
#define __SDL_MAIN_H

#include "SDL/SDL.h"


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * SDL user interface context.
 */
typedef struct
{
	SDL_Surface* screen;        /**< surface tied with screen (back buffer) */
	SDL_Surface* buffer;        /**< drawing buffer */

	int16* audio_buf_l;
	int16* audio_buf_r;
	int16* audio_pos_l;
	int16* audio_pos_r;
	int audio_len;

	uint32 ticks_begin;
	uint32 ticks_end;

	SDL_Event event;            /**< input event */

} t_sdl;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      sdl_init();
extern void     sdl_shutdown();
extern void     sdl_update_video();
extern void     sdl_update_audio();
extern void     sdl_update_input();

#endif /* __SDL_H */
