/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* vdc_huc6270.h: NEC PCEngine HuC6270 VDC emulator */

#ifndef __VDC_HUC6270_H
#define __VDC_HUC6270_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

typedef struct
{
	/* registry */
	uint16 reg[0x20];       /**< HuC6270 VDC registers */
		/*
		0x00: MAWR  W   memory access write register (addresss counter)
		0x01: MARR  W   memory access read register (address counter)
		0x02: VRR   W   VRAM read register (read VRAM at MARR)
		0x02: VWR   R   VRAM write register (write VRAM at MAWR)
		0x03: ?
		0x04: ?
		0x05: CR    W   control register
		0x06: R     W   raster counter register
		0x07: BXR   W   background X-scroll register
		0x08: BYR   W   background Y-scroll register
		0x09: MWR   W   memory width register (controls bg bitmap width)
		0x0A: HSR   W   horizontal sync register
		0x0B: HDR   W   horizontal display register
		0x0C: VPR   W   vertical sync register
		0x0D: VDW   W   vertical display register
		0x0E: VCR   W   vertical display end-pos register
		0x0F: DCR   W   DMA control register
		0x10: SOUR  W   DMA source address register
		0x11: DESR  W   DMA destination address register
		0x12: LENR  W   DMA block length register
		0x13: SATB  W   sprite attribute table (points to begin address)
		0x14: ?
		... : ?
		0x1F: ? */
	uint8 status;           /**< status flags register */
	uint8 latch;            /**< register latch */
	uint8 vram_latch;       /**< video RAM latch */
	int8 addr_inc;          /**< address increment register set by 0x05 write */
	uint8 dvssr;            /**< VRAM to SATB DMA trigger register */

	/* write pointers */
	uint16* vram_write;     /**< pointer to current write location in VRAM */

	/* display properties (NOT HW) */
	int disp_width;         /**< display width (in pixels) */
	int disp_height;        /**< display height (in pixels) */
	int disp_width_old;     /**< old display width when viewport changes */
	int disp_height_old;    /**< old display height when viewport changes */

	/* background pattern cache */
	uint8 bg_cache[0x20000];
	uint16 bg_list[0x800];  /**< list of background patterns indexed by address >> 6 */
	uint16 bg_list_i;       /**< current index in background pattern list */
	uint8 bg_dirty[0x800];

	/* sprite pattern cache */
	uint8 sp_cache[0x80000];
	uint16 sp_list[0x200];  /**< list of sprite patterns indexed by address >> 6 */
	uint16 sp_list_i;       /**< current index in sprite pattern list */
	uint16 sp_dirty[0x200];

	/* used sprite list */
	t_pce_vdc_sprite used_sprites[0x40];
	uint8 used_sprite_list[0x40];
	uint8 used_sprite_i;

	/* rendering */
	int planes = -1         /**< plane enable, bit 0: background, bit 1: sprite */
	uint16 pixel[2][0x100]  /**< VCE color to 16bit pixel table */
	uint16 pixel_lut[0x200] /**< precalculated 16bit pixel lookup table */
	uint32 bp_lut[0x10000]  /**< precalculated bit-plane lookup table */


	/* screen */
/*
extern int playfield_shift;
extern uint32 playfield_col_mask;
extern uint32 playfield_row_mask;
extern int playfield_shift_table[];
extern int playfield_row_mask_table[];
*/
} t_pce_vdc

/**
 * Sprite. One SATB entry.
 */
typedef struct {
	int32   top;            /**< SATB 0x00, sprite top */
	int32   bottom;         /**< SATB 0x04, sprite bottom */
	int32   x;              /**< SATB
} t_pce_vdc_sprite;

/*
extern uint8 objram[0x200];
extern uint32 y_offset;
extern uint32 byr;
*/

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

/* Status flag register constants */
#define VDC_BSY         0x40    /**< DMA busy (DMA transfer is happening) */
#define VDC_VD          0x20    /**< VSYNC (vertical blanking) */
#define VDC_DV          0x10    /**< DMA transfer from VRAM to VRAM */
#define VDC_DS          0x08    /**< DMA transfer from VRAM to SATB */
#define VDC_RR          0x04    /**< scanline interrupt */
#define VDC_OR          0x02    /**< sprite overflow */
#define VDC_OC          0x01    /**< sprite collision */

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce_vdc*   pce_vdc;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int          pce_vdc_init();
extern void         pce_vdc_reset();
extern void         pce_vdc_shutdown();

extern int          pce_vdc_r(int);
extern void         pce_vdc_w(int, int);
extern void         pce_vdc_dma();

#endif /* __VDC_HUC6270_H */
