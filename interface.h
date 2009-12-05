/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* interface.h: Common emulator/UI interface. */

#ifndef __INTERFACE_H
#define __INTERFACE_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * Structure containing data needed for video rendering.
 */
typedef struct
{
	uint8 *pixeldata;   /**< pixeldata */
	int width;          /**< width */
	int height;         /**< height */

	/**
	 * Viewport metadata.
	 */
	struct
	{
		int x;          /**< X position of viewport */
		int y;          /**< Y position of viewport */
		int width;      /**< width of viewport */
		int height;     /**< height of viewport */
		int ch;         /**< viewport width/height has been changed */

	} vp;

} t_video;

/**
 * Structure containing data needed for audio playback.
 */
typedef struct
{
	int sample_rate;    /**< sample rate */
	int buffer_size;    /**< sound buffer size (in bytes) */
	int16 *buffer[2];   /**< signed 16-bit stereo sound data */

} t_audio;

/**
 * Structure containing data needed for input device handling.
 */
typedef struct
{
	int quit;           /**< UI quit signal */
	int reset;          /**< UI reset signal */

	/* FIXME this is lame temporary solution. In reality there will be plenty
	 * emulators each with specific number of input device of various type
	 * and craziness. Emulator should specify its need of input device binding
	 * to some kind of cfg struct inside t_input and UI should configure itself
	 * to follow this cfg struct exporting right values which can be read
	 * directly by the emulator (hashtable?) */
	int button[2][255]; /**< input device buttons use constants */

} t_input;

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

#define INPUT_UP        0
#define INPUT_DOWN      1
#define INPUT_LEFT      2
#define INPUT_RIGHT     3
/* ... */
#define INPUT_START     8
#define INPUT_SELECT    10
/* ... */
#define INPUT_BUTTON1   64
#define INPUT_BUTTON2   65
/* ... */

#endif /* __INTERFACE_H */
