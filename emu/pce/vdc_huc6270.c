/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* vdc_huc6270.h: NEC PCEngine HuC6270 VDC emulator */

#include "bc_emu.h"
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
 * Initialize VDC (video display controller).
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

	/* prepare video memory */
	memset(pce->vram, 0, 0x10000);

	/* reset registers */
	memset(pce_vdc->reg, 0, 0x20);
	status = 0;
	latch = 0;
	vram_latch = 0;
	addr_inc = 1;
	dvssr = 0;

	/* reset background pattern cache */
	memset(pce_vdc->bg_cache, 0, sizeof(pce_vdc->bg_cache));
	memset(pce_vdc->bg_list, 0, sizeof(pce_vdc->bg_list));
	pce_vdc->bg_list_i = 0;
	memset(pce_vdc->bg_dirty, 0, sizeof(pce_vdc->bg_dirty));

	/* reset sprite pattern cache */
	memset(pce_vdc->sprite_cache, 0, sizeof(pce_vdc->sprite_cache));
	memset(pce_vdc->sprite_list, 0, sizeof(pce_vdc->sprite_list));
	sprite_list_i = 0;
	memset(pce_vdc->sprite_dirty, 0, sizeof(pce_vdc->sprite_dirty));

/*
    playfield_shift = 6;
    playfield_row_mask = 0x1f;
    playfield_col_mask = 0xff;

*/
	/* pre-calculate bitplane to pixel lookup table */
	int i, j, x;
	for(i = 0; i < 0x100; i++)
		for(j = 0; j < 0x100; j++)
		{
			uint32 val = 0;
			for(x = 0; x < 8; x++)
			{
				val |= (j & (0x80 >> x)) ?
					(uint32)(8 << (x << 2)) : 0;
				val |= (i & (0x80 >> x)) ?
					(uint32)(8 << (x << 2)) : 0;
			}
#ifdef LSB
			pce_vdc->bp_lut[(j << 8) | i] = val;
#else
			pce_vdc->bp_lut[(i << 8) | j] = val;
#endif /* LSB */
		}

	/* pre-calculate VCE to pixel lookup table */
	for(i = 0; i < 0x200; i++)
	{
		int r = (i >> 3) & 7;
		int g = (i >> 6) & 7;
		int b = (i >> 0) & 7;
		pce_vdc->pixel_lut[i] = (r << 13 | g << 8 | b << 2) & 0xE71C;
	}

	return 1;
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
 * Macro to inline code for marking dirty place in background pattern cache
 * during writes/DMA.
 * \param addr          dirty address
 */
#define MARK_BG_DIRTY(addr)                                     \
{                                                               \
	int id = (addr >> 4) & 0x7FF;                               \
	if(pce_vdc->bg_dirty[id] == 0)                              \
	{                                                           \
		pce_vdc->bg_list[pce_vdc->bg_list_i] = id;              \
		pce_vdc->bg_list_i += 1;                                \
	}                                                           \
	pce_vdc->bg_dirty[id] |= (1 << (addr & 0x07));              \
}

/**
 * Macro to inline code for marking dirty place in sprite pattern cache during
 * writes/DMA.
 * \param addr          dirty address
 */
#define MARK_SP_DIRTY(addr)                                     \
{                                                               \
	int id = (addr >> 6) & 0x1FF;                               \
	if(pce_vdc->sprite_dirty[id] == 0)                          \
	{                                                           \
		pce_vdc->sp_list[pce_vdc->sp_list_i] = id;              \
		pce_vdc->sp_list += 1;                                  \
	}                                                           \
	pce_vdc->sp_dirty[id] |= (1 << (addr & 0x0F));              \
}

/**
 * Write VDC memory at specified offset. In case of reading status flags clear
 * status flags and clear IRQ line.
 * \param offset        offset
 * \return              original status or data at address in first register
 */
int vdc_r(int offset)
{
	assert(pce & pce_vdc);

	/* check and store offset MSB */
	int msb = (offset & 1);

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
				tmp = (vram[(addr | msb) & 0xFFFF]);

				/* increase address if MSB */
				if(msb)
					pce_vdc->reg[1] += addr_inc;

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
void vdc_w(int offset, int data)
{
	assert(pce && pce_vdc);

	/* check and store offset MSB */
	uint8 msb = (offset & 1);

	switch(offset)
	{
		/* write status/latch */
		case 0x0000:
			pce_vdc->latch = (data & 0x1F);
			break;

		/* write data */
		case 0x0002:
		case 0x0003:
			uint8 latch = pce_vdc->latch;

			if(msb)
				pce_vdc->reg[latch] = (pce_vdc->reg[latch] & 0x00FF) | (data << 8);
			else
				pce_vdc->reg[latch] = (pce_vdc->reg[latch] & 0xFF00) | (data);

			switch(latch)
			{
				/* VWR: write VRAM at address specified in MAWR */
				case 0x02:
					if(msb)
					{
						uint16 vram_data = (data << 8 | pce_vdc->vram_latch);
						if(vram_data != vram_write[(reg[0] & 0x7FFF)])
						{
							pce_vdc->vram_write[(reg[0] & 0x7FFF)] = vram_data;
							MARK_BG_DIRTY(reg[0]);
							MARK_SP_DIRTY(reg[0]);
						}

						/* autoincrement MAWR */
						reg[0] += addr_inc;
					}
					else
						vram_latch = data;
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
					y_offset = byr = (reg[0x08] & 0x1FF);
					y_offset &= playfield_col_mask;
					break;

				/* MWR: configures virtual background size */
				case 0x09:
					if(!msb)
					{
						playfield_shift = playfield_shift_table[(data >> 4) & 3];
						playfield_row_mask = playfield_row_mask_table[(data >> 4) & 3];
						playfield_col_mask = ((data >> 6) & 1) ? 0x01FF : 0x00FF;
					}
					break;

				/* HDR: controls horizontal display width + end position */
				case 0x0B:
					pce_vdc->disp_width = (1 + (pce_vdc->reg[0x0B] & 0x3F)) << 3;

					if(pce_vdc->disp_width != pce_vdc->disp_width_old)
					{
						debug("VDC viewport changed: w=%dpx", pce_vdc->disp_width);
						//bitmap.viewport.ow = bitmap.viewport.w;
						//bitmap.viewport.w = old_width = disp_width;
						//bitmap.viewport.changed = 1;
					}
					break;

				/* VDW: controls vertical display width (NOT end position) */
				case 0x0D:
					pce_vdc->disp_height = 1 + (reg[0x0D] & 0x01FF);

					if(pce_vdc->disp_height != pce_vdc->disp_height_old)
					{
						debug("VDC viewport changed: h=%dpx", pce_vdc->disp_height);
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
						dvssr = 1;
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
	int desr_inc = (reg[0x0F] >> 3) & 1;
	int sour_inc = (reg[0x0F] >> 2) & 1;
	int irq = (reg[0x0F] >> 1) & 1;

	/* fetch address registers */
	int sour = (reg[0x10] & 0x7FFF);
	int desr = (reg[0x11] & 0x7FFF);
	int lenr = (reg[0x12] & 0x7FFF);

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
			vram->write[(desr & 0x7FFF)] = tmp;
			pce_vdc_bg_dirty(desr);
			pce_vdc_sprite_dirty(desr);
		}
		sour = (sour_inc) ? (sour - 1) : (sour + 1);
		desr = (desr_inc) ? (desr - 1) : (desr + 1);
	}
	while (lenr--);

	/* update status flag register */
	pce_vdc->status |= STATUS_DV;

	/* cause IRQ if enabled in DCR */
	if(irq)
		pce_cpu_set_irq_line(0, ASSERT_LINE);
}

/* -------------------------------------------------------------------------- *
 * VDC bitmap rendering                                                       *
 * -------------------------------------------------------------------------- */

/**
 * Update background pattern cache. Iterate through bg_list and find dirty
 * entries and renew them according to VRAM/bitplane content.
 */
static void pce_vdc_bg_cache()
{
	int i;
	uint16 id;
	uint8 x, y, c;
	uint16 index1, index2;
	uint32 tmp;

	if(!pce_vdc->bg_list_i)
		return;

	for(i = 0; i < pce_vdc->bg_list_i; i += 1)
	{
		/* lookup pattern id in list */
		id = pce_vdc->bg_list[i];
		pce_vdc->bg_list[i] = 0;

		for(y = 0; y < 8; y += 1)
			if(pce_vdc->bg_dirty[id] & (1 << y))
			{
				index1 = pce_vdc->vram_write[(id << 4) | y];
				index2 = pce_vdc->vram_write[(id << 4) | y | (8)];

				tmp = (bp_lut[index1] >> 2) | bp_lut[index2];

				for(x = 0; x < 8; x += 1)
				{
					c = (tmp >> (x << 2)) & 0x0F;
					pce_vdc->bg_cache[(id << 6) | (y << 3) | (x)] = c;
				}
			}

		pce_vdc->bg_dirty[id] = 0;
	}

	pce_vdc->bg_list_i = 0;
}

/**
 * Update sprite pattern cache.
 */
static void pce_vdc_sprite_cache()
{
	int i;
	uint16 id;
	uint8 x, y, c;
	uint8 i0, i1, i2, i3;
	uint16 b0, b1, b2, b3;

	if(!pce_vdc->sp_list_i)
		return;

	for(i = 0; i < pce_vdc->sp_list_i; i += 1)
	{
		/* lookup pattern id in list */
		id = pce_vdc->sp_list[i];
		pce_vdc->sp_list[i] = 0;

		for(y = 0; y < 0x10; y += 1)
			if(pce_vdc->sp_dirty[id] & (1 << y))
			{
				b0 = pce_vdc->vram_write[(id << 6) + y + (0x00)];
				b1 = pce_vdc->vram_write[(id << 6) + y + (0x10)];
				b2 = pce_vdc->vram_write[(id << 6) + y + (0x20)];
				b3 = pce_vdc->vram_write[(id << 6) + y + (0x30)];

				for(x = 0; x < 0x10; x += 1)
				{
					i0 = (b0 >> (x ^ 0x0F)) & 1;
					i1 = (b1 >> (x ^ 0x0F)) & 1;
					i2 = (b2 >> (x ^ 0x0F)) & 1;
					i3 = (b3 >> (x ^ 0x0F)) & 1;

					c = (i3 << 3 | i2 << 2 | i1 << 1 | i0);

					pce_vdc->sp_cache[(id << 8)
						| (y << 4) | (x)] = c;
					pce_vdc->sp_cache[0x20000 | (id << 8)
						| (y << 4) | (x ^ 0x0F)] = c;
					pce_vdc->sp_cache[0x40000 | (id << 8)
						| ((y ^ 0x0F) << 4) | (x)] = c;
					pce_vdc->sp_cache[0x60000 | (id << 8)
						| ((y ^ 0x0F) << 4) | (x ^ 0x0F)] = c;
				}
			}

		pce_vdc->sp_dirty[id] = 0;
	}

	pce_vdc->sp_list_i = 0;
}

/**
 * Render bg plane.
 */
void render_bg_16(int line, t_video* video)
{
    uint16 *nt;
    uint8 *src, palette;
    uint16 *dst;
    int column, name, attr, x, shift, v_line, nt_scroll;
    int xscroll = (reg[7] & 0x03FF);
    int end = disp_nt_width;

    /* Offset in pattern, in lines */
    v_line = (y_offset & 7);

    /* Offset in name table, in columns */
    nt_scroll = (xscroll >> 3);

    /* Offset in column, in pixels */
    shift = (xscroll & 7);

    /* Draw an extra tile for the last column */
    if(shift) end += 1;

    /* Point to current offset within name table */
    nt = (uint16 *)&vram[(y_offset >> 3) << playfield_shift];

    /* Point to start in line buffer */
    dst = (uint16 *)&bitmap.data[(line * bitmap.pitch) + ((0x20 + (0 - shift)) << 1)];

    /* Draw columns */
    for(column = 0; column < end; column += 1)
    {
        /* Get attribute */
        attr = nt[(column + nt_scroll) & playfield_row_mask];

        /* Extract name and palette bits */
        name = (attr & 0x07FF);
        palette = (attr >> 8) & 0xF0;

        /* Point to current pattern line */
        src = &bg_pattern_cache[(name << 6) + (v_line << 3)];

        /* Draw column */
        for(x = 0; x < 8; x += 1)
        {
            dst[(column << 3) | (x)] = pixel[0][(src[x] | palette)];
        }
    }
}

void render_obj_16(int line)
{
    t_sprite *p;
    int j, i, x, c;
    int name, name_mask;
    int v_line;
    uint8 *src;
    int nt_line;
    uint16 *dst;

    for(j = (used_sprite_index - 1); j >= 0; j -= 1)
    {
        i = used_sprite_list[j];
        p = &sprite_list[i];

        if( (line >= p->top) && (line < p->bottom))
        {
            v_line = (line - p->top) & p->height;
            nt_line = v_line;
            if(p->flags & FLAG_YFLIP) nt_line = (p->height - nt_line);
            name_mask = ((nt_line >> 4) & 3) << 1;
            name = (p->name_left | name_mask);
            v_line &= 0x0F;

            src = &obj_pattern_cache[(name << 8) | ((v_line & 0x0f) << 4)];
            dst = (uint16 *)&bitmap.data[(line * bitmap.pitch) + (((0x20+p->xpos) & 0x1ff) * (bitmap.granularity))];

            for(x = 0; x < 0x10; x += 1)
            {
                c = src[x];
                if(c) dst[x] = pixel[1][((c) | p->palette)];
            }

            if(p->flags & FLAG_CGX)
            {
                name = (p->name_right | name_mask);
                src = &obj_pattern_cache[(name << 8) | ((v_line & 0x0f) << 4)];
                dst += 0x10;

                for(x = 0; x < 0x10; x += 1)
                {
                    c = src[x];
                    if(c) dst[x] = pixel[1][((c) | p->palette)];
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
	/* render background plane line (if enabled)
	 * VDC CR (0x05) bit 7 = bg enable/disable */
	if((reg[0x05] & 0x80) && (plane_enable & 1))
	{
		pce_vdc_bg_cache();
		pce_vdc_render_bg(line, video);
	}
	/* if disabled fill bg with black color */
	else
	{
		int i;
		int Bpp = video.bpp >> 3;           /* depth in bytes */
		int Bwidth = video.width * Bpp;     /* width in bytes (pitch) */

		/* set pointer to viewport begin */
		uint16* ptr = (uint16*)video->pixeldata[
			(line * Bwidth) + (video.vp.x * Bpp)];
		/* fill viewport with black pixels */
		for(i = 0; i < pce_vdc->disp_width; i++)
			ptr[i] = pixel[0][0];
	}

	/* render sprite plane line (if enabled)
	 * VDC CR (0x05) bit 6 = sprite enable/disable */
	if((reg[0x05] & 0x40) && (plane_enable & 2))
	{
		pce_vdc_sp_cache();
		pce_vdc_render_sp(line, video);
	}
}


int make_sprite_list(void)
{
    uint16 *sat = &objramw[0];
    int xpos, ypos, name, attr;
    int cgx, xflip, cgy, yflip;
    int width, height;
    int i;
    uint32 flip;

    used_sprite_index = 0;
    memset(&used_sprite_list, 0, sizeof(used_sprite_list));

    memset(&sprite_list, 0, sizeof(sprite_list));

    for(i = 0; i < 0x40; i += 1)
    {
        ypos = sat[(i << 2) | (0)];
        xpos = sat[(i << 2) | (1)];
        name = sat[(i << 2) | (2)];
        attr = sat[(i << 2) | (3)];

        ypos &= 0x3FF;
        xpos &= 0x3FF;

        if(xpos && ypos)
        {
            ypos -= 64;
            if(ypos >= 0x100) continue;
            cgy = (attr >> 12) & 3;
            cgy |= (cgy >> 1);
            height = (cgy + 1) << 4;
            if((ypos + height) < 0) continue;

            xpos -= 32;
            if(xpos >= 0x200) continue;
            cgx = (attr >> 8) & 1;
            width  = (cgx) ? 32 : 16;
            if((xpos + width) < 0) continue;

            xflip = (attr >> 11) & 1;
            yflip = (attr >> 15) & 1;
            flip = ((xflip << 9) | (yflip << 10)) & 0x600;

            name = (name >> 1) & 0x1FF;
            name &= ~((cgy << 1) | cgx);
            name |= flip;
            if(xflip && cgx) name ^= 1;

            sprite_list[i].top = ypos;
            sprite_list[i].bottom = ypos + height;
            sprite_list[i].xpos = xpos;
            sprite_list[i].name_left = name;
            sprite_list[i].name_right = name ^ 1;
            sprite_list[i].height = (height - 1);
            sprite_list[i].palette = (attr & 0x0F) << 4;

            if(yflip)
                sprite_list[i].flags |= FLAG_YFLIP;

            if(cgx)
                sprite_list[i].flags |= FLAG_CGX;

            if(!(attr & 0x80))
                sprite_list[i].flags |= FLAG_PRIORITY;

            used_sprite_list[used_sprite_index] = (i);
            used_sprite_index += 1;
        }
    }

    return (used_sprite_index);
}



