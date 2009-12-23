/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* psg.h: NEC PCEngine PSG (programmable sound generator) */

#ifndef __PCE_PSG_H
#define __PCE_PSG_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * NEC PCEngine PSG (programmable sound generator).
 */
typedef struct
{
	uint8 sel_ch;                   /**< selected channel */

	/* registers */
	uint8 balance;                  /**< balance (all channels 4+4b R,L) */
	uint8 noise;                    /**< white noise control and frequency */
	uint8 lfo_freq;                 /**< LFO (low-frequency oscillator) frequency */
	uint8 lfo_ctrl;                 /**< LFO (low-frequency oscillator) control */

	/* lookup tables */
	int16 freq_lut[92][32];
	uint8 noise_lut[0x1FFFF];

	/* channels */
	struct {
		int counter;                /**< waveform index counter */

		uint8 wav_index;            /**< wav data index */
		uint8 wav[32];              /**< wav data */
		uint16 freq;                /**< frequency */
		uint8 ctrl;                 /**< control (enable, DDA, volume) */
		uint8 balance;              /**< balance 4+4b L+R */

	} ch[8];                        /**< channel struct */

} t_pce_psg;

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

/*
 * Current channel shortcut.
 */
#define PSG_CH       pce_psg->ch[pce_psg->sel_ch]

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce_psg*   pce_psg;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int      pce_psg_init();
extern void     pce_psg_reset();
extern void     pce_psg_shutdown();

extern void     pce_psg_w(uint16, uint8);
extern void     pce_psg_fill(int16*, int16*, int);

#endif /* __PCE_PSG_H */

