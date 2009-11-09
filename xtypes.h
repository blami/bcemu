/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* xtypes.c: Portable types used in program. */

#ifndef __XTYPES_H
#define __XTYPES_H


/*
 * Portable standart integer types.
 */
typedef unsigned char                       uint8;
typedef unsigned short int                  uint16;
typedef unsigned int                        uint32;
__extension__ typedef unsigned long long    uint64;
typedef signed char                         int8;
typedef signed short int                    int16;
typedef signed int                          int32;
__extension__ typedef signed long long      int64;

/**
 * PAIR union is pair of two 16bit UINTs affected by host system endianess.
 */
typedef union {
#ifdef LSB
	struct { uint8 l,h,h2,h3; } b;
	struct { uint16 l,h; } w;
#else
	struct { uint8 h3,h2,h,l; } b;
	struct { uint16 h,l; } w;
#endif
	uint32 d;
} pair;

#endif /* _XTYPES_H */
