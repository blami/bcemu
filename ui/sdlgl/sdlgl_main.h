/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.h: SDL OpenGL user interface frontend header. */

#ifndef __SDLGL_MAIN_H
#define __SDLGL_MAIN_H

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <GL/gl.h>
#include <GL/glu.h>


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
	GLuint texture;             /**< texture id */

	int16* audio_buf_l;
	int16* audio_buf_r;
	int16* audio_pos_l;
	int16* audio_pos_r;
	int audio_len;

	SDL_Event event;            /**< input event */

} t_sdlgl;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      sdlgl_init();
extern void     sdlgl_shutdown();
extern void     sdlgl_update_video();
extern void     sdlgl_update_audio();
extern void     sdlgl_update_input();

#endif /* __SDLGL_H */
