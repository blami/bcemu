/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pce.c: NEC PCEngine emulator. */

#include "bc_emu.h"
#include "emu/pce/pce_main.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/psg.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/rom.h"


t_pce* pce = NULL;

/* -------------------------------------------------------------------------- *
 * Init, shutdown, reset routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize PCE emulator.
 */
int pce_init()
{
	debug("PCE init");

	pce = xmalloc(sizeof(t_pce));
	memset(pce, 0, sizeof(t_pce));

	/* initialize emulator subsystems */
	pce_rom_parse();
	pce_cpu_init();
	pce_vce_init();
	pce_vdc_init();
	pce_psg_init();

	return 1;
}

/**
 * Shutdown PCE emulator.
 */
void pce_shutdown()
{
	debug("PCE shutdown");
	assert(pce && pce_cpu && pce_vce && pce_vdc && pce_psg);

	/* shutdown emulator subsystems */
	pce_psg_shutdown();
	pce_vdc_shutdown();
	pce_vce_shutdown();
	pce_cpu_shutdown();

	/* clean pce */
	xfree(pce);
}

/**
 * Reset PCE emulator.
 */
void pce_reset()
{
	debug("PCE reset");
	assert(pce && pce_cpu && pce_vce && pce_vdc && pce_psg);

	pce_cpu_reset();
	pce_vce_reset();
	pce_vdc_reset();
	pce_psg_reset();
}

/* -------------------------------------------------------------------------- *
 * PCE                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Run emulation for one frame.
 */
void pce_frame()
{
	int line;

	//debug("PCE frame");
	assert(pce && pce_cpu && pce_vce && pce_vdc && pce_psg);

	/* render 262 lines per frame (NTSC) */
	for(pce_vdc->y_offset = pce_vdc->byr, line = 0; line < 262; line++)
	{
		/* VDC R (raster counter) register equals line + 64 */
		if((line + 64) == (pce_vdc->reg[0x06] & 0x3FF))
			if(pce_vdc->reg[0x05] && 0x04)
			{
				pce_vdc->status |= VDC_RR;
				pce_cpu_set_irq_line(0, ASSERT_LINE);
			}

		/* vertical blanking */
		if(line == 240)
		{
			/* DVSRR or DCR is set, DMA write */
			if(pce_vdc->dvssr || pce_vdc->reg[0x0F] & 0x10)
			{
				pce_vdc->dvssr = 0;

				/* DMA transfer VRAM to SATB */
				memcpy(pce->satb, &pce->vram[(pce_vdc->reg[0x13] << 1) & 0xFFFE],
					0x200);

				/* DMA transfer done IRQ */
				if(pce_vdc->reg[0x0F] & 0x01)
				{
					pce_vdc->status |= VDC_DS;
					pce_cpu_set_irq_line(0, ASSERT_LINE);
				}

				/* prepare sprites for next frame */
				pce_vdc_sprite_update();
			}

			/* cause vertical blank interrupt */
			if(pce_vdc->reg[0x05] & 0x0008)
			{
				pce_vdc->status |= VDC_VD;
				pce_cpu_set_irq_line(0, ASSERT_LINE);
			}
		}

		/* at 7.16MHz we have 455 cycles per line window */
		pce_cpu_exec(455);

		if(line < pce_vdc->disp_height)
			pce_vdc_render_line(line, emu_video);

		pce_vdc->y_offset = (pce_vdc->y_offset + 1) & pce_vdc->buf_col_mask;
	}
}
