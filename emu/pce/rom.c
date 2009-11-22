/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* rom.c: NEC PCEngine ROM parser */

#include "bc_emu.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/psg.h"
#include "emu/pce/rom.h"
/* #include "tools/memdump.h" */


/**
 * Parse pre-loaded ROM image.
 * \return          1 if success, otherwise 0
 */
int pce_rom_parse()
{
	uint8* buf = NULL, *ptr = NULL;
	int size;

	debug("ROM parsing rom image");
	assert(emu_rom && emu_rom->data);
	assert(pce);

	size = emu_rom->size;
	debug("ROM image size is: %d bytes", size);

#ifdef DEBUG
	if(size > 0x100000)
		debug("ROM image is larger than PCE maximum: 1048576 bytes!");
#endif /* DEBUG */

	buf = emu_rom->data;
	ptr = buf;

	/* some ROM images needs stripping of the first 512 bytes of header. ROM
	 * image should be even multiple of 512, odd multiples are most certainly
	 * containing header. */
	if((size / 512) & 1)
	{
		debug("ROM header found. stripping first 512 bytes.");
		size -= 512;
		buf += 512;
	}

	/* ROM images of exact size 393216 bytes must be splitted in memory as
	 * described in the following scheme:
	 * 1: image offset: 0,      length: 262144 bytes -> ROM offset: 0
	 * 2: image offset: 262144  length: 131072 bytes -> ROM offset: 524288 */
	if(size == 0x60000)
	{
		debug("spliting ROM");
		memcpy(pce->rom + 0x00000, buf + 0x00000, 0x40000);
		memcpy(pce->rom + 0x80000, buf + 0x40000, 0x20000);
	}
	else
	{
		memcpy(pce->rom, buf, (size > 0x100000) ? 0x100000 : size);
	}

#ifdef DEBUG
	debug("ROM parsed successfully");
	/* memdump(pce->rom, (size > 0x100000) ? 0x10000 : size); */
#endif /* DEBUG */

	/* we don't need entire emu_rom anymore. Cleanup */
	debug("ROM image was moved into PCE emulator. cleaning-up.");
	xfree(emu_rom->data);
	xfree(emu_rom);

	return 1;
}
