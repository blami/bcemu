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

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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

	vp_rect.x = emu_video->vp.x;
	vp_rect.y = emu_video->vp.y;
	vp_rect.w = emu_video->vp.width;
	vp_rect.h = emu_video->vp.height;

	/* fill entire screen with black color if viewport was changed */
	if(emu_video->vp.ch)
		SDL_FillRect(sdl->screen, &sdl->screen->clip_rect,
			SDL_MapRGB(sdl->screen->format, 0x00, 0x00, 0x00));
	/* draw current viewport */
	SDL_BlitSurface(sdl->buffer, &vp_rect, sdl->screen, &sdl->screen->clip_rect);

	/* flip surfaces (doublebuffer) */
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
			/* ESC or Q: quit */
			case SDLK_ESCAPE:
			case SDLK_q:
				debug("SDL exit triggered");
				emu_input->quit = 1;
				break;
			/* R: reset */
			case SDLK_r:
				debug("SDL reset triggered");
				emu_input->reset = 1;
				break;
			/* UP: up button */
			case SDLK_UP:
				debug("SDL emu button:up");
				emu_input->button[0][INPUT_UP] = 1;
				break;
			/* DOWN: down button */
			case SDLK_DOWN:
				debug("SDL emu button:down");
				emu_input->button[0][INPUT_DOWN] = 1;
				break;
			/* LEFT: left button */
			case SDLK_LEFT:
				debug("SDL emu button:left");
				emu_input->button[0][INPUT_LEFT] = 1;
				break;
			/* RIGHT: right button */
			case SDLK_RIGHT:
				debug("SDL emu button:right");
				emu_input->button[0][INPUT_RIGHT] = 1;
				break;
			/* A: button 1 */
			case SDLK_a:
				debug("SDL emu button:1");
				emu_input->button[0][INPUT_BUTTON1] = 1;
				break;
			/* S: button 2 */
			case SDLK_s:
				debug("SDL emu button:2");
				emu_input->button[0][INPUT_BUTTON2] = 1;
				break;
			/* RETURN: start button */
			case SDLK_RETURN:
				debug("SDL emu button:start");
				emu_input->button[0][INPUT_START] = 1;
				break;
			}
			break;
		}
	}
}
