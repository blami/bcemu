/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* rom_parser.c: NEC PCEngine ROM parser */

#include "bc_emu.h"


/**
 * Parse pre-loaded ROM image to emulators ROM memory. This function uses
 * global emu_rom pointer to get ROM image loaded by arch-specific function.
 * \return          1 if success, otherwise 0
 */
int pce_rom_parse()
{
	uint8* buf = NULL, *ptr = NULL;
	int size;

	debug("ROM parser initiated");
	assert(emu_rom && emu_rom->data);
	assert(pce);

	size = emu_rom->size;
	debug("ROM image size is: %d bytes", size);

#ifdef DEBUG
	if(size > 0x100000)
		debug("ROM image is larger than maximum: 1048576 bytes!");
#endif /* DEBUG */

	buf = emu_rom->data;
	ptr = buf;

	/* some ROM images need to strip the first 512 bytes of header. ROM image
	 * should be even-multiple of 512, odd multiples are most certainly
	 * containing header. */
	if((size / 512) & 1)
	{
		debug("ROM header found. stripping first 512 bytes.");
		size -= 512;
		buf += 512;
	}

	/* ROM images of exact size 393216 bytes must be splitted in ROM memory as
	 * described in following scheme:
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
	/* mem_dump(pce->rom, (size > 0x100000) ? 0x10000 : size); */
#endif /* DEBUG */

	/* we don't need entire emu_rom anymore. Cleanup */
	debug("ROM image was moved into PCE emulator. cleaning-up.");
	xfree(emu_rom->data);
	xfree(emu_rom);

	return 1;
}
