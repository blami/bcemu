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
 * Sprite attributes table (SATB) entry.
 */
typedef struct {
	int32   top;                    /**< 0x00, top */
	int32   bottom;                 /**< 0x04, bottom (top+height) */
	int32   xpos;                   /**< 0x08, X position */
	uint32  pat_addr_l;             /**< 0x0C, pattern address left-inverted */
	uint32  pat_addr_r;             /**< 0x10, pattern address right-inverted */
	uint32  height;                 /**< 0x14, height */
	uint8   palette;                /**< 0x18, sprite pallete index */
	uint8   attr;                   /**< 0x19, attributes (enable, SPBG etc.) */
	uint8   fill[6];

} t_pce_vdc_sprite;

/**
 * NEC PCEngine HuC6270 VDC (video display controller).
 */
typedef struct
{
	/* registers */
	uint16 reg[0x20];               /**< HuC6270 VDC registers */
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
		0x1F: ?
		*/
	uint8 status;                   /**< status flags register */
	uint8 latch;                    /**< register latch */
	uint8 vram_latch;               /**< video RAM latch */
	int8 addr_inc;                  /**< address increment register (0x05) */
	uint8 dvssr;                    /**< VRAM to SATB DMA trigger register */

	/* write pointers */
	uint16* vram_write;             /**< write location in VRAM */
	uint16* satb_write;             /**< write location in SATB */

	/* display */
	int disp_width;                 /**< display width (pixels) */
	int disp_height;                /**< display height (pixels) */
	int disp_width_old;             /**< old display width */
	int disp_height_old;            /**< old display height */
	uint32 y_offset;
	uint32 byr;
	int planes;                 /**< plane enable, bit 0: background, bit 1: sprite */
	int buf_shift;
	uint32 buf_col_mask;
	uint32 buf_row_mask;
	int buf_shift_table[4];
	int buf_row_mask_table[4];

	/* background tile pattern cache (indexed by pat_addr (addr << 6) */
	uint8 bp_cache[0x20000];        /**< pattern cache */
	uint16 bp_list[0x800];          /**< list of background tile patterns */
	uint16 bp_list_i;               /**< current index */
	uint8 bp_dirty[0x800];          /**< dirty entries */

	/* sprite character pattern cache (indexed by pat_addr (addr << 6) */
	uint8 sp_cache[0x80000];        /**< pattern cache */
	uint16 sp_list[0x200];          /**< list of sprite character patterns */
	uint16 sp_list_i;               /**< current index */
	uint16 sp_dirty[0x200];         /**< dirty entries */

	/* sprite list */
	t_pce_vdc_sprite sprite[0x40];
	uint8 sprite_list[0x40];
	uint8 sprite_list_i;

} t_pce_vdc;

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

/*
 * Status flag register constants
 */
#define VDC_BSY         0x40        /**< DMA busy (DMA transfer is happening) */
#define VDC_VD          0x20        /**< VSYNC (vertical blanking) */
#define VDC_DV          0x10        /**< DMA transfer from VRAM to VRAM */
#define VDC_DS          0x08        /**< DMA transfer from VRAM to SATB */
#define VDC_RR          0x04        /**< scanline interrupt */
#define VDC_OR          0x02        /**< sprite overflow */
#define VDC_OC          0x01        /**< sprite collision */

/*
 * Sprite attribute flags
 */
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
