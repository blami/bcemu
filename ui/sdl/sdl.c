/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.c: SDL user interface frontend. */

#include "bc_emu.h"
#include "ui/sdl/sdl.h"


SDL_Surface* sdl_surfscr;
SDL_Surface* sdl_surfbuf;

/* -------------------------------------------------------------------------- *
 * Init, shutdown routines                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Initialize SDL user interface.
 */
void sdl_ui_init()
{
	debug("SDL init");

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("couldn't initialize SDL");

	// TODO configuration file sdl.resolution, sdl.bpp
	if(!(sdl_surfscr = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF)))
	{
		SDL_Quit();
		fatal("couldn't initialize SDL video surface");
	}

	// TODO configuration file sdl.keyrepeat (.delay, .interval)
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	SDL_WM_SetCaption("bc_emu", NULL);
}

/**
 * Shutdown SDL user interface.
 */
void sdl_ui_shutdown()
{
	debug("SDL shutdown");

	if(sdl_surfscr)
		SDL_FreeSurface(sdl_surfscr);
	if(sdl_surfbuf)
		SDL_FreeSurface(sdl_surfbuf);

	SDL_Quit();
}

/* -------------------------------------------------------------------------- *
 * User interface API                                                         *
 * -------------------------------------------------------------------------- */

/**
 * Update input devices.
 */
void sdl_update_input()
{
	// TODO support joysticks and gamepads
	// TODO configuration of input devices (currently only one device
	// supported)

	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
		SDL_QUIT:
			debug("SDL exit triggered")
			// FIXME exit cleanly
			exit(EXIT_SUCCESS);
			break;

		SDL_KEYDOWN:
			debug("SDL keydown: %x", e.key.keysym.scancode);
			switch(e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				debug("SDL exit triggered");
				// FIXME exit cleanly
				exit(EXIT_SUCCESS);
				break;
			default:
				debug("SDL unbound key: %x", e.key.keysym.scancode);
			}
			break;
		}
	}
}

/**
 * Update video output.
 */
void sdl_update_video()
{
}

/**
 * Update audio output.
 */
void sdl_update_audio()
{
}

/* -------------------------------------------------------------------------- *
 * Private functions, helpers                                                 *
 * -------------------------------------------------------------------------- */

