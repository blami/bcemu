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
	int i,j;
	uint32 noise_lfsr = 0x10000; /* noise lfsr */
	debug("PSG init");

	pce_psg = xmalloc(sizeof(t_pce_psg));

	/* prepare frequency lookup tables */

	/* build frequency step lookup table */
	for(i = 91; i > 0; i--)
		for(j = 0; j < 32; j++)
		{
			pce_psg->freq_lut[i][32] = (int16)floor(
				(pow(2.0, -0.25 * i) * (j / 31.0) * 6553.5) + 0.5);
		}
	
	/* build noise generator level lookup table
	 * noise generator is 17-level linear feedback shift register with taps at
	 * 6(11) and 17(0) */
	for(i = 0; i < 0x1FFFF; i++)
	{
		pce_psg->noise_lut[i] = (noise_lfsr & 1) ? 0x1F : 0;
		/* shift with taps */
		noise_lfsr = (noise_lfsr >> 1) ^ (-(int32)(noise_lfsr & 1) & 0x10020);
	}

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

	//debug("PSG write: addr=$%04X data=$%04X", addr, data);

	switch(addr)
	{
	/* global */
	case 0x0800: /* channel select */
		pce_psg->sel_ch = (data & 7);
		break;
	case 0x0801: /* sound balance (global) */
		pce_psg->balance = data;
		break;
	case 0x0807: /* white noise */
		pce_psg->noise = data;
		break;
	case 0x0808: /* LFO (low frequency oscilator) frequency */
		pce_psg->lfo_freq = data;
		break;
	case 0x0809: /* LFO (low frequency oscilator) control */
		pce_psg->lfo_ctrl = data;
		break;

	/* channel specific */
	case 0x0802: /* frequency LSB */
		PSG_CH.freq = (PSG_CH.freq & 0x0F00) | (data);
		break;
	case 0x0803: /* frequency MSB */
		PSG_CH.freq = (PSG_CH.freq & 0x00FF) | ((data & 0x0F) << 8);
		break;
	case 0x0804: /* channel control (frequency, dda, volume) */
		PSG_CH.ctrl = data;
		/* if needed set wave index to 0 */
		if((data & 0xC0) == 0x40)
			PSG_CH.wav_index = 0;
		break;
	case 0x0805: /* channel balance */
		PSG_CH.balance = data;
		break;
	case 0x0806: /* channel waveform data */
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
void pce_psg_fill(int16 *buf_l, int16 *buf_r, int length)
{
	assert(pce_psg);
	assert(buf_l && buf_r);

	//debug("PSG fill buffers size=%d", length);

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
			/* frequency base (PSG clock / lenght of waveform) */
			int freq_base = (3580000 / 32) * 3; /* *2 just sounds better */
			/* current frequency (avoid division by zero) */
			int freq = (pce_psg->ch[ch].freq != 0)
				? freq_base / pce_psg->ch[ch].freq : 0;

			/* calculate offset (upper 5 bits of counter == data length) */
			int offset = (pce_psg->ch[ch].counter >> 12) & 0x1F;

			pce_psg->ch[ch].counter += freq;

			/* get data and add to samples */
			int data = (pce_psg->ch[ch].wav[offset] & 0x1F);

			sample[0] = (sample[0] + (data * l_vol));
			sample[1] = (sample[1] + (data * r_vol));
		}

		if(sample[0] & 0x8000)
			sample[0] ^= 0x8000;

		if(sample[1] & 0x8000)
			sample[1] ^= 0x8000;

		//debug("PSG: sample_l=%d, sample_r=%d", sample[0], sample[1]);

		*buf_l++ = sample[0];
		*buf_r++ = sample[1];

		--length;
	}
}
