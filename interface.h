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
 * Video data.
 */
typedef struct
{
	uint8 *pixeldata;   /**< pixeldata */
	int width;          /**< width */
	int height;         /**< height */
	int bpp;            /**< bits per pixel (color depth) */

	/**
	 * Viewport metadata.
	 */
	struct
	{
		int x;          /**< X position of viewport */
		int y;          /**< Y position of viewport */
		int width;      /**< width of viewport */
		int height;     /**< height of viewport */
		int old_width;  /**< old width of viewport */
		int old_height; /**< old height of viewport */
		int changed;    /**< viewport width/height has been changed */

	} viewport;

} t_video;

/**
 * Audio data.
 */
typedef struct
{
	int sample_rate;    /**< sample rate */
	int buffer_size;    /**< sound buffer size (in bytes) */
	int16 *buffer[2];   /**< signed 16-bit stereo sound data */

} t_audio;

/**
 * Input data.
 */
typedef struct
{
	int quit;           /**< UI quit signal */

	/* TODO refactor */
	uint8 dev[5];       /* Can be any of the DEVICE_* values */
	uint32 pad[5];      /* Can be any of the INPUT_* bitmasks */
	uint32 system;      /* Can be any of the SYSTEM_* bitmasks */

} t_input;

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

/*
 * Input constants
 */

#endif /* __INTERFACE_H */