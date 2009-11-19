/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.c: SDL user interface frontend. */

#include "bc_emu.h"
#include "sdl.h"


static t_sdl* sdl;

/* -------------------------------------------------------------------------- *
 * Init, shutdown routines                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Initialize SDL user interface.
 */
int sdl_init()
{
	debug("SDL init");

	sdl = xmalloc(sizeof(t_sdl));
	sdl = memset(sdl, 0, sizeof(t_sdl));

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return 0;
	/* TODO configuration file section `ui.[sdl.]resoultion, ui.[sdl.]bpp */
	if(!(sdl->screen = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF)))
	{
		SDL_Quit();
		return 0;
	}

	/* TODO configuration file section `ui.[sdl.]keyrepeat */
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_WM_SetCaption("bc_emu", NULL);

	return 1;
}

/**
 * Shutdown SDL user interface.
 */
void sdl_shutdown()
{
	debug("SDL shutdown");
	assert(sdl != NULL);

	/* cleanup surfaces */
	if(sdl->screen)
		SDL_FreeSurface(sdl->screen);
	if(sdl->buffer)
		SDL_FreeSurface(sdl->buffer);

	/* cleanup sdl */
	xfree(sdl);

	SDL_Quit();
}

/* -------------------------------------------------------------------------- *
 * User interface API                                                         *
 * -------------------------------------------------------------------------- */

/**
 * Update audio output. Takes pre-filled emulator sound buffers as parameter
 * and converts them to SDL playable audio which is immediately processed.
 */
void sdl_update_audio()
{
}

/**
 * Update video output. Takes pre-filled emulator screen bitmap as parameter
 * and converts it to SDL surface of specified properties. Does centering and
 * scaling if emulator changes viewport/resolution and also immediate bliting.
 */
void sdl_update_video()
{
}

/**
 * Update input. Dump input devices state into structure pointed by input
 * pointer.
 * TODO read configuration section ui.[sdl.]keymapping.*
 * \param input         pointer to structure holding input data to be updated
 */
void sdl_update_input(t_input* input)
{
	assert(sdl);
	assert(input);

	/* cleanup previous state */
	memset(input, 0, sizeof(t_input));

	while(SDL_PollEvent(&sdl->event))
	{
		switch(sdl->event.type)
		{
		case SDL_QUIT:
			debug("SDL exit triggered");
			input->quit = 1;
			break;

		case SDL_KEYDOWN:
			debug("SDL keydown: %x", sdl->event.key.keysym.scancode);
			switch(sdl->event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				debug("SDL exit triggered");
				input->quit = 1;
				break;
			}
			break;
		}
	}
}
