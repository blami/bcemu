/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* vce_huc6260.c: NEC PCEngine HuC6260 VCE emulator */

#include "bc_emu.h"
#include "emu/pce/pce_main.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/psg.h"
#include "emu/pce/rom.h"


t_pce_vce* pce_vce = NULL;

/* -------------------------------------------------------------------------- *
 * Init, reset, shutdown routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize VCE (video color encoder).
 */
int pce_vce_init()
{
	int i, j, x;
	debug("VCE init");

	pce_vce = xmalloc(sizeof(t_pce_vce));

	/* prepare lookup tables for bitplanes and pixel lookups */

	/* build bitplane to pixel lookup table */
	for(i = 0; i < 256; i++)
		for(j = 0; j < 256; j++)
		{
			uint32 val = 0;
			for(x = 0; x < 8; x++)
			{
				val |= (j & (0x80 >> x)) ?
					(uint32)(8 << (x << 2)) : 0;
				val |= (i & (0x80 >> x)) ?
					(uint32)(4 << (x << 2)) : 0;
			}
#ifdef LSB
			pce_vce->bp_lut[(j << 8) | i] = val;
#else
			pce_vce->bp_lut[(i << 8) | j] = val;
#endif /* LSB */
		}

	/* build VCE to pixel lookup table (512 colors) */
	for(i = 0; i < 512; i++)
	{
		int r = (i >> 3) & 7;
		int g = (i >> 6) & 7;
		int b = (i >> 0) & 7;
		pce_vce->pixel_lut[i] = (r << 13 | g << 8 | b << 2) & 0xE71C;
	}

	memset(pce_vce->data, 0, 0x400);
	return 1;
}

/**
 * Reset VCE (video color encoder).
 */
void pce_vce_reset()
{
	debug("VCE reset");
	assert(pce_vce);
}

/**
 * Shutdown VCE (video color encoder).
 */
void pce_vce_shutdown()
{
	debug("VCE shutdown");
	assert(pce_vce);

	xfree(pce_vce);
}

/* -------------------------------------------------------------------------- *
 * VCE                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Read VCE memory.
 * \param addr          VCE memory address
 * \return              data
 */
uint8 pce_vce_r(int addr)
{
	/* check and store address MSB */
	int msb = (addr & 1);

	debug("VCE read: addr=$%03X msb=%d", addr, msb);

	if((addr & ~1) == 0x404)
	{
		uint8 temp = pce_vce->data[((pce_vce->addr & 0x1FF) << 1) | (msb)];
		/* increment address if MSB */
		if(msb)
			pce_vce->addr += 1;
		return (temp);
	}

	return (0xFF);
}

/**
 * Write VCE memory.
 * \param addr          VCE memory address
 * \param data          data to be written
 */
void pce_vce_w(int addr, int data)
{
	/* check and store address MSB */
	int msb = (addr & 1);

	//debug("VCE write: addr=$%03X data=$%02X msb=%d", addr, data, msb);

	switch(addr & ~1)
	{
	case 0x400: /* CR: control register */
		/* bit 0-1: DCC (0=5.37MHz, 1=7.16MHz, 2,3=10.7MHz
		 * bit 2:   Frame/Field (0=262 lines, 1=263 lines)
		 * bit 7:   Strip colorburst (0=don't strip, 1=strip) */
		if(!msb)
			pce_vce->ctrl = (data & 1);
		break;
	case 0x402 /* 0x403 */: /* CTA: color table address */
		if(!msb)
			pce_vce->addr = (pce_vce->addr & 0x0100) | data;
		else
			pce_vce->addr = (pce_vce->addr & 0x00FF) | ((data & 1) << 8);
		break;
	case 0x404 /* 0x405 */: /* CTD: color table data */
		/* bit 0-2: blue
		 * bit 3-5: red
		 * bit 6-8: green */

		/* pce_vce->addr & 0x1FF is offset */
		if(data != pce_vce->data[((pce_vce->addr & 0x1FF) << 1) | msb])
		{
			/* Some games (Pc Genjin 2) sends invalid data most significant
			 * byte. When writing to $404+$405, data should have maximal length
			 * of 9 bits. */
			if(msb && data > 1)
			{
				/*
				debug("VCE unexpected CTD data write"
					"(MSB data is $%04X, should be $%04X)", data, data & 0x1);
				*/
				data = data & 0x1;
			}

			pce_vce->data[((pce_vce->addr & 0x1FF) << 1) | msb] = data;

			if((pce_vce->addr & 0x0F) != 0x00)
			{
				/* form VCEs pixel_lut index */
				uint16 tmp = *(uint16 *)&pce_vce->data[pce_vce->addr << 1];
#ifndef LSB
				tmp = (tmp >> 8) | (tmp << 8);
#endif /* LSB */

				pce_vce->pixel[(pce_vce->addr >> 8) & 1][(pce_vce->addr & 0xFF)] = pce_vce->pixel_lut[tmp];
			}

			/* update overscan color (color 0 in all palettes which is visible
			 * around TV picture area. This is really not important) */
			if((pce_vce->addr & 0x0F) == 0x00)
			{
				int n;
				/* form VCEs pixel_lut index */
				uint16 tmp = *(uint16 *)&pce_vce->data[0];
#ifndef LSB
				tmp = (tmp >> 8) | (tmp << 8);
#endif /* LSB */

				for(n = 0; n < 16; n ++)
					pce_vce->pixel[0][(n << 4)] = pce_vce->pixel_lut[tmp];
			}
		}

		/* increment address if MSB set */
		if(msb)
			pce_vce->addr++;
		break;
	}
}
