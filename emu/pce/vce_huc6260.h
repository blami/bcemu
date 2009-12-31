/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* vce_huc6260.h: NEC PCEngine HuC6280 VCE emulator */

#ifndef __VCE_HUC6260_H
#define __VCE_HUC6260_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * NEC PCEngine VCE (video color encoder).
 */
typedef struct
{
	/* registers */
	uint8 ctrl;                     /**< control */
	uint8 data[0x400];              /**< color data */
	uint16 addr;                    /**< address */

	/* pre-calculated lookup tables */
	uint16 pixel_lut[512];          /**< pixel lookup table (512 colors) */
	uint32 bp_lut[0x10000];         /**< bit-plane lookup table */

	/* pixel table */
	uint16 pixel[2][256];           /**< VCE color to 16bit pixel table */

} t_pce_vce;

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce_vce* pce_vce;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int          pce_vce_init();
extern void         pce_vce_shutdown();

extern uint8        pce_vce_r(int addr);
extern void         pce_vce_w(int addr, int data);

#endif /* __VCE_HUC6260_H */
