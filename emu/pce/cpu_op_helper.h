/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * Copyright © 1998-2000 Juergen Buchmueller                                  *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* cpu_op_util.h: HuC6280 CPU opcode set helper macros */

#ifndef __PCE_CPU_OP_HELPER_H
#define __PCE_CPU_OP_HELPER_H


/*
 * NOTE: code generates bodies of OP-code functions in cpu_op_table.inc, there
 * are widely used macros from cpu_huc6280.h and variables `arg1' and `arg2'.
 * They must be defined in any place where these stub-generators are used.
 */

/* -------------------------------------------------------------------------- *
 * Flag helpers                                                               *
 * -------------------------------------------------------------------------- */

/**
 * Set proper NZ flags in CPU P (status) register according to `x' where N will
 * be set if x < 0 and Z if x == 0.
 * \param x             value that affects P flags 
 */
#define SET_NZ(n)                                                       \
	P = (P & ~(_pN | _pT | _pZ)) | (n & _pN) | ((n == 0) ? _pZ : 0)

/**
 * Compose CPU P (status) register by setting `set' and un-setting `clear' bits.
 * \param set           bits to be set
 * \param clear         bits to be un-set
 */
#define COMPOSE_P(set, clear)                                           \
	P = (P & ~clear) | set

/* -------------------------------------------------------------------------- *
 * Interrupt helpers                                                          *
 * -------------------------------------------------------------------------- */

/**
 * Handle specified interrupt vector. Takes 7 CPU cycles.
 * \param vector        interrupt vector 
 */
#define DO_INTERRUPT(vector)                                            \
{                                                                       \
	pce_cpu->extra_cycles += 7;                                         \
	PUSH(PCH);                                                          \
	PUSH(PCL);                                                          \
	COMPOSE_P(0, _pB);                                                  \
	PUSH(P);                                                            \
	P = (P & ~_pD) | _pI;                                               \
	PCL = RDMEM(vector);                                                \
	PCH = RDMEM((vector+1));                                            \
}

/**
 * Check all interrupt lines and DO_INTERUPT on non-clear lines.
 */
#define CHECK_IRQ_LINES                                                 \
	if(!(P & _pI))                                                      \
	{                                                                   \
		/* IRQ1 */                                                      \
		if(pce_cpu->irq_state[0] != CLEAR_LINE &&                       \
			!(pce_cpu->irq_mask & 0x2))                                 \
		{                                                               \
			DO_INTERRUPT(INT_VEC_IRQ1);                                 \
			(*pce_cpu->irq_callback)(0);                                \
		}                                                               \
		/* IRQ2 */                                                      \
		else if(pce_cpu->irq_state[1] != CLEAR_LINE &&                  \
			!(pce_cpu->irq_mask & 0x1))                                 \
		{                                                               \
			DO_INTERRUPT(INT_VEC_IRQ2);                                 \
			(*pce_cpu->irq_callback)(1);                                \
		}                                                               \
		/* timer */                                                     \
		else if(pce_cpu->irq_state[2] != CLEAR_LINE &&                  \
			!(pce_cpu->irq_mask & 0x4) )                                \
		{                                                               \
			pce_cpu->irq_state[2] = CLEAR_LINE;                         \
			DO_INTERRUPT(INT_VEC_TIMER);                                \
		}                                                               \
	}

/* -------------------------------------------------------------------------- *
 * Memory access helpers                                                      *
 * -------------------------------------------------------------------------- */

/**
 * Read memory.
 * \param addr          address to be read
 */
#define RDMEM(addr)                                                     \
	pce_cpu_mem_r((pce_cpu->mmr[(addr) >> 13] << 13) |                  \
		((addr) & 0x1fff))

/**
 * Write memory.
 * \param addr          address to be written
 * \param data          data
 */
#define WRMEM(addr,data)                                                \
	pce_mem_w((pce_cpu->mmr[(addr) >> 13] << 13) |                      \
		((addr) & 0x1fff), data);

/**
 * Read zero-page memory.
 * \param addr          address to be read
 */
#define RDMEMZ(addr)                                                    \
	pce_cpu_mem_r((pce_cpu->mmr[1] << 13) | ((addr) & 0x1fff));

/**
 * Write zero-page memory.
 * \param addr          address to be written
 * \param data          data
 */
#define WRMEMZ(addr,data)                                               \
	pce_cpu_mem_w((pce_cpu->mmr[1] << 13) | ((addr) & 0x1fff),          \
		data);

/**
 * Read a word from memory.
 * \param addr          address to be read
 */
#define RDMEMW(addr)                                                    \
	pce_cpu_mem_r((pce_cpu->mmr[(addr) >> 13] << 13) |                  \
		((addr) & 0x1fff))                                              \
	|                                                                   \
	(pce_cpu_mem_r((pce_cpu->mmr[(addr+1) >> 13] << 13) |               \
		((addr + 1) & 0x1fff)) << 8)

/**
 * Read a word from zero-page memory.
 * \param addr          address to be read
 */
#define RDZPWORD(addr)                                                  \
	((addr & 0xff) == 0xff) ?                                           \
		/* handle corner cases */                                       \
		pce_cpu_mem_r((pce_cpu->mmr[1] << 13) | ((addr) & 0x1fff))      \
		+ (pce_cpu_mem_r((pce_cpu->mmr[1] << 13) | ((addr - 0xff) & 0x1fff)) << 8) \
		:                                                               \
		pce_cpu_mem_r((pce_cpu->mmr[1] << 13) | ((addr)&0x1fff))        \
		+ (pce_cpu_mem_r( (pce_cpu->mmr[1] << 13) | ((addr+1)&0x1fff))<<8)

/* -------------------------------------------------------------------------- *
 * Stack helpers                                                              *
 * -------------------------------------------------------------------------- */

/**
 * Push register on stack. Lower SP (stack pointer).
 * \param reg           register
 */
#define PUSH(reg)                                                       \
	pce_cpu_mem_w((pce_cpu->mmr[1] << 13) | pce_cpu->sp.d, reg);        \
	S--

/**
 * Pull register from stack. Increase SP (stack pointer).
 * \param reg           register
 */
#define PULL(reg)                                                       \
	S++;                                                                \
	reg = pce_cpu_mem_r((pce_cpu->mmr[1] << 13) | pce_cpu->sp.d)

/* -------------------------------------------------------------------------- *
 * Opcode fetch helpers                                                       *
 * -------------------------------------------------------------------------- */

/**
 * Read (fetch) opcode from memory at PC address.
 */
#define RDOP()                                                          \
	pce_cpu_mem_r((pce_cpu->mmr[PCW>>13] << 13) | (PCW & 0x1fff))

/**
 * Read (fetch) opcode argument from memory at PC address.
 */
#define RDOPARG()                                                       \
	pce_cpu_mem_r((pce_cpu->mmr[PCW>>13] << 13) | (PCW&0x1fff))

/* -------------------------------------------------------------------------- *
 * Branch helpers                                                             *
 * -------------------------------------------------------------------------- */

/**
 * Branch relative.
 * \param cond          branch condition
 */
#define BRA(cond)                                                       \
	if(cond)                                                            \
	{                                                                   \
		/* positive branch */                                           \
		pce_cpu->cycle_count -= 4;                                      \
		arg1 = RDOPARG();                                               \
		PCW++;                                                          \
		EAW = PCW + (signed char)arg1;                                  \
		PCD = EAD;                                                      \
	}                                                                   \
	else                                                                \
	{                                                                   \
		/* negative branch */                                           \
		PCW++;                                                          \
		pce_cpu->cycle_count -= 2;                                      \
	}

/* -------------------------------------------------------------------------- *
 * Addressing mode helpers                                                    *
 * -------------------------------------------------------------------------- */

/**
 * EA = zero page.
 */
#define EA_ZPG                                                          \
	ZPL = RDOPARG();                                                    \
	PCW++;                                                              \
	EAD = ZPD

/**
 * EA = zero page + X register.
 */
#define EA_ZPX                                                          \
	ZPL = RDOPARG() + X;                                                \
	PCW++;                                                              \
	EAD = ZPD

/**
 * EA = zero page + Y register.
 */
#define EA_ZPY                                                          \
	ZPL = RDOPARG() + Y;                                                \
	PCW++;                                                              \
	EAD = ZPD

/**
 * EA = zero page indirect (pre).
 */
#define EA_ZPI                                                          \
	ZPL = RDOPARG();                                                    \
	PCW++;                                                              \
	EAD = RDZPWORD(ZPD)

/**
 * EA = zero page indirect + X register (pre).
 */
#define EA_IDX                                                          \
	ZPL = RDOPARG() + X;                                                \
	PCW++;                                                              \
	EAD = RDZPWORD(ZPD);

/**
 * EA = zero page indirect + Y register (post).
 */
#define EA_IDY                                                          \
	ZPL = RDOPARG();                                                    \
	PCW++;                                                              \
	EAD = RDZPWORD(ZPD);                                                \
	EAW += Y

/**
 * EA = absolute.
 */
#define EA_ABS                                                          \
	EAL = RDOPARG();                                                    \
	PCW++;                                                              \
	EAH = RDOPARG();                                                    \
	PCW++

/**
 * EA = absolute + X register.
 */
#define EA_ABX                                                          \
	EA_ABS;                                                             \
	EAW += X

/**
 * EA = absolute + Y register.
 */
#define EA_ABY                                                          \
	EA_ABS;                                                             \
	EAW += Y

/**
 * EA = indirect.
 */
#define EA_IND                                                          \
	EA_ABS;                                                             \
	arg1 = RDMEM(EAD);                                                  \
	EAD++;                                                              \
	EAH = RDMEM(EAD);                                                   \
	EAL = arg1

/**
 * EA indirect + X register.
 */
#define EA_IAX                                                          \
	EA_ABS;                                                             \
	EAD+=X;                                                             \
	arg1 = RDMEM(EAD);                                                  \
	EAD++;                                                              \
	EAH = RDMEM(EAD);                                                   \
	EAL = arg1

/* -------------------------------------------------------------------------- *
 * OP-code argument helpers                                                   *
 * -------------------------------------------------------------------------- */

/* read argument */
#define RD_IMM      arg1 = RDOPARG(); PCW++
#define RD_IMM2     arg2 = RDOPARG(); PCW++
#define RD_ACC      arg1 = A
#define RD_ZPG      EA_ZPG; arg1 = RDMEMZ(EAD)
#define RD_ZPX      EA_ZPX; arg1 = RDMEMZ(EAD)
#define RD_ZPY      EA_ZPY; arg1 = RDMEMZ(EAD)
#define RD_ABS      EA_ABS; arg1 = RDMEM(EAD)
#define RD_ABX      EA_ABX; arg1 = RDMEM(EAD)
#define RD_ABY      EA_ABY; arg1 = RDMEM(EAD)
#define RD_ZPI      EA_ZPI; arg1 = RDMEM(EAD)
#define RD_IDX      EA_IDX; arg1 = RDMEM(EAD)
#define RD_IDY      EA_IDY; arg1 = RDMEM(EAD)

/* write argument */
#define WR_ZPG      EA_ZPG; WRMEMZ(EAD, arg1)
#define WR_ZPX      EA_ZPX; WRMEMZ(EAD, arg1)
#define WR_ZPY      EA_ZPY; WRMEMZ(EAD, arg1)
#define WR_ABS      EA_ABS; WRMEM(EAD, arg1)
#define WR_ABX      EA_ABX; WRMEM(EAD, arg1)
#define WR_ABY      EA_ABY; WRMEM(EAD, arg1)
#define WR_ZPI      EA_ZPI; WRMEM(EAD, arg1)
#define WR_IDX      EA_IDX; WRMEM(EAD, arg1)
#define WR_IDY      EA_IDY; WRMEM(EAD, arg1)

/* write-back argument to last unmodified EA (effective address) */
#define WB_ACC      A = (UINT8)arg1;
#define WB_EA       WRMEM(EAD, arg1)
#define WB_EAZ      WRMEMZ(EAD, arg1)

#endif /* __PCE_CPU_OP_HELPER_H */
