/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * Copyright © 1998-2000 Juergen Buchmueller                                  *
 * Copyright © 2006 Mark Ormston                                              *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* cpu_instr.inc: HuC6280 CPU instruction set implementation */

/*
 * NOTE: this code references arg1, arg2 and some pce_cpu members. It should
 * be used only from cpu_huc6280.c. This is the reason why its .inc and not .h.
 */

#ifndef __PCE_CPU_INSTR_INC
#define __PCE_CPU_INSTR_INC


/* -------------------------------------------------------------------------- *
 * 65c02 instructions                                                         *
 * -------------------------------------------------------------------------- */

/*
 * ADC
 * Add with carry.
 */
#define ADC \
	if(P & _pD) \
	{ \
		int c = (P & _pC); \
		int lo = (A & 0x0f) + (arg1 & 0x0f) + c; \
		int hi = (A & 0xf0) + (arg1 & 0xf0); \
		P &= ~(_pV | _pC); \
		if(lo > 0x09) \
		{ \
			hi += 0x10; \
			lo += 0x06; \
		} \
		if(~(A ^ arg1) & (A ^ hi) & _pN) \
			P |= _pV; \
		if(hi > 0x90) \
			hi += 0x60; \
		if(hi & 0xff00) \
			P |= _pC; \
		A = (lo & 0x0f) + (hi & 0xf0); \
	} \
	else \
	{ \
	int c = (P & _pC); \
	int sum = A + arg1 + c; \
		P &= ~(_pV | _pC); \
		if(~(A^arg1) & (A^sum) & _pN) \
			P |= _pV; \
		if(sum & 0xff00) \
			P |= _pC; \
		A = (uint8)sum; \
	} \
	SET_NZ(A)

/*
 * AND
 * Logical and.
 */
#define AND \
	A = (uint8)(A & arg1); \
	SET_NZ(A)

/*
 * ASL
 * Attribute shift left.
 */
#define ASL \
	P = (P & ~_pC) | ((arg1 >> 7) & _pC); \
	arg1 = (uint8)(arg1 << 1); \
	SET_NZ(arg1)

/*
 * BBR
 * Branch if bit is reset.
 */
#define BBR(bit) \
	BRA(!(arg1 & (1 << bit)))

/*
 * BBS
 * Branch if bit is set.
 */
#define BBS(bit) \
	BRA(arg1 & (1 << bit))

/*
 * BBC
 * Branch if carry clear.
 */
#define BCC \
	BRA(!(P & _pC))

/*
 * BCS
 * Branch if carry set.
 */
#define BCS \
	BRA(P & _pC)

/*
 * BEQ
 * Branch if equal.
 */
#define BEQ \
	BRA(P & _pZ)

/*
 * BIT
 * Bit test.
 */
#define BIT \
	P = (P & ~(_pN|_pV|_pT|_pZ)) \
		| ((arg1 & 0x80) ? _pN:0) \
		| ((arg1 & 0x40) ? _pV:0) \
		| ((arg1 & A)  ? 0:_pZ)

/*
 * BMI
 * Branch if minus.
 */
#define BMI \
	BRA(P & _pN)

/*
 * BNE
 * Branch if not equal.
 */
#define BNE BRA(!(P & _pZ))

/*
 * BPL
 * Branch if plus.
 */
#define BPL \
	BRA(!(P & _pN))

/*
 * BRK
 * Break.
 */
#define BRK \
	PCW++; \
	PUSH(PCH); \
	PUSH(PCL); \
	PUSH(P | _pB); \
	P = (P & ~_pD) | _pI; \
	PCL = RDMEM(INT_VEC_IRQ2); \
	PCH = RDMEM(INT_VEC_IRQ2+1)

/*
 * BVC
 * Branch if overflow clear.
 */
#define BVC \
	BRA(!(P & _pV))

/*
 * BVS
 * Branch if overflow set.
 */
#define BVS \
	BRA(P & _pV)

/*
 * CLC
 * Clear carry flag.
 */
#define CLC \
	P &= ~_pC

/*
 * CLD
 * Clear decimal flag.
 */
#define CLD \
	P &= ~_pD

/*
 * CLI
 * Clear interrupt flag.
 */
#define CLI \
	if(P & _pI) \
	{ \
		P &= ~_pI; \
		CHECK_IRQ_LINES; \
	}

/*
 * CLV
 * Clear overflow flag.
 */
#define CLV \
	P &= ~_pV

/*
 * CMP
 * Compare accumulator.
 */
#define CMP \
	P &= ~_pC; \
	if(A >= arg1) \
		P |= _pC; \
	SET_NZ((uint8)(A - arg1))

/*
 * CPX
 * Compare index X.
 */
#define CPX \
	P &= ~_pC; \
	if(X >= arg1) \
		P |= _pC; \
	SET_NZ((uint8)(X - arg1))

/*
 * CPY
 * Compare index Y.
 */
#define CPY \
	P &= ~_pC; \
	if(Y >= arg1) \
		P |= _pC; \
	SET_NZ((uint8)(Y - arg1))

/*
 * DEA
 * Decrement accumulator.
 */
#define DEA \
	A = (uint8)--A; \
	SET_NZ(A)

/*
 * DEC
 * Decrement memory.
 */
#define DEC \
	arg1 = (uint8)--arg1; \
	SET_NZ(arg1)

/*
 * DEX
 * Decrement index X.
 */
#define DEX \
	X = (uint8)--X; \
	SET_NZ(X)

/*
 * DEY
 * Decrement index Y.
 */
#define DEY \
	Y = (uint8)--Y; \
	SET_NZ(Y)

/*
 * EOR
 * Logical exclusive or.
 */
#define EOR \
	A = (uint8)(A ^ arg1); \
	SET_NZ(A)

/*
 * ILL
 * Ilegal opcode.
 */
#define ILL \
	debug("CPU illegal instruction (pc=%04x)", pce_cpu_reg_r(CPU_PC)); \
	pce_cpu->cycle_count -= 2;

/*
 * INA
 * Increment accumulator.
 */
#define INA \
	A = (uint8)++A; \
	SET_NZ(A)

/*
 * INC
 * Increment memory.
 */
#define INC \
	arg1 = (uint8)++arg1; \
	SET_NZ(arg1)

/*
 * INX
 * Increment index X.
 */
#define INX \
	X = (uint8)++X; \
	SET_NZ(X)

/*
 * INY
 * Increment index Y.
 */
#define INY \
	Y = (uint8)++Y; \
	SET_NZ(Y)

/*
 * JMP
 * Jump to effective address.
 */
#define JMP \
	PCD = EAD

/*
 * JSR
 * Jump to subroutine.
 */
#define JSR \
	PCW--; \
	PUSH(PCH); \
	PUSH(PCL); \
	PCD = EAD

/*
 * LDA
 * Load accumulator.
 */
#define LDA \
	A = (uint8)arg1; \
	SET_NZ(A)

/*
 * LDX
 * Load index X.
 */
#define LDX \
	X = (uint8)arg1; \
	SET_NZ(X)

/*
 * LDY
 * Load index Y.
 */
#define LDY \
	Y = (uint8)arg1; \
	SET_NZ(Y)

/*
 * LSR
 * Logical shift right.
 */
#define LSR \
	P = (P & ~_pC) | (arg1 & _pC); \
	arg1 = (uint8)arg1 >> 1; \
	SET_NZ(arg1)

/*
 * NOP
 * No operation.
 */
#define NOP

/*
 * ORA
 * Logical inclusive or.
 */
#define ORA \
	A = (uint8)(A | arg1); \
	SET_NZ(A)

/*
 * PHA
 * Push accumulator.
 */
#define PHA \
	PUSH(A)

/*
 * PHP
 * Push status (P register).
 */
#define PHP \
	COMPOSE_P(0,0); \
	PUSH(P)

/*
 * PHX
 * Push index X.
 */
#define PHX \
	PUSH(X)

/*
 * PHY
 * Push index Y.
 */
#define PHY \
	PUSH(Y)

/*
 * PLA
 * Pull accumulator.
 */
#define PLA \
	PULL(A); \
	SET_NZ(A)

/*
 * PLP
 * Pull status (P register).
 */
#define PLP \
	PULL(P); \
	CHECK_IRQ_LINES

/*
 * PLX
 * Pull index X.
 */
#define PLX \
	PULL(X)

/*
 * PLY
 * Pull index Y.
 */
#define PLY \
	PULL(Y)

/*
 * RMB
 * Reset memory bit.
 */
#define RMB(bit) \
	arg1 &= ~(1 << bit)

/*
 * ROL
 * Rotate left.
 */
#define ROL \
	arg1 = (arg1 << 1) | (P & _pC); \
	P = (P & ~_pC) | ((arg1 >> 8) & _pC); \
	arg1 = (uint8)arg1; \
	SET_NZ(arg1)

/*
 * ROR
 * Rotate right.
 */
#define ROR \
	arg1 |= (P & _pC) << 8; \
	P = (P & ~_pC) | (arg1 & _pC); \
	arg1 = (uint8)(arg1 >> 1); \
	SET_NZ(arg1)

/*
 * RTI
 * Return from interrupt.
 */
#define RTI \
	PULL(P); \
	PULL(PCL); \
	PULL(PCH); \
	CHECK_IRQ_LINES

/*
 * RTS
 * Return from subroutine.
 */
#define RTS \
	PULL(PCL); \
	PULL(PCH); \
	PCW++; \

/*
 * SBC
 * Subtract with carry.
 */
#define SBC \
	if(P & _pD) \
	{ \
		int c = (P & _pC) ^ _pC; \
		int sum = A - arg1 - c; \
		int lo = (A & 0x0f) - (arg1 & 0x0f) - c; \
		int hi = (A & 0xf0) - (arg1 & 0xf0); \
		P &= ~(_pV | _pC); \
		if((A ^ arg1) & (A ^ sum) & _pN) \
			P |= _pV; \
		if (lo & 0xf0) \
			lo -= 6; \
		if (lo & 0x80) \
			hi -= 0x10; \
		if (hi & 0x0f00) \
			hi -= 0x60; \
		if ((sum & 0xff00) == 0) \
			P |= _pC; \
		A = (lo & 0x0f) + (hi & 0xf0); \
	} \
	else \
	{ \
	int c = (P & _pC) ^ _pC; \
	int sum = A - arg1 - c; \
		P &= ~(_pV | _pC); \
		if ((A^arg1) & (A^sum) & _pN) \
			P |= _pV; \
		if ((sum & 0xff00) == 0) \
			P |= _pC; \
		A = (uint8) sum; \
	} \
	SET_NZ(A)

/*
 * SEC
 * Set carry flag.
 */
#define SEC \
	P |= _pC

/*
 * SED
 * Set decimal flag.
 */
#define SED \
	P |= _pD

/*
 * SEI
 * Set interrupt flag.
 */
#define SEI \
	P |= _pI

/*
 * SMB
 * Set memory bit.
 */
#define SMB(bit) \
	arg1 |= (1<<bit)

/*
 * STA
 * Store accumulator.
 */
#define STA \
	arg1 = A

/*
 * STX
 * Store index X.
 */
#define STX \
	arg1 = X

/*
 * STY
 * Store index Y.
 */
#define STY \
	arg1 = Y

/*
 * STZ
 * Store zero.
 */
#define STZ \
	arg1 = 0

/*
 * TAX
 * Transfer accumulator to X index.
 */
#define TAX \
	X = A; \
	SET_NZ(X)

/*
 * TAY
 * Transfer accumulator to Y index.
 */
#define TAY \
	Y = A; \
	SET_NZ(Y)

/*
 * TRB
 * Test bits and reset.
 */
#define TRB \
	P = (P & ~(_pN | _pV | _pT | _pZ)) \
		| ((arg1&0x80) ? _pN : 0) \
		| ((arg1&0x40) ? _pV : 0) \
		| ((arg1&A) ? 0 : _pZ); \
	arg1 &= ~A

/*
 * TSB
 * Test bits and set.
 */
#define TSB \
	P = (P & ~(_pN | _pV | _pT | _pZ)) \
		| ((arg1 & 0x80) ? _pN : 0) \
		| ((arg1 & 0x40) ? _pV : 0) \
		| ((arg1 & A) ? 0 : _pZ); \
	arg1 |= A

/*
 * TSX
 * Transfer stack LSB to index X.
 */
#define TSX \
	X = S; \
	SET_NZ(X)

/*
 * TXA
 * Transfer index X to accumulator.
 */
#define TXA \
	A = X; \
	SET_NZ(A)

/*
 * TXS
 * Transfer index X to stack LSB.
 */
#define TXS \
	S = X

/*
 * TYA
 * Transfer index Y to accumulator.
 */
#define TYA \
	A = Y; \
	SET_NZ(A)

/* -------------------------------------------------------------------------- *
 * HuC6280 instructions                                                       *
 * -------------------------------------------------------------------------- */

/*
 * BSR
 * Branch to subroutine.
 */
#define BSR \
	PUSH(PCH); \
	PUSH(PCL); \
	pce_cpu->cycle_count -= 4; \
	BRA(1) /* +4 cycles */

/*
 * CLA
 * Clear accumulator.
 */
#define CLA \
	A = 0

/*
 * CLX
 * Clear index X.
 */
#define CLX \
	X = 0

/*
 * CLY
 * Clear index Y.
 */
#define CLY \
	Y = 0

/*
 * CSH
 * Clock select high (set speed to 7.16MHz)
 */
#define CSH \
	debug("CPU speed changed to 7.16MHz"); \
	pce_cpu->speed = 1

/*
 * CSL
 * Clock select low (set speed to 3.58MHz)
 */
#define CSL \
	debug("CPU speed changed to 3.58MHz"); \
	pce_cpu->speed = 0

/*
 * SAX
 * Swap accumulator and index X.
 */
#define SAX \
	arg1 = X; \
	X = A; \
	A = arg1

/*
 * SAY
 * Swap accumulator and index Y.
 */
#define SAY \
	arg1 = Y; \
	Y = A; \
	A = arg1

/*
 * SET
 * Set decimal memory operation flag.
 * FIXME this need more investigation, still crashes.
 */
#define SET \
{ \
	debug("CPU SET instruction. Problem may occur!"); \
	int op, arg1; \
	 \
	/* fetch instruction */ \
	op = RDOP(); \
	if((op & 0x9F) == 0x09) \
	{ \
		uint8 acc; \
		PCW++; \
		arg1 = RDOPARG(); \
		PCW++; \
		acc = RDMEMZ(X); \
\
		switch(op) \
		{ \
		case 0x09: /* ORA */ \
			acc = (uint8)(acc | arg1); \
			break; \
		case 0x29: /* AND */ \
			acc = (uint8)(acc & arg1); \
			break; \
		case 0x49: /* EOR */ \
			acc = (uint8)(acc ^ arg1); \
			break; \
		case 0x69: /* ADC */ \
			if(P & _pD) \
			{ \
				int c = (P & _pC); \
				int lo = (acc & 0x0F) + (arg1 & 0x0F) + c; \
				int hi = (acc & 0xF0) + (arg1 & 0xF0); \
				P &= ~(_pV | _pC); \
				if(lo > 0x09) \
				{ \
					hi += 0x10; \
					lo += 0x06; \
				} \
				if(~(acc ^ arg1) & (acc ^ hi) & _pN) \
					P |= _pV; \
				if(hi > 0x90) \
					hi += 0x60; \
				if(hi & 0xFF00) \
					P |= _pC; \
				 \
				acc = (lo & 0x0F) + (hi & 0xF0); \
			} \
			else \
			{ \
				int c = (P & _pC); \
				int sum = acc + arg1 + c; \
				P &= ~(_pV | _pC); \
				if(~(acc ^ arg1) & (acc ^ sum) & _pN) \
					P |= _pV; \
				if(sum & 0xff00) \
					P |= _pC; \
				 \
				acc = (uint8) sum; \
			} \
			break; \
		} \
\
		/* update flags using `fake' accumulator */ \
		SET_NZ(acc); \
		WRMEMZ(X, acc); \
		pce_cpu->cycle_count -= 8; \
	} \
	P &= ~_pT; \
}

/*
 * SXY
 * Swap index registers X and Y.
 */
#define SXY \
	arg1 = X; \
	X = Y; \
	Y = arg1

/*
 * ST0
 * Store at VDC address 1.
 */
#define ST0 \
	pce_vdc_w(0x0000, arg1)

/*
 * ST1
 * Store at VDC address 2.
 */
#define ST1 \
	pce_vdc_w(0x0002, arg1)

/*
 * ST2
 * Store at VDC address 3.
 */
#define ST2 \
	pce_vdc_w(0x0003, arg1)

/*
 * TAI
 * Transfer 'from' alternate 'to' increment.
 */
#define TAI \
	from = RDMEMW(PCW); \
	to = RDMEMW(PCW + 2); \
	length = RDMEMW(PCW + 4); \
	PCW += 6; \
	alternate = 0; \
	while ((length--) != 0) \
	{ \
		WRMEM(to, RDMEM(from + alternate)); \
		to++; \
		alternate ^= 1; \
	} \
	pce_cpu->cycle_count-=(6 * length) + 17;

/*
 * TAM
 * Transfer accumulator to MPR registers.
 */
#define TAM \
	if(arg1 & 0x01) pce_cpu->mpr[0] = A; \
	if(arg1 & 0x02) pce_cpu->mpr[1] = A; \
	if(arg1 & 0x04) pce_cpu->mpr[2] = A; \
	if(arg1 & 0x08) pce_cpu->mpr[3] = A; \
	if(arg1 & 0x10) pce_cpu->mpr[4] = A; \
	if(arg1 & 0x20) pce_cpu->mpr[5] = A; \
	if(arg1 & 0x40) pce_cpu->mpr[6] = A; \
	if(arg1 & 0x80) pce_cpu->mpr[7] = A

/*
 * TDD
 * Transfer 'from' decrement 'to' decrement.
 */
#define TDD \
	from = RDMEMW(PCW); \
	to = RDMEMW(PCW + 2); \
	length = RDMEMW(PCW + 4); \
	PCW += 6; \
	while((length--) != 0) \
	{ \
		WRMEM(to, RDMEM(from)); \
		to--; \
		from--; \
	} \
	pce_cpu->cycle_count-=(6 * length) + 17;

/*
 * TIA
 * Transfer 'from' increment 'to' alternate.
 */
#define TIA \
	from = RDMEMW(PCW); \
	to = RDMEMW(PCW + 2); \
	length = RDMEMW(PCW + 4); \
	PCW += 6; \
	alternate = 0; \
	while((length--) != 0) \
	{ \
		WRMEM(to + alternate, RDMEM(from)); \
		from++; \
		alternate ^= 1; \
	} \
	pce_cpu->cycle_count -= (6 * length) + 17;

/*
 * TII
 * Transfer 'from' increment 'to' increment.
 */
#define TII \
	from = RDMEMW(PCW); \
	to = RDMEMW(PCW + 2); \
	length = RDMEMW(PCW + 4); \
	PCW += 6; \
	while((length--) != 0) \
	{ \
		WRMEM(to, RDMEM(from)); \
		to++; \
		from++; \
	} \
	pce_cpu->cycle_count -= (6 * length) + 17;

/*
 * TIN
 * Transfer 'from' increment 'to' does nothing.
 */
#define TIN \
	from = RDMEMW(PCW); \
	to = RDMEMW(PCW + 2); \
	length = RDMEMW(PCW + 4); \
	PCW += 6; \
	while((length--) != 0) \
	{ \
		WRMEM(to,RDMEM(from)); \
		from++; \
	} \
	pce_cpu->cycle_count-=(6 * length) + 17;

/*
 * TMA
 * Transfer MPR registers to accumulator, highest bit wins.
 */
#define TMA \
	if(arg1 & 0x01) \
		A = pce_cpu->mpr[0]; \
	else if(arg1 & 0x02) \
		A = pce_cpu->mpr[1]; \
	else if(arg1 & 0x04) \
		A = pce_cpu->mpr[2]; \
	else if(arg1 & 0x08) \
		A = pce_cpu->mpr[3]; \
	else if(arg1 & 0x10) \
		A = pce_cpu->mpr[4]; \
	else if(arg1 & 0x20) \
		A = pce_cpu->mpr[5]; \
	else if(arg1 & 0x40) \
		A = pce_cpu->mpr[6]; \
	else if(arg1 & 0x80) \
		A = pce_cpu->mpr[7]

/*
 * TST
 * Test bits.
 */
#define TST \
	P = (P & ~(_pN | _pV | _pT | _pZ)) \
		| ((arg2 & 0x80) ? _pN : 0) \
		| ((arg2 & 0x40) ? _pV : 0) \
		| ((arg2 & arg1) ? 0 : _pZ)

#endif /* __PCE_CPU_OP_H */
// vim: ft=cpp
