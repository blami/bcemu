/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.c: SDL user interface frontend. */

#include "bc_emu.h"
#include "ui/sdl/sdl_main.h"


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

	/* prepare buffer SDL surface */
	sdl->buffer = SDL_CreateRGBSurface(sdl->screen->flags,
		1024,       /* FIXME this info should be set by emu, but ui is initialized before emu */
		256,        /* FIXME -||- */
		sdl->screen->format->BitsPerPixel,
		sdl->screen->format->Rmask,
		sdl->screen->format->Gmask,
		sdl->screen->format->Bmask,
		sdl->screen->format->Amask);

	/* FIXME filthy */
	emu_video->pixeldata = (uint8 *)&sdl->buffer->pixels[0];

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
	SDL_Rect vp_rect;

	/*
	vp_rect.x = 0;
	vp_rect.y = 0;
	vp_rect.w = 1024;
	vp_rect.h = 256;
	*/

	//SDL_FillRect(sdl->buffer, &sdl->buffer->clip_rect, SDL_MapRGB(sdl->screen->format, 0xFF, 0x0, 0xFF));
	SDL_BlitSurface(sdl->buffer, &sdl->buffer->clip_rect, sdl->screen, &sdl->screen->clip_rect);
	SDL_Flip(sdl->screen);
}

/**
 * Update input. Dump input devices state into structure pointed by input
 * pointer.
 * TODO read configuration section ui.[sdl.]keymapping.*
 */
void sdl_update_input()
{
	assert(sdl);
	assert(emu_input);

	/* cleanup previous state */
	memset(emu_input, 0, sizeof(t_input));

	while(SDL_PollEvent(&sdl->event))
	{
		switch(sdl->event.type)
		{
		case SDL_QUIT:
			debug("SDL exit triggered");
			emu_input->quit = 1;
			break;

		case SDL_KEYDOWN:
			debug("SDL keydown: %x", sdl->event.key.keysym.scancode);
			switch(sdl->event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				debug("SDL exit triggered");
				emu_input->quit = 1;
				break;
			}
			break;
		}
	}
}
