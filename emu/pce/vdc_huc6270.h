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

/**
 * Sprite attributes.
 */
typedef struct {
	int32   top;            /**< 0x00, top */
	int32   bottom;         /**< 0x04, bottom (top+height) */
	int32   xpos;           /**< 0x08, X position */
	uint32  pat_addr_l;     /**< 0x0C, pattern address left-inverted */
	uint32  pat_addr_r;     /**< 0x10, pattern address right-inverted */
	uint32  height;         /**< 0x14, height */
	uint8   palette;        /**< 0x18, sprite pallete index */
	uint8   attr;           /**< 0x19, attributes (like enable, SPBG etc.) */
	uint8   fill[6];
} t_pce_vdc_sprite;

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

	uint16* vram_write;     /**< pointer to current write location in VRAM */
	uint16* satb_write;     /**< pointer to current write location in SATB */

	/* display properties */
	int disp_width;         /**< display width (in pixels) */
	int disp_height;        /**< display height (in pixels) */
	int disp_width_old;     /**< old display width when viewport changes */
	int disp_height_old;    /**< old display height when viewport changes */
	uint32 y_offset;
	uint32 byr;

	/* background tile pattern cache */
	uint8 bp_cache[0x20000];/**< background tile pattern cache */
	uint16 bp_list[0x800];  /**< list of background patterns indexed by address >> 6 */
	uint16 bp_list_i;       /**< current index in background pattern list */
	uint8 bp_dirty[0x800];

	/* sprite pattern cache */
	uint8 sp_cache[0x80000];/**< sprite character cache */
	uint16 sp_list[0x200];  /**< list of sprite patterns indexed by address >> 6 */
	uint16 sp_list_i;       /**< current index in sprite pattern list */
	uint16 sp_dirty[0x200];

	/* used sprite list */
	t_pce_vdc_sprite sprite[0x40];
	uint8 sprite_list[0x40];
	uint8 sprite_list_i;

	/* rendering */
	int planes;                 /**< plane enable, bit 0: background, bit 1: sprite */
	uint16 pixel[2][0x100];     /**< VCE color to 16bit pixel table */
	uint16 pixel_lut[0x200];    /**< precalculated 16bit pixel lookup table */
	uint32 bp_lut[0x10000];     /**< precalculated bit-plane lookup table */

	/* screen buffer */
	int buf_shift;
	uint32 buf_col_mask;
	uint32 buf_row_mask;
	int buf_shift_table[4];
	int buf_row_mask_table[4];

} t_pce_vdc;

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

/* Sprite attribute flags */
#define SP_ENABLE       0x80
#define SP_SPBG         0x01
#define SP_CGX          0x02
#define SP_YFLIP        0x04

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
