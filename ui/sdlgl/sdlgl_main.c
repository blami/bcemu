/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* sdl.c: SDL OpenGL user interface frontend. */

#include "bc_emu.h"
#include "ui/sdlgl/sdlgl_main.h"


static t_sdlgl* sdlgl;
static void sdlgl_audio_fill(void*, uint8*, int);
static inline int sdlgl_npot(int);

/* -------------------------------------------------------------------------- *
 * Init, shutdown routines                                                    *
 * -------------------------------------------------------------------------- */

/**
 * Initialize SDL user interface.
 */
int sdlgl_init()
{
	debug("SDL OpenGL init");

	sdlgl = xmalloc(sizeof(t_sdlgl));
	sdlgl = memset(sdlgl, 0, sizeof(t_sdlgl));

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		return 0;

	/* prepare OpenGL */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	/* TODO configuration file section `ui.[sdlgl.]resoultion, ui.[sdl.]bpp */
	if(!(sdlgl->screen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL)))
	{
		SDL_Quit();
		debug("SDL OpenGL error. Couldn't open screen!");
		return 0;
	}

	debug("SDL OpenGL screen opened: %dx%dx%d", 640, 480, 16);

	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f); 
	/* aspect ratio */
	//float ar = (GLfloat)(640 / 480);

	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glViewport(0, 0, 640, 480);

	/* hint and shade model settings */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	sdlgl->texture = -1;

	debug("SDL OpenGL prepared");

	/* TODO configuration file section `ui.[sdl.]audio_bitrate,
	 * ui.[sdl.]audio_ch */
	SDL_AudioSpec as;

	/* setup audio device request */
	as.freq = 22050;
	as.format = AUDIO_S16; /* signed 16bit */
	as.channels = 2;
	as.samples = 4096;
	as.callback = sdlgl_audio_fill;
	as.userdata = NULL;

	if(SDL_OpenAudio(&as, NULL) < 0)
	{
		SDL_Quit();
		debug("SDL error. Couldn't open audio device!");
		return 0;
	}
	debug("SDL audio device opened: %d channels, freq=%d", 2, 44100);

	sdlgl->audio_len = 0;
	//sdl->audio_pos = sdl->audio_buf;

	SDL_PauseAudio(0); /* trigger callback */

	/* prepare buffer SDL surface */
	sdlgl->buffer = SDL_CreateRGBSurface(SDL_SWSURFACE,
		1024,       /* FIXME this info should be set by emu, but ui is initialized before emu */
		256,        /* FIXME -||- */
		sdlgl->screen->format->BitsPerPixel,
		sdlgl->screen->format->Rmask,
		sdlgl->screen->format->Gmask,
		sdlgl->screen->format->Bmask,
		sdlgl->screen->format->Amask
		);
	sdlgl->texture = NULL;

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
void sdlgl_shutdown()
{
	debug("SDL OpenGL shutdown");
	assert(sdlgl != NULL);

	/* cleanup surfaces */
	if(sdlgl->screen)
		SDL_FreeSurface(sdlgl->screen);
	if(sdlgl->buffer)
		SDL_FreeSurface(sdlgl->buffer);

	/* close audio device */
	SDL_CloseAudio();

	/* cleanup sdl */
	xfree(sdlgl);

	SDL_Quit();
}

/* -------------------------------------------------------------------------- *
 * User interface API                                                         *
 * -------------------------------------------------------------------------- */

/**
 * Update audio output. Takes pre-filled emulator sound buffers as parameter
 * and converts them to SDL playable audio which is immediately processed.
 */
void sdlgl_update_audio()
{
	//memset(emu_audio->buffer[1], 10000, emu_audio->buffer_size);
	sdlgl->audio_buf_l = emu_audio->buffer[0];
	sdlgl->audio_buf_r = emu_audio->buffer[1];

	sdlgl->audio_pos_l = sdlgl->audio_buf_l;
	sdlgl->audio_pos_r = sdlgl->audio_buf_r;

	sdlgl->audio_len = emu_audio->buffer_size;
}

/**
 * Update video output. Takes pre-filled emulator screen bitmap as parameter
 * and converts it to SDL surface of specified properties. Does centering and
 * scaling if emulator changes viewport/resolution and also immediate bliting.
 */
void sdlgl_update_video()
{
	SDL_Rect vp_rect;
	GLuint texture;

	vp_rect.x = emu_video->vp.x;
	vp_rect.y = emu_video->vp.y;
	vp_rect.w = emu_video->vp.width;
	vp_rect.h = emu_video->vp.height;

	/* lock surface and copy image data */
	SDL_LockSurface(sdlgl->buffer);
	memcpy(&sdlgl->buffer->pixels[0], emu_video->pixeldata, (
		emu_video->width *
		emu_video->height
		)
		* sizeof(uint16));
	SDL_UnlockSurface(sdlgl->buffer);

	/* prepare texture */
	if(&sdlgl->texture == -1)
	{
		glGenTextures(1, &sdlgl->texture);
	}

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, sdlgl->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		3,
		sdlgl->buffer->clip_rect.w,
		sdlgl->buffer->clip_rect.h,
		0,
		GL_RGB,
		GL_UNSIGNED_SHORT_5_6_5,
		sdlgl->buffer->pixels);

	/*
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	*/

	/* calculate viewport coords */
	float vx, vy, vw, vh;

	vx = (float)vp_rect.x / emu_video->width;
	vy = (float)vp_rect.y / emu_video->height;
	vw = (float)(vp_rect.x + vp_rect.w) / emu_video->width;
	vh = (float)(vp_rect.y + vp_rect.h) / emu_video->height;

	glBegin(GL_QUADS);
		glTexCoord2f(vx, vh); glVertex2i(-1,-1);
		glTexCoord2f(vw, vh); glVertex2i( 1,-1);
		glTexCoord2f(vw, vy); glVertex2i( 1, 1);
		glTexCoord2f(vx, vy); glVertex2i(-1, 1);
	glEnd();

	/*
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	*/

	glFlush();

	glDeleteTextures(1, &sdlgl->texture);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sdlgl->texture);

	/* render frame */
	SDL_GL_SwapBuffers();
}

/**
 * Update input. Dump input devices state into structure pointed by input
 * pointer.
 * TODO read configuration section ui.[sdl.]keymapping.*
 */
void sdlgl_update_input()
{
	assert(sdlgl);
	assert(emu_input);

	/* cleanup previous state */
	//memset(emu_input, 0, sizeof(t_input));
	emu_input->quit = 0;
	emu_input->reset = 0;

	while(SDL_PollEvent(&sdlgl->event))
	{
		switch(sdlgl->event.type)
		{
		case SDL_QUIT:
			debug("SDL exit triggered");
			emu_input->quit = 1;
			break;

		case SDL_KEYDOWN:
			debug("SDL keydown: %x", sdlgl->event.key.keysym.scancode);
			switch(sdlgl->event.key.keysym.sym)
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
			debug("SDL keyup: %x", sdlgl->event.key.keysym.scancode);
			switch(sdlgl->event.key.keysym.sym)
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
static void sdlgl_audio_fill(void* data, uint8* stream, int len)
{
	assert(sdlgl);

	if(sdlgl->audio_len == 0)
		return;

	len = (len > sdlgl->audio_len) ? sdlgl->audio_len : len;

	SDL_MixAudio(stream, sdlgl->audio_pos_l, len, SDL_MIX_MAXVOLUME);
	SDL_MixAudio(stream, sdlgl->audio_pos_r, len, SDL_MIX_MAXVOLUME);

	sdlgl->audio_pos_l += len;
	sdlgl->audio_pos_r += len;
	sdlgl->audio_len -= len;
}
