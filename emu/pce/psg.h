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
 * Programmable sound generator.
 */
typedef struct
{
	uint8 sel_ch;               /**< selected channel */

	uint8 balance;              /**< sound balance (all channels) */
	uint8 noise_freq;           /**< white noise frequency */
	uint8 noise_ctrl;           /**< white noise control */
	uint8 lfo_freq;             /**< LFO (low-frequency oscillator) frequency */
	uint8 lfo_ctrl;             /**< LFO (low-frequency oscillator) control */

	struct {
		int counter;            /* Waveform index counter */
		uint16 frequency;       /* Channel frequency */
		uint8 control;          /* Channel enable, DDA, volume */
		uint8 balance;          /* Channel balance */
		uint8 waveform[32];     /* Waveform data */
		uint8 waveform_index;   /* Waveform data index */
	} channel[8];

} t_pce_psg;



/* Macro to access currently selected PSG channel */
#define PSGCH   psg.channel[psg.select]


/* Global variables */
extern t_psg psg;

/* Function prototypes */
int psg_init(void);
void psg_reset(void);
void psg_shutdown(void);
void psg_w(uint16 address, uint8 data);
void psg_update(int16 *bufl, int16 *bufr, int length);

#endif /* __PCE_PSG_H */

