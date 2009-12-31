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
static void sdl_audio_fill(void*, uint8*, int);

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
		debug("SDL error. Couldn't open screen!");
		return 0;
	}
	debug("SDL screen opened: %dx%dx%d", 640, 480, 16);

	/* TODO configuration file section `ui.[sdl.]audio_bitrate,
	 * ui.[sdl.]audio_ch */
	SDL_AudioSpec as;

	/* setup audio device request */
	as.freq = 22050;
	as.format = AUDIO_S16; /* signed 16bit */
	as.channels = 2;
	as.samples = 4096;
	as.callback = sdl_audio_fill;
	as.userdata = NULL;

	if(SDL_OpenAudio(&as, NULL) < 0)
	{
		SDL_Quit();
		debug("SDL error. Couldn't open audio device!");
		return 0;
	}
	debug("SDL audio device opened: %d channels, freq=%d", 2, 44100);

	sdl->audio_len = 0;
	//sdl->audio_pos = sdl->audio_buf;

	SDL_PauseAudio(0); /* trigger callback */

	/* prepare buffer SDL surface */
	sdl->buffer = SDL_CreateRGBSurface(sdl->screen->flags,
		1024,       /* FIXME this info should be set by emu, but ui is initialized before emu */
		256,        /* FIXME -||- */
		16,         /* FIXME -||- */
#ifdef LSB
		0xF800,
		0x07E0,
		0x001F,
		0x0000
#else
		0xF100,
		0x0E70,
		0x008F,
		0x0000
#endif /* LSB */
		);

	/* FIXME filthy */
	//emu_video->pixeldata = (uint8 *)&sdl->buffer->pixels[0];

	/* TODO configuration file section `ui.[sdl.]keyrepeat */
	//SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
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

	/* close audio device */
	SDL_CloseAudio();

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
	//memset(emu_audio->buffer[1], 10000, emu_audio->buffer_size);
	sdl->audio_buf_l = emu_audio->buffer[0];
	sdl->audio_buf_r = emu_audio->buffer[1];

	sdl->audio_pos_l = sdl->audio_buf_l;
	sdl->audio_pos_r = sdl->audio_buf_r;

	sdl->audio_len = emu_audio->buffer_size;
}

/**
 * Update video output. Takes pre-filled emulator screen bitmap as parameter
 * and converts it to SDL surface of specified properties. Does centering and
 * scaling if emulator changes viewport/resolution and also immediate bliting.
 */
void sdl_update_video()
{
	SDL_Rect vp_rect;

	/* lock surface and copy image data */
	SDL_LockSurface(sdl->buffer);
	memcpy(&sdl->buffer->pixels[0], emu_video->pixeldata,
		(emu_video->width * emu_video->height) * sizeof(uint16));
	SDL_UnlockSurface(sdl->buffer);
	
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
	//memset(emu_input, 0, sizeof(t_input));
	emu_input->quit = 0;
	emu_input->reset = 0;

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
			/* UI (should be cleared every tick) */
			case SDLK_ESCAPE:   /* ESC,q: quit */
			case SDLK_q:
				debug("SDL exit triggered");
				emu_input->quit = 1;
				break;
			case SDLK_r:        /* r: reset */
				debug("SDL reset triggered");
				emu_input->reset = 1;
				break;

			/* emulator (must be statefull, don't forget to set SDL_KEYUP!!!) */
			case SDLK_UP:
				debug("SDL emu button:up");
				emu_input->button[0][INPUT_UP] = 1;
				break;
			case SDLK_DOWN:
				debug("SDL emu button:down");
				emu_input->button[0][INPUT_DOWN] = 1;
				break;
			case SDLK_LEFT:
				debug("SDL emu button:left");
				emu_input->button[0][INPUT_LEFT] = 1;
				break;
			case SDLK_RIGHT:
				debug("SDL emu button:right");
				emu_input->button[0][INPUT_RIGHT] = 1;
				break;
			case SDLK_a:
				debug("SDL emu button:1");
				emu_input->button[0][INPUT_BUTTON1] = 1;
				break;
			case SDLK_s:
				debug("SDL emu button:2");
				emu_input->button[0][INPUT_BUTTON2] = 1;
				break;
			case SDLK_RETURN:
				debug("SDL emu button:start");
				emu_input->button[0][INPUT_START] = 1;
				break;
			}
			break;

		case SDL_KEYUP:
			debug("SDL keyup: %x", sdl->event.key.keysym.scancode);
			switch(sdl->event.key.keysym.sym)
			{
			/* emulator (must be statefull, don't forget to set SDL_KEYDOWN!!!) */
			case SDLK_UP:
				debug("SDL emu button:up");
				emu_input->button[0][INPUT_UP] = 0;
				break;
			case SDLK_DOWN:
				debug("SDL emu button:down");
				emu_input->button[0][INPUT_DOWN] = 0;
				break;
			case SDLK_LEFT:
				debug("SDL emu button:left");
				emu_input->button[0][INPUT_LEFT] = 0;
				break;
			case SDLK_RIGHT:
				debug("SDL emu button:right");
				emu_input->button[0][INPUT_RIGHT] = 0;
				break;
			case SDLK_a:
				debug("SDL emu button:1");
				emu_input->button[0][INPUT_BUTTON1] = 0;
				break;
			case SDLK_s:
				debug("SDL emu button:2");
				emu_input->button[0][INPUT_BUTTON2] = 0;
				break;
			case SDLK_RETURN:
				debug("SDL emu button:start");
				emu_input->button[0][INPUT_START] = 0;
				break;
			}
			break;
		}
	}
}

/* -------------------------------------------------------------------------- *
 * Helpers                                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Fill SDL audio buffer running in separate thread (by definition of SDL audio
 * subsystem.
 * \param udata         pointer to audio data
 * \param stream        pointer to audio buffer to be filled
 * \param len           byte length of the audio buffer
 */
static void sdl_audio_fill(void* data, uint8* stream, int len)
{
	assert(sdl);

	if(sdl->audio_len == 0)
		return;

	len = (len > sdl->audio_len) ? sdl->audio_len : len;
	SDL_MixAudio(stream, sdl->audio_pos_l, len, SDL_MIX_MAXVOLUME);
	SDL_MixAudio(stream, sdl->audio_pos_r, len, SDL_MIX_MAXVOLUME);
	sdl->audio_pos_l += len;
	sdl->audio_pos_r += len;
	sdl->audio_len -= len;
}
