/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* psg.c: NEC PCEngine HuC6280 PSG (programmable sound generator) emulator */

#include "bc_emu.h"
#include "emu/pce/pce_main.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/psg.h"
#include "emu/pce/rom.h"


t_pce_psg* pce_psg = NULL;

/* -------------------------------------------------------------------------- *
 * Init, reset, shutdown routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize PSG.
 * \return              returns 1 if success, otherwise 0
 */
int pce_psg_init()
{
	debug("PSG init");

	pce_psg = xmalloc(sizeof(t_pce_psg));
	pce_psg_reset();

	return 1;
}

/**
 * Reset PSG.
 */
void pce_psg_reset()
{
	assert(pce_psg);
	debug("PSG reset");

	memset(pce_psg, 0, sizeof(t_pce_psg));
}

/**
 * Shutdown PSG.
 */
void pce_psg_shutdown()
{
	debug("PSG shutdown");
	assert(pce_psg);

	/* cleanup pce_psg */
	xfree(pce_psg);
}

/* -------------------------------------------------------------------------- *
 * PSG                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Write PSG memory. PSG starts at address 0x0800 and ends at 0x0809.
 * \param addr          address
 * \param data          unsigned data to be written to PSG memory
 */
void pce_psg_w(uint16 addr, uint8 data)
{
	assert(pce_psg);

	//debug("PSG write: addr=%08x msb=%d data=%d", addr, msb, data);

	switch(addr)
	{
		/* channel select */
		case 0x0800:
			pce_psg->sel_ch = (data & 7);
			break;

		/* sound balance (all channels) */
		case 0x0801:
			pce_psg->balance = data;
			break;

		/* white noise */
		case 0x0807:
			pce_psg->noise = data;
			break;

		/* LFO (low frequency oscilator) frequency */
		case 0x0808:
			pce_psg->lfo_freq = data;
			break;

		/* LFO (low frequency oscilator) control */
		case 0x0809:
			pce_psg->lfo_ctrl = data;
			break;

		/*
		 * Current channel specific:
		 */

		/* channel frequency (LSB, MSB) */
		case 0x0802:
			PSG_CH.freq = (PSG_CH.freq & 0x0F00) | (data);
			break;
		case 0x0803:
			PSG_CH.freq = (PSG_CH.freq & 0x00FF) | ((data & 0x0F) << 8);
			break;

		/* channel ctrl (enable, DDA, volume) */
		case 0x0804:
			PSG_CH.ctrl = data;
			/* if needed set wave index to 0 */
			if((data & 0xC0) == 0x40)
				PSG_CH.wav_index = 0;
			break;

		/* channel balance */
		case 0x0805:
			PSG_CH.balance = data;
			break;

		/* channel waveform data */
		case 0x0806:
			PSG_CH.wav[PSG_CH.wav_index] = data;
			/* increase waveform data index */
			PSG_CH.wav_index = ((PSG_CH.wav_index + 1) & 0x1F);
			break;

		default:
			debug("PSG UNKNOWN write at %x", addr);
	}
}

/**
 * Emulate PSG. Fill output buffers with data of specified length in samples.
 * \param buf_l         left channel output buffer
 * \param buf_r         right channel output buffer
 * \param length        length of requested data in samples
 */
void pce_psg_output(int16 *buf_l, int16 *buf_r, int length)
{
	assert(pce_psg);

	while(length > 0)
	{
		int ch;
		int sample[2] = {0, 0};    /* samples */

		int first_ch; 
		int last_ch;

		/* skip channels 0,1 if LFO (low frequency oscillator) is enabled */
		first_ch = ((pce_psg->lfo_ctrl & 3) == 0)
			? 0 : 2;
		/* skip channels 4,5 if white noise is enabled */
		last_ch = (pce_psg->noise & 0x80)
			? 4 : 6;

		for(ch = first_ch; ch < last_ch; ch ++)
		{
			if((pce_psg->ch[ch].ctrl & 0xC0) != 0x80)
				continue; /* channel is not enabled */

			/* compute channel balance as sum of global balance and channel
			 * balance */
			int l_bal = (pce_psg->balance >> 4) & 0x0F
				+ (pce_psg->ch[ch].balance >> 4) & 0x0F;
			int r_bal = (pce_psg->balance >> 0) & 0x0F
				+ (pce_psg->ch[ch].balance >> 0) & 0x0F;

			/* compute channel volume level as sum of channel volume and
			 * balance */
			int l_vol = l_bal + (pce_psg->ch[ch].ctrl & 0x1F);
			int r_vol = r_bal + (pce_psg->ch[ch].ctrl & 0x1F);

			/* generate samples */

			/* largest step (PSG clock / lenght of waveform) */
			int step_base = (3580000 / 32);
			/* current step (avoid division by zero) */
			int step = (pce_psg->ch[ch].freq != 0)
				? step_base / pce_psg->ch[ch].freq : 0;

			/* calculate offset (upper 5 bits of counter == data length) */
			int offset = (pce_psg->ch[ch].counter >> 12) & 0x1F;

			pce_psg->ch[ch].counter += step;

			/* get data and add to samples */
			int data = (pce_psg->ch[ch].wav[offset] & 0x1F);

			sample[0] = (sample[0] + (data * l_vol));
			sample[1] = (sample[1] + (data * r_vol));
		}

		if(sample[0] & 0x8000)
			sample[0] ^= 0x8000;

		if(sample[1] & 0x8000)
			sample[1] ^= 0x8000;

		*buf_l++ = sample[0];
		*buf_r++ = sample[1];

		--length;
	}
}
