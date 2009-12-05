/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* vdc_huc6270.h: NEC PCEngine HuC6270 VDC emulator */

#include "bc_emu.h"
#include "emu/pce/pce_main.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/psg.h"
#include "emu/pce/rom.h"


t_pce_vdc* pce_vdc = NULL;

/* -------------------------------------------------------------------------- *
 * Init, reset, shutdown routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize VDC (video display controller) and rendering routines.
 */
int pce_vdc_init()
{
	debug("VDC init");

	pce_vdc = xmalloc(sizeof(t_pce_vdc));

	pce_vdc_reset();

	return 0;
}

/**
 * Reset VDC (video display controller).
 */
void pce_vdc_reset()
{
	debug("VDC reset");
	assert(pce && pce_vdc);
	assert(emu_video);

	/* prepare video memory */
	memset(pce->vram, 0, 0x10000);
	/* set default write addresses */
	pce_vdc->vram_write = (uint16*)&pce->vram[0];
	pce_vdc->satb_write = (uint16*)&pce->satb[0];

	/* reset registers */
	memset(pce_vdc->reg, 0, 0x20);
	pce_vdc->status = 0;
	pce_vdc->latch = 0;
	pce_vdc->vram_latch = 0;
	pce_vdc->addr_inc = 1;
	pce_vdc->dvssr = 0;

	pce_vdc->y_offset = 0;
	pce_vdc->byr = 0;
	pce_vdc->planes = -1; /* enable both planes */

	/* reset background pattern cache */
	memset(pce_vdc->bp_cache, 0, sizeof(pce_vdc->bp_cache));
	memset(pce_vdc->bp_list, 0, sizeof(pce_vdc->bp_list));
	pce_vdc->bp_list_i = 0;
	memset(pce_vdc->bp_dirty, 0, sizeof(pce_vdc->bp_dirty));

	/* reset sprite pattern cache */
	memset(pce_vdc->sp_cache, 0, sizeof(pce_vdc->sp_cache));
	memset(pce_vdc->sp_list, 0, sizeof(pce_vdc->sp_list));
	pce_vdc->sp_list_i = 0;
	memset(pce_vdc->sp_dirty, 0, sizeof(pce_vdc->sp_dirty));

	/* VDC rendering/shift buffer */
	pce_vdc->buf_shift = 6;
	pce_vdc->buf_row_mask = 0x1F;
	pce_vdc->buf_col_mask = 0xFF;

	pce_vdc->buf_shift_table[0] = 6;
	pce_vdc->buf_shift_table[1] = 7;
	pce_vdc->buf_shift_table[2] = 8;
	pce_vdc->buf_shift_table[3] = 8;

	pce_vdc->buf_row_mask_table[0] = 0x1F;
	pce_vdc->buf_row_mask_table[1] = 0x3F;
	pce_vdc->buf_row_mask_table[2] = 0x7F;
	pce_vdc->buf_row_mask_table[3] = 0x7F;

	/* setup video interface to NEC PCEngine VDC specs */
	emu_video->width = 1024;
	emu_video->height = 256;
	emu_video->vp.x = 0x20;
	emu_video->vp.y = 0x00;
	emu_video->vp.width = 256;
	emu_video->vp.height = 240;
}

/**
 * Shutdown VDC (video display controller).
 */
void pce_vdc_shutdown()
{
	debug("VDC shutdown");
	assert(pce_vdc);

	xfree(pce_vdc);
}

/* -------------------------------------------------------------------------- *
 * VDC                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Macro to inline code for marking dirty place in background tile cache
 * during writes/DMA.
 * \param addr          dirty address
 */
#define bp_dirty(addr)                                          \
{                                                               \
	int pat_addr = (addr >> 4) & 0x7FF; /* BAT lower 12b */     \
	if(pce_vdc->bp_dirty[pat_addr] == 0)                        \
	{                                                           \
		pce_vdc->bp_list[pce_vdc->bp_list_i] = pat_addr;        \
		pce_vdc->bp_list_i += 1;                                \
	}                                                           \
	pce_vdc->bp_dirty[pat_addr] |= (1 << (addr & 0x07));        \
}

/**
 * Macro to inline code for marking dirty place in sprite character cache during
 * writes/DMA.
 * \param addr          dirty address
 */
#define sp_dirty(addr)                                          \
{                                                               \
	int pat_addr = (addr >> 6) & 0x1FF; /* SATB offset 2 */     \
	if(pce_vdc->sp_dirty[pat_addr] == 0)                        \
	{                                                           \
		pce_vdc->sp_list[pce_vdc->sp_list_i] = pat_addr;        \
		pce_vdc->sp_list_i += 1;                                \
	}                                                           \
	pce_vdc->sp_dirty[pat_addr] |= (1 << (addr & 0x0F));        \
}

/**
 * Write VDC memory at specified offset. In case of reading status flags clear
 * status flags and clear IRQ line.
 * \param offset        offset
 * \return              original status or data at address in first register
 */
int pce_vdc_r(int offset)
{
	assert(pce && pce_vdc);

	/* check and store offset MSB */
	int msb = (offset & 1);

	//debug("VDC read: offset=%08x msb=%d", offset, msb);

	uint8 tmp;

	switch(offset)
	{
		/* read status/latch (IRQ clear) */
		case 0x0000:
			/* store return status */
			tmp = pce_vdc->status;

			/* reset status after read and clear IRQ line */
			pce_vdc->status = 0;
			pce_cpu_set_irq_line(0, CLEAR_LINE);

			return tmp;

		/* read data */
		case 0x0002:
		case 0x0003:
			if(pce_vdc->latch == 0x02)
			{
				/* fetch address */
				uint16 addr = pce_vdc->reg[1] << 1;

				/* read address */
				tmp = (pce->vram[(addr | msb) & 0xFFFF]);

				/* increase address if MSB */
				if(msb)
					pce_vdc->reg[1] += pce_vdc->addr_inc;

				return tmp;
			}
			break;
	}

	return 0xFF;
}

/**
 * Write data into VDC memory at specified offset.
 * \param offset        offset
 * \param data          data to be written
 */
void pce_vdc_w(int offset, int data)
{
	assert(pce && pce_vdc);

	/* check and store offset MSB */
	uint8 msb = (offset & 1);

	//debug("VDC write: offset=%08x msb=%d data=%d", offset, msb, data);

	switch(offset)
	{
		/* write status/latch */
		case 0x0000:
			pce_vdc->latch = (data & 0x1F);
			break;

		/* write data */
		case 0x0002:
		case 0x0003:
			//debug("VDC write: latch=%02x", pce_vdc->latch);
			if(msb)
				pce_vdc->reg[pce_vdc->latch] =
					(pce_vdc->reg[pce_vdc->latch] & 0x00FF) | (data << 8);
			else
				pce_vdc->reg[pce_vdc->latch] =
					(pce_vdc->reg[pce_vdc->latch] & 0xFF00) | data;

			switch(pce_vdc->latch)
			{
				/* VWR: write VRAM at address specified in MAWR */
				case 0x02:
					if(msb)
					{
						uint16 vram_data = (data << 8 | pce_vdc->vram_latch);
						if(vram_data != pce_vdc->vram_write[(pce_vdc->reg[0] & 0x7FFF)])
						{
							pce_vdc->vram_write[(pce_vdc->reg[0] & 0x7FFF)] = vram_data;

							/* mark cache dirty */
							bp_dirty(pce_vdc->reg[0]);
							sp_dirty(pce_vdc->reg[0]);
						}

						/* autoincrement MAWR */
						pce_vdc->reg[0] += pce_vdc->addr_inc;
					}
					else
						pce_vdc->vram_latch = data;
					break;

				/* CR: IW address autoincrement value (1, 32, 64, 128) */
				case 0x05:
					if(msb)
					{
						static uint8 iw_tbl[] = {1, 32, 64, 128};
						pce_vdc->addr_inc = iw_tbl[(data >> 3) & 3];
					}
					break;

				/* BYR: viewport Y-offset from the background map origin */
				case 0x08:
					pce_vdc->y_offset = pce_vdc->byr = (pce_vdc->reg[0x08] & 0x1FF);
					pce_vdc->y_offset &= pce_vdc->buf_col_mask;
					break;

				/* MWR: configures virtual background size */
				case 0x09:
					if(!msb)
					{
						pce_vdc->buf_shift = pce_vdc->buf_shift_table[(data >> 4) & 3];
						pce_vdc->buf_row_mask = pce_vdc->buf_row_mask_table[(data >> 4) & 3];
						pce_vdc->buf_col_mask = ((data >> 6) & 1) ? 0x01FF : 0x00FF;
					}
					break;

				/* HDR: controls horizontal display width + end position */
				case 0x0B:
					pce_vdc->disp_width = (1 + (pce_vdc->reg[0x0B] & 0x3F)) << 3;

					if(pce_vdc->disp_width != pce_vdc->disp_width_old)
					{
						debug("VDC viewport changed: w=%dpx", pce_vdc->disp_width);
						// FIXME
						//bitmap.viewport.ow = bitmap.viewport.w;
						//bitmap.viewport.w = old_width = disp_width;
						//bitmap.viewport.changed = 1;
					}
					break;

				/* VDW: controls vertical display width (NOT end position) */
				case 0x0D:
					pce_vdc->disp_height = 1 + (pce_vdc->reg[0x0D] & 0x01FF);

					if(pce_vdc->disp_height != pce_vdc->disp_height_old)
					{
						debug("VDC viewport changed: h=%dpx", pce_vdc->disp_height);
						// FIXME
						//bitmap.viewport.oh = bitmap.viewport.h;
						//bitmap.viewport.h = old_height = disp_height;
						//bitmap.viewport.changed = 1;
					}
					break;

				/* LENR: DMA block length */
				case 0x12:
					if(msb)
						pce_vdc_dma();
					break;

				/* SATB: points to SATB, we need to trigger VRAM to SATB DMA */
				case 0x13:
					if(msb)
						pce_vdc->dvssr = 1;
					break;
			}
	}
}

/**
 * Process VDC DMA request. Read source, destination address, block length and
 * move data. This handles only VRAM to VRAM operations.
 */
void pce_vdc_dma()
{
	assert(pce_cpu && pce_vdc);

	/* fetch DMA transfer control data from DCR (0x0F):
	 * b  1:    enable interrupt after VRAM to VRAM DMA completion
	 * b  2:    0:increment sour address, 1:decrement sour address
	 * b  3:    0:increment desr address, 1:decrement desr address */
	int desr_inc = (pce_vdc->reg[0x0F] >> 3) & 1;
	int sour_inc = (pce_vdc->reg[0x0F] >> 2) & 1;
	int irq = (pce_vdc->reg[0x0F] >> 1) & 1;

	/* fetch address registers */
	int sour = (pce_vdc->reg[0x10] & 0x7FFF);
	int desr = (pce_vdc->reg[0x11] & 0x7FFF);
	int lenr = (pce_vdc->reg[0x12] & 0x7FFF);

	debug("VDC DMA VRAM src:%04X %c dst: %04X %c len: %04X",
		sour, sour_inc ? '-' : '+',
		desr, desr_inc ? '-' : '+',
		lenr);

	/* transfer data */
	do
	{
		uint16 tmp = pce_vdc->vram_write[(sour & 0x7FFF)];
		if(tmp != pce_vdc->vram_write[(desr & 0x7FFF)])
		{
			pce_vdc->vram_write[(desr & 0x7FFF)] = tmp;
			bp_dirty(desr);
			sp_dirty(desr);
		}
		sour = (sour_inc) ? (sour - 1) : (sour + 1);
		desr = (desr_inc) ? (desr - 1) : (desr + 1);
	}
	while (lenr--);

	/* update status flag register */
	pce_vdc->status |= VDC_DV;

	/* cause IRQ if enabled in DCR */
	if(irq)
		pce_cpu_set_irq_line(0, ASSERT_LINE);
}

/* -------------------------------------------------------------------------- *
 * VDC bitmap rendering                                                       *
 * -------------------------------------------------------------------------- */

/**
 * Pre-calculate sprite list of sprites used in frame.
 * \return              sprite list index
 */
int pce_vdc_sprite_update()
{
	assert(pce && pce_vdc);

	uint16 *satb = (uint16*)&pce->satb[0];
	/* FIXME: change pat_addr to uint16 */
	int pat_addr;   /* SATB offset 2 */
	int attr;       /* SATB offset 3 sprite flags:
		b 0-3       sprite color indexes (which palettes to use)
		b 4-6       ??
		b 7         SPBG
		            0 = in front of BG
		            1 = behind BG
		b 8         CGX cell width:
		            0 = 1 cell (16px)
		            1 = 2 cells (32px)
		b 9-A       ??
		b B         X-invert
		b C-D       CGY cell height
		            00 = 1 cell (16px)
		            01 = 2 cells (32px)
		            10 = ??
		            11 = 4 cells (64px)
		b E         ??
		b F         Y-invert
		*/
	int xpos, ypos;
	int cgx, cgy;
	int xflip, yflip;
	int width, height;
	int spbg;
	int i;
	uint32 flip;    /* combination of yflip and xflip */

	/* prepare list memory */
	pce_vdc->sprite_list_i = 0;
	memset(&pce_vdc->sprite_list, 0, sizeof(pce_vdc->sprite_list));
	memset(&pce_vdc->sprite, 0, sizeof(pce_vdc->sprite));

	/* fill list */
	for(i = 0; i < 0x40; i++)
	{
		/* read attributes from SATB */
		ypos = satb[(i << 2) | 0];
		xpos = satb[(i << 2) | 1];
		pat_addr = satb[(i << 2) | 2];
		attr = satb[(i << 2) | 3];

		/* correct x and y position */
		ypos &= 0x3FF;
		xpos &= 0x3FF;

		if(xpos && ypos)
		{
			ypos -= 64;
			xpos -= 32;

			if(ypos >= 0x100)
				continue;
			if(xpos >= 0x200)
				continue;

			/* check CGY attr */
			cgy = (attr >> 12) & 3;
			cgy |= (cgy >> 1);

			/* calculate sprite height */
			height = (cgy + 1) << 4;
			if((ypos + height) < 0)
				continue;

			/* check CGX attr */
			cgx = (attr >> 8) & 1;

			/* calculate sprite width */
			width = (cgx) ? 32 : 16;
			if((xpos + width) < 0)
				continue;

			/* check X,Y-flip attr and store both to flip */
			xflip = (attr >> 11) & 1;
			yflip = (attr >> 15) & 1;
			flip = ((xflip << 9) | (yflip << 10)) & 0x600;

			/* check SPBG */
			spbg = !(attr & 0x80);

			/* calculate pattern address */
			pat_addr = (pat_addr >> 1) & 0x1FF;
			/* apply transformations (CGY, CGX, X-flip and Y-flip) */
			pat_addr &= ~((cgy << 1) | cgx);
			pat_addr |= flip;

			if(xflip && cgx)
				pat_addr ^= 1;

			/* store sprite to sprite list */
			pce_vdc->sprite[i].top = ypos;
			pce_vdc->sprite[i].bottom = ypos + height;
			pce_vdc->sprite[i].xpos = xpos;
			pce_vdc->sprite[i].pat_addr_l = pat_addr;
			pce_vdc->sprite[i].pat_addr_r = pat_addr ^ 1;
			pce_vdc->sprite[i].height = (height - 1);
			pce_vdc->sprite[i].palette = (attr & 0x0F) << 4;

			/* set flags */
			if(yflip)
				pce_vdc->sprite[i].attr |= SP_YFLIP;
			if(cgx)
				pce_vdc->sprite[i].attr |= SP_CGX;
			if(spbg)
				pce_vdc->sprite[i].attr |= SP_SPBG;

			/* add entry to used sprite list and increase index */
			pce_vdc->sprite_list[pce_vdc->sprite_list_i] = i;
			pce_vdc->sprite_list_i++;
		}
	}

	/* return used sprite list last index */
	return pce_vdc->sprite_list_i;
}

/**
 * Update background tile cache.
 */
static void pce_vdc_cache_bp()
{
	int i;
	uint16 pat_addr;    /* BAT lower 12b */
	uint8 x, y;
	uint16 i1, i2;
	uint8 c;
	uint32 tmp;

	assert(pce_vce);

	if(!pce_vdc->bp_list_i)
		return;

	for(i = 0; i < pce_vdc->bp_list_i; i++)
	{
		/* lookup pat_addr in list */
		pat_addr = pce_vdc->bp_list[i];
		pce_vdc->bp_list[i] = 0;

		for(y = 0; y < 8; y++)
			if(pce_vdc->bp_dirty[pat_addr] & (1 << y))
			{
				/* color index (planar) */
				i1 = pce_vdc->vram_write[(pat_addr << 4) | y];
				i2 = pce_vdc->vram_write[(pat_addr << 4) | y | 8];

				/* VCE bitplane table lookup */
				tmp = (pce_vce->bp_lut[i1] >> 2) | pce_vce->bp_lut[i2];

				/* store background tile into cache */
				for(x = 0; x < 8; x++)
				{
					/* color index */
					c = (tmp >> (x << 2)) & 0x0F;
					pce_vdc->bp_cache[(pat_addr << 6) | (y << 3) | x] = c;
				}
			}

		pce_vdc->bp_dirty[pat_addr] = 0;
	}

	pce_vdc->bp_list_i = 0;
}

/**
 * Update sprite character cache.
 */
static void pce_vdc_cache_sp()
{
	int i;
	uint16 pat_addr;    /* SATB offset 2 */
	uint8 x, y, c;
	uint8 i0, i1, i2, i3;
	uint16 b0, b1, b2, b3;

	if(!pce_vdc->sp_list_i)
		return;

	for(i = 0; i < pce_vdc->sp_list_i; i++)
	{
		/* lookup pat_addr in list */
		pat_addr = pce_vdc->sp_list[i];
		pce_vdc->sp_list[i] = 0;

		for(y = 0; y < 0x10; y++)
			if(pce_vdc->sp_dirty[pat_addr] & (1 << y))
			{
				/* sprite character bytes (planar) */
				b0 = pce_vdc->vram_write[(pat_addr << 6) + y + 0x00];
				b1 = pce_vdc->vram_write[(pat_addr << 6) + y + 0x10];
				b2 = pce_vdc->vram_write[(pat_addr << 6) + y + 0x20];
				b3 = pce_vdc->vram_write[(pat_addr << 6) + y + 0x30];

				/* store sprite character into cache */
				for(x = 0; x < 0x10; x += 1)
				{
					/* color index (planar) */
					i0 = (b0 >> (x ^ 0x0F)) & 1;
					i1 = (b1 >> (x ^ 0x0F)) & 1;
					i2 = (b2 >> (x ^ 0x0F)) & 1;
					i3 = (b3 >> (x ^ 0x0F)) & 1;

					/* color */
					c = (i3 << 3 | i2 << 2 | i1 << 1 | i0 << 0);

					pce_vdc->sp_cache[(pat_addr << 8)
						| (y << 4) | x] = c;
					pce_vdc->sp_cache[0x20000 | (pat_addr << 8)
						| (y << 4) | (x ^ 0x0F)] = c;
					pce_vdc->sp_cache[0x40000 | (pat_addr << 8)
						| ((y ^ 0x0F) << 4) | x] = c;
					pce_vdc->sp_cache[0x60000 | (pat_addr << 8)
						| ((y ^ 0x0F) << 4) | (x ^ 0x0F)] = c;
				}
			}

		pce_vdc->sp_dirty[pat_addr] = 0;
	}

	pce_vdc->sp_list_i = 0;
}

/**
 * Render background tile pattern in line.
 */
static void pce_vdc_render_bp(int line, t_video* video)
{
	uint16 *nt;
	uint8 *src, palette;
	uint16 *dst;
	int column, pat_addr, attr, x, shift, v_line, nt_scroll;
	int xscroll = (pce_vdc->reg[7] & 0x03FF);
	int end = pce_vdc->disp_width >> 3;

	int bpp = 16;

	int Bpp = bpp >> 3;                 /* depth in bytes */
	int Bwidth = video->width * Bpp;    /* width in bytes (pitch) */

	/* offset in pattern, in lines */
	v_line = (pce_vdc->y_offset & 7);

	/* offset in name table, in columns */
	nt_scroll = (xscroll >> 3);

	/* offset in column, in pixels */
	shift = (xscroll & 7);

	/* draw an extra tile for the last column */
	if(shift)
		end++;

	/* point to current offset within name table */
	nt = (uint16 *)&pce->vram[
		(pce_vdc->y_offset >> 3) << pce_vdc->buf_shift];

	/* point to start in line buffer */
	dst = (uint16 *)&video->pixeldata[
		(line * Bwidth) + ((0x20 + (0 - shift)) << 1)];

	/* draw columns */
	for(column = 0; column < end; column++)
	{
		/* get attribute */
		attr = nt[(column + nt_scroll) & pce_vdc->buf_row_mask];

		/* extract name and palette bits */
		pat_addr = (attr & 0x07FF);
		palette = (attr >> 8) & 0xF0;

		/* point to current pattern line */
		src = &pce_vdc->bp_cache[(pat_addr << 6) + (v_line << 3)];

		/* draw column */
		for(x = 0; x < 8; x++)
		{
			dst[(column << 3) | (x)] =
				pce_vce->pixel[0][(src[x] | palette)];
		}
	}
}

/**
 * Render sprite pattern in specific line.
 */
static void pce_vdc_render_sp(int line, t_video* video)
{
	t_pce_vdc_sprite* sp;
	int i, j;
	int x;
	int c;
	int pat_addr, pat_addr_mask;
	int v_line;
	uint8 *src;
	int nt_line;
	uint16 *dst;

	int bpp = 16;

	int Bpp = bpp >> 3;                 /* depth in bytes */
	int Bwidth = video->width * Bpp;    /* width in bytes (pitch) */

	for(j = (pce_vdc->sprite_list_i - 1); j >= 0; j -= 1)
	{
		/* fetch sprite pattern */
		i = pce_vdc->sprite_list[j];
		sp = &pce_vdc->sprite[i];

		if((line >= sp->top) && (line < sp->bottom))
		{
			v_line = (line - sp->top) & sp->height;
			nt_line = v_line;

			if(sp->attr & SP_YFLIP)
				nt_line = (sp->height - nt_line);

			pat_addr_mask = ((nt_line >> 4) & 3) << 1;
			pat_addr = (sp->pat_addr_l | pat_addr_mask);
			v_line &= 0x0F;

			src = &pce_vdc->sp_cache[(pat_addr << 8) | ((v_line & 0x0F) << 4)];
			dst = (uint16 *)&video->pixeldata[(line * Bwidth)
				+ (((0x20 + sp->xpos) & 0x1FF) * (Bpp))];

			for(x = 0; x < 0x10; x += 1)
			{
				c = src[x];
				if(c)
					dst[x] = pce_vce->pixel[1][(c | sp->palette)];
			}

			if(sp->attr & SP_CGX)
			{
				pat_addr = (sp->pat_addr_r | pat_addr_mask);
				src = &pce_vdc->sp_cache[(pat_addr << 8) | ((v_line & 0x0f) << 4)];
				dst += 0x10;

				for(x = 0; x < 0x10; x += 1)
				{
					c = src[x];
					if(c)
						dst[x] = pce_vce->pixel[1][(c | sp->palette)];
				}
			}
		}
	}
}


/**
 * Render one line to bitmap.
 * \param line          line to be rendered
 * \param video         video structure holding display data readable by UI
 */
void pce_vdc_render_line(int line, t_video* video)
{
	//debug("VDC render line=%d", line);

	/* render background plane line (if enabled)
	 * VDC CR (0x05) bit 7 = bg enable/disable */
	if((pce_vdc->reg[0x05] & 0x80) && (pce_vdc->planes & 1))
	{
		pce_vdc_cache_bp();
		pce_vdc_render_bp(line, video);
	}
	/* if disabled fill bg with black color */
	else
	{
		int i;
		int bpp = 16;

		int Bpp = bpp >> 3;                 /* depth in bytes */
		int Bwidth = video->width * Bpp;    /* width in bytes (pitch) */

		/* set pointer to viewport begin */
		uint16* ptr = (uint16*)&video->pixeldata[(line * Bwidth) + (video->vp.x * Bpp)];

		/* fill viewport with black pixels */
		for(i = 0; i < pce_vdc->disp_width; i++)
			ptr[i] = pce_vce->pixel[0][0];
	}

	/* render sprite plane line (if enabled)
	 * VDC CR (0x05) bit 6 = sprite enable/disable */
	if((pce_vdc->reg[0x05] & 0x40) && (pce_vdc->planes & 2))
	{
		pce_vdc_cache_sp();
		pce_vdc_render_sp(line, video);
	}
}
