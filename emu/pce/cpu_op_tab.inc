/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * Copyright © 1998-2000 Juergen Buchmueller                                  *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* cpu_op_tab.inc: HuC6280 CPU opcode function table generator */

#ifndef __PCE_CPU_OP_TAB_INC
#define __PCE_CPU_OP_TAB_INC


#undef OP
/**
 * Generates static inline functions from opcode table rows.
 * \param op            instruction opcode
 */
#define OP(op)                                                               \
	static __inline__ void pce_cpu_op##op(void)

/**
 * Cycles count shortcut (to make table fit to 80 characters).
 */
#define _cycles     pce_cpu->cycle_count

/* -------------------------------------------------------------------------- *
 * Opcode table                                                               *
 * -------------------------------------------------------------------------- */

/*
 * OP   ARGS        CYCLES          READ            OP          WRITE
 */
OP(000) {           _cycles -= 8;                   BRK;                }
OP(020) {           _cycles -= 7;   EA_ABS;         JSR;                }
OP(040) {           _cycles -= 7;                   RTI;           }
OP(060) {           _cycles -= 7;                   RTS;           }
OP(080) { int arg1;                                     BRA(1);       }
OP(0a0) { int arg1; _cycles -= 2;   RD_IMM;         LDY;           }
OP(0c0) { int arg1; _cycles -= 2;   RD_IMM;         CPY;           }
OP(0e0) { int arg1; _cycles -= 2;   RD_IMM;         CPX;           }

OP(010) { int arg1;                                     BPL;           }
OP(030) { int arg1;                                     BMI;           }
OP(050) { int arg1;                                     BVC;           }
OP(070) { int arg1;                                     BVS;           }
OP(090) { int arg1;                                     BCC;           }
OP(0b0) { int arg1;                                     BCS;           }
OP(0d0) { int arg1;                                     BNE;           }
OP(0f0) { int arg1;                                     BEQ;           }

OP(001) { int arg1; _cycles -= 7;   RD_IDX;         ORA;           }
OP(021) { int arg1; _cycles -= 7;   RD_IDX;         AND;           }
OP(041) { int arg1; _cycles -= 7;   RD_IDX;         EOR;           }
OP(061) { int arg1; _cycles -= 7;   RD_IDX;         ADC;           }
OP(081) { int arg1; _cycles -= 7;                   STA;        WR_IDX; }
OP(0a1) { int arg1; _cycles -= 7;   RD_IDX;         LDA;           }
OP(0c1) { int arg1; _cycles -= 7;   RD_IDX;         CMP;           }
OP(0e1) { int arg1; _cycles -= 7;   RD_IDX;         SBC;           }

OP(011) { int arg1; _cycles -= 7;   RD_IDY;         ORA;           }
OP(031) { int arg1; _cycles -= 7;   RD_IDY;         AND;           }
OP(051) { int arg1; _cycles -= 7;   RD_IDY;         EOR;           }
OP(071) { int arg1; _cycles -= 7;   RD_IDY;         ADC;           }
OP(091) { int arg1; _cycles -= 7;                   STA;        WR_IDY; }
OP(0b1) { int arg1; _cycles -= 7;   RD_IDY;         LDA;           }
OP(0d1) { int arg1; _cycles -= 7;   RD_IDY;         CMP;           }
OP(0f1) { int arg1; _cycles -= 7;   RD_IDY;         SBC;           }

OP(002) { int arg1; _cycles -= 3;                   SXY;           }
OP(022) { int arg1; _cycles -= 3;                   SAX;           }
OP(042) { int arg1; _cycles -= 3;                   SAY;           }
OP(062) {           _cycles -= 2;                   CLA;           }
OP(082) {           _cycles -= 2;                   CLX;           }
OP(0a2) { int arg1; _cycles -= 2;   RD_IMM;         LDX;           }
OP(0c2) {           _cycles -= 2;                   CLY;           }
OP(0e2) {                                               ILL;           }

OP(012) { int arg1; _cycles -= 7;   RD_ZPI;         ORA;           }
OP(032) { int arg1; _cycles -= 7;   RD_ZPI;         AND;           }
OP(052) { int arg1; _cycles -= 7;   RD_ZPI;         EOR;           }
OP(072) { int arg1; _cycles -= 7;   RD_ZPI;         ADC;           }
OP(092) { int arg1; _cycles -= 7;                   STA;        WR_ZPI; }
OP(0b2) { int arg1; _cycles -= 7;   RD_ZPI;         LDA;           }
OP(0d2) { int arg1; _cycles -= 7;   RD_ZPI;         CMP;           }
OP(0f2) { int arg1; _cycles -= 7;   RD_ZPI;         SBC;           }

OP(003) { int arg1; _cycles -= 4;   RD_IMM;         ST0;           }
OP(023) { int arg1; _cycles -= 4;   RD_IMM;         ST2;           }
OP(043) { int arg1; _cycles -= 4;   RD_IMM;         TMA;           }
OP(063) {                                               ILL;           }
OP(083) { int arg1;
		  int arg2; _cycles -= 7;   RD_IMM2; RD_ZPG; TST;            }
OP(0a3) { int arg1;
		  int arg2; _cycles -= 7;   RD_IMM2; RD_ZPX; TST;            }
OP(0c3) { int to,from,length;                           TDD;           }
OP(0e3) { int to,from,length,alternate;                 TIA;           }

OP(013) { int arg1; _cycles -= 4;   RD_IMM;         ST1;           }
OP(033) {                                               ILL;           }
OP(053) { int arg1; _cycles -= 5;   RD_IMM;         TAM;           }
OP(073) { int to,from,length;                           TII;           }
OP(093) { int arg1;
		  int arg2; _cycles -= 8;   RD_IMM2; RD_ABS; TST; }
OP(0b3) { int arg1;
		  int arg2; _cycles -= 8;   RD_IMM2; RD_ABX; TST; }
OP(0d3) { int to,from,length;                           TIN;           }
OP(0f3) { int to,from,length,alternate;                 TAI;           }

OP(004) { int arg1; _cycles -= 6;   RD_ZPG;         TSB;        WB_EAZ; }
OP(024) { int arg1; _cycles -= 4;   RD_ZPG;         BIT;           }
OP(044) { int arg1;                                     BSR;           }
OP(064) { int arg1; _cycles -= 4;                   STZ;        WR_ZPG; }
OP(084) { int arg1; _cycles -= 4;                   STY;        WR_ZPG; }
OP(0a4) { int arg1; _cycles -= 4;   RD_ZPG;         LDY;           }
OP(0c4) { int arg1; _cycles -= 4;   RD_ZPG;         CPY;           }
OP(0e4) { int arg1; _cycles -= 4;   RD_ZPG;         CPX;           }

OP(014) { int arg1; _cycles -= 6;   RD_ZPG;         TRB;        WB_EAZ; }
OP(034) { int arg1; _cycles -= 4;   RD_ZPX;         BIT;           }
OP(054) {           _cycles -= 2;                   CSL;         }
OP(074) { int arg1; _cycles -= 4;                   STZ;        WR_ZPX; }
OP(094) { int arg1; _cycles -= 4;                   STY;        WR_ZPX; }
OP(0b4) { int arg1; _cycles -= 4;   RD_ZPX;         LDY;           }
OP(0d4) {           _cycles -= 2;                   CSH;         }
OP(0f4) {           _cycles -= 2;                   SET;           }

OP(005) { int arg1; _cycles -= 4;   RD_ZPG;         ORA;           }
OP(025) { int arg1; _cycles -= 4;   RD_ZPG;         AND;           }
OP(045) { int arg1; _cycles -= 4;   RD_ZPG;         EOR;           }
OP(065) { int arg1; _cycles -= 4;   RD_ZPG;         ADC;           }
OP(085) { int arg1; _cycles -= 4;                   STA;        WR_ZPG; }
OP(0a5) { int arg1; _cycles -= 4;   RD_ZPG;         LDA;           }
OP(0c5) { int arg1; _cycles -= 4;   RD_ZPG;         CMP;           }
OP(0e5) { int arg1; _cycles -= 4;   RD_ZPG;         SBC;           }

OP(015) { int arg1; _cycles -= 4;   RD_ZPX;         ORA;           }
OP(035) { int arg1; _cycles -= 4;   RD_ZPX;         AND;           }
OP(055) { int arg1; _cycles -= 4;   RD_ZPX;         EOR;           }
OP(075) { int arg1; _cycles -= 4;   RD_ZPX;         ADC;           }
OP(095) { int arg1; _cycles -= 4;                   STA;        WR_ZPX; }
OP(0b5) { int arg1; _cycles -= 4;   RD_ZPX;         LDA;           }
OP(0d5) { int arg1; _cycles -= 4;   RD_ZPX;         CMP;           }
OP(0f5) { int arg1; _cycles -= 4;   RD_ZPX;         SBC;           }

OP(006) { int arg1; _cycles -= 6;   RD_ZPG;         ASL;        WB_EAZ; }
OP(026) { int arg1; _cycles -= 6;   RD_ZPG;         ROL;        WB_EAZ; }
OP(046) { int arg1; _cycles -= 6;   RD_ZPG;         LSR;        WB_EAZ; }
OP(066) { int arg1; _cycles -= 6;   RD_ZPG;         ROR;        WB_EAZ; }
OP(086) { int arg1; _cycles -= 4;                   STX;        WR_ZPG; }
OP(0a6) { int arg1; _cycles -= 4;   RD_ZPG;         LDX;           }
OP(0c6) { int arg1; _cycles -= 6;   RD_ZPG;         DEC;        WB_EAZ; }
OP(0e6) { int arg1; _cycles -= 6;   RD_ZPG;         INC;        WB_EAZ; }

OP(016) { int arg1; _cycles -= 6; RD_ZPX; ASL; WB_EAZ  }
OP(036) { int arg1; _cycles -= 6; RD_ZPX; ROL; WB_EAZ  }
OP(056) { int arg1; _cycles -= 6; RD_ZPX; LSR; WB_EAZ  }
OP(076) { int arg1; _cycles -= 6; RD_ZPX; ROR; WB_EAZ  }
OP(096) { int arg1; _cycles -= 4;          STX; WR_ZPY; }
OP(0b6) { int arg1; _cycles -= 4; RD_ZPY; LDX;           }
OP(0d6) { int arg1; _cycles -= 6; RD_ZPX; DEC; WB_EAZ; }
OP(0f6) { int arg1; _cycles -= 6; RD_ZPX; INC; WB_EAZ; }

OP(007) { int arg1; _cycles -= 7; RD_ZPG; RMB(0);WB_EAZ;}
OP(027) { int arg1; _cycles -= 7; RD_ZPG; RMB(2);WB_EAZ;}
OP(047) { int arg1; _cycles -= 7; RD_ZPG; RMB(4);WB_EAZ;}
OP(067) { int arg1; _cycles -= 7; RD_ZPG; RMB(6);WB_EAZ;}
OP(087) { int arg1; _cycles -= 7; RD_ZPG; SMB(0);WB_EAZ;}
OP(0a7) { int arg1; _cycles -= 7; RD_ZPG; SMB(2);WB_EAZ;}
OP(0c7) { int arg1; _cycles -= 7; RD_ZPG; SMB(4);WB_EAZ;}
OP(0e7) { int arg1; _cycles -= 7; RD_ZPG; SMB(6);WB_EAZ;}

OP(017) { int arg1; _cycles -= 7; RD_ZPG; RMB(1);WB_EAZ;}
OP(037) { int arg1; _cycles -= 7; RD_ZPG; RMB(3);WB_EAZ;}
OP(057) { int arg1; _cycles -= 7; RD_ZPG; RMB(5);WB_EAZ;}
OP(077) { int arg1; _cycles -= 7; RD_ZPG; RMB(7);WB_EAZ;}
OP(097) { int arg1; _cycles -= 7; RD_ZPG; SMB(1);WB_EAZ;}
OP(0b7) { int arg1; _cycles -= 7; RD_ZPG; SMB(3);WB_EAZ;}
OP(0d7) { int arg1; _cycles -= 7; RD_ZPG; SMB(5);WB_EAZ;}
OP(0f7) { int arg1; _cycles -= 7; RD_ZPG; SMB(7);WB_EAZ;}

OP(008) {           _cycles -= 3;          PHP;           }
OP(028) {           _cycles -= 4;          PLP;           }
OP(048) {           _cycles -= 3;          PHA;           }
OP(068) {           _cycles -= 4;          PLA;           }
OP(088) {           _cycles -= 2;          DEY;           }
OP(0a8) {           _cycles -= 2;          TAY;           }
OP(0c8) {           _cycles -= 2;          INY;           }
OP(0e8) {           _cycles -= 2;          INX;           }

OP(018) {           _cycles -= 2;          CLC;           }
OP(038) {           _cycles -= 2;          SEC;           }
OP(058) {           _cycles -= 2;          CLI;           }
OP(078) {           _cycles -= 2;          SEI;           }
OP(098) {           _cycles -= 2;          TYA;           }
OP(0b8) {           _cycles -= 2;          CLV;           }
OP(0d8) {           _cycles -= 2;          CLD;           }
OP(0f8) {           _cycles -= 2;          SED;           }

OP(009) { int arg1; _cycles -= 2; RD_IMM; ORA;           }
OP(029) { int arg1; _cycles -= 2; RD_IMM; AND;           }
OP(049) { int arg1; _cycles -= 2; RD_IMM; EOR;           }
OP(069) { int arg1; _cycles -= 2; RD_IMM; ADC;           }
OP(089) { int arg1; _cycles -= 2; RD_IMM; BIT;           }
OP(0a9) { int arg1; _cycles -= 2; RD_IMM; LDA;           }
OP(0c9) { int arg1; _cycles -= 2; RD_IMM; CMP;           }
OP(0e9) { int arg1; _cycles -= 2; RD_IMM; SBC;           }

OP(019) { int arg1; _cycles -= 5; RD_ABY; ORA;           }
OP(039) { int arg1; _cycles -= 5; RD_ABY; AND;           }
OP(059) { int arg1; _cycles -= 5; RD_ABY; EOR;           }
OP(079) { int arg1; _cycles -= 5; RD_ABY; ADC;           }
OP(099) { int arg1; _cycles -= 5;          STA; WR_ABY; }
OP(0b9) { int arg1; _cycles -= 5; RD_ABY; LDA;           }
OP(0d9) { int arg1; _cycles -= 5; RD_ABY; CMP;           }
OP(0f9) { int arg1; _cycles -= 5; RD_ABY; SBC;           }

OP(00a) { int arg1; _cycles -= 2; RD_ACC; ASL; WB_ACC; }
OP(02a) { int arg1; _cycles -= 2; RD_ACC; ROL; WB_ACC; }
OP(04a) { int arg1; _cycles -= 2; RD_ACC; LSR; WB_ACC; }
OP(06a) { int arg1; _cycles -= 2; RD_ACC; ROR; WB_ACC; }
OP(08a) {           _cycles -= 2;          TXA;           }
OP(0aa) {           _cycles -= 2;          TAX;           }
OP(0ca) {           _cycles -= 2;          DEX;           }
OP(0ea) {           _cycles -= 2;          NOP;           }

OP(01a) {           _cycles -= 2;          INA;           }
OP(03a) {           _cycles -= 2;          DEA;           }
OP(05a) {           _cycles -= 3;          PHY;           }
OP(07a) {           _cycles -= 4;          PLY;           }
OP(09a) {           _cycles -= 2;          TXS;           }
OP(0ba) {           _cycles -= 2;          TSX;           }
OP(0da) {           _cycles -= 3;          PHX;           }
OP(0fa) {           _cycles -= 4;          PLX;           }

OP(00b) {                                      ILL;           }
OP(02b) {                                      ILL;           }
OP(04b) {                                      ILL;           }
OP(06b) {                                      ILL;           }
OP(08b) {                                      ILL;           }
OP(0ab) {                                      ILL;           }
OP(0cb) {                                      ILL;           }
OP(0eb) {                                      ILL;           }

OP(01b) {                                      ILL;           }
OP(03b) {                                      ILL;           }
OP(05b) {                                      ILL;           }
OP(07b) {                                      ILL;           }
OP(09b) {                                      ILL;           }
OP(0bb) {                                      ILL;           }
OP(0db) {                                      ILL;           }
OP(0fb) {                                      ILL;           }

OP(00c) { int arg1; _cycles -= 7; RD_ABS; TSB; WB_EA;  }
OP(02c) { int arg1; _cycles -= 5; RD_ABS; BIT;           }
OP(04c) {           _cycles -= 4; EA_ABS; JMP;           }
OP(06c) { int arg1; _cycles -= 7; EA_IND; JMP;           }
OP(08c) { int arg1; _cycles -= 5;          STY; WR_ABS; }
OP(0ac) { int arg1; _cycles -= 5; RD_ABS; LDY;           }
OP(0cc) { int arg1; _cycles -= 5; RD_ABS; CPY;           }
OP(0ec) { int arg1; _cycles -= 5; RD_ABS; CPX;           }

OP(01c) { int arg1; _cycles -= 7; RD_ABS; TRB; WB_EA;  }
OP(03c) { int arg1; _cycles -= 5; RD_ABX; BIT;           }
OP(05c) {                                      ILL;           }
OP(07c) { int arg1; _cycles -= 7; EA_IAX; JMP;           }
OP(09c) { int arg1; _cycles -= 5;          STZ; WR_ABS; }
OP(0bc) { int arg1; _cycles -= 5; RD_ABX; LDY;           }
OP(0dc) {                                      ILL;           }
OP(0fc) {                                      ILL;           }

OP(00d) { int arg1; _cycles -= 5; RD_ABS; ORA;           }
OP(02d) { int arg1; _cycles -= 5; RD_ABS; AND;           }
OP(04d) { int arg1; _cycles -= 5; RD_ABS; EOR;           }
OP(06d) { int arg1; _cycles -= 5; RD_ABS; ADC;           }
OP(08d) { int arg1; _cycles -= 5;          STA; WR_ABS; }
OP(0ad) { int arg1; _cycles -= 5; RD_ABS; LDA;           }
OP(0cd) { int arg1; _cycles -= 5; RD_ABS; CMP;           }
OP(0ed) { int arg1; _cycles -= 5; RD_ABS; SBC;           }

OP(01d) { int arg1; _cycles -= 5; RD_ABX; ORA;           }
OP(03d) { int arg1; _cycles -= 5; RD_ABX; AND;           }
OP(05d) { int arg1; _cycles -= 5; RD_ABX; EOR;           }
OP(07d) { int arg1; _cycles -= 5; RD_ABX; ADC;           }
OP(09d) { int arg1; _cycles -= 5;          STA; WR_ABX; }
OP(0bd) { int arg1; _cycles -= 5; RD_ABX; LDA;           }
OP(0dd) { int arg1; _cycles -= 5; RD_ABX; CMP;           }
OP(0fd) { int arg1; _cycles -= 5; RD_ABX; SBC;           }

OP(00e) { int arg1; _cycles -= 7; RD_ABS; ASL; WB_EA;  }
OP(02e) { int arg1; _cycles -= 7; RD_ABS; ROL; WB_EA;  }
OP(04e) { int arg1; _cycles -= 7; RD_ABS; LSR; WB_EA;  }
OP(06e) { int arg1; _cycles -= 7; RD_ABS; ROR; WB_EA;  }
OP(08e) { int arg1; _cycles -= 5;          STX; WR_ABS; }
OP(0ae) { int arg1; _cycles -= 5; RD_ABS; LDX;           }
OP(0ce) { int arg1; _cycles -= 7; RD_ABS; DEC; WB_EA;  }
OP(0ee) { int arg1; _cycles -= 7; RD_ABS; INC; WB_EA;  }

OP(01e) { int arg1; _cycles -= 7; RD_ABX; ASL; WB_EA;  }
OP(03e) { int arg1; _cycles -= 7; RD_ABX; ROL; WB_EA;  }
OP(05e) { int arg1; _cycles -= 7; RD_ABX; LSR; WB_EA;  }
OP(07e) { int arg1; _cycles -= 7; RD_ABX; ROR; WB_EA;  }
OP(09e) { int arg1; _cycles -= 5;          STZ; WR_ABX; }
OP(0be) { int arg1; _cycles -= 5; RD_ABY; LDX;           }
OP(0de) { int arg1; _cycles -= 7; RD_ABX; DEC; WB_EA;  }
OP(0fe) { int arg1; _cycles -= 7; RD_ABX; INC; WB_EA;  }

OP(00f) { int arg1; _cycles -= 4; RD_ZPG; BBR(0);       }
OP(02f) { int arg1; _cycles -= 4; RD_ZPG; BBR(2);       }
OP(04f) { int arg1; _cycles -= 4; RD_ZPG; BBR(4);       }
OP(06f) { int arg1; _cycles -= 4; RD_ZPG; BBR(6);       }
OP(08f) { int arg1; _cycles -= 4; RD_ZPG; BBS(0);       }
OP(0af) { int arg1; _cycles -= 4; RD_ZPG; BBS(2);       }
OP(0cf) { int arg1; _cycles -= 4; RD_ZPG; BBS(4);       }
OP(0ef) { int arg1; _cycles -= 4; RD_ZPG; BBS(6);       }

OP(01f) { int arg1; _cycles -= 4; RD_ZPG; BBR(1);       }
OP(03f) { int arg1; _cycles -= 4; RD_ZPG; BBR(3);       }
OP(05f) { int arg1; _cycles -= 4; RD_ZPG; BBR(5);       }
OP(07f) { int arg1; _cycles -= 4; RD_ZPG; BBR(7);       }
OP(09f) { int arg1; _cycles -= 4; RD_ZPG; BBS(1);       }
OP(0bf) { int arg1; _cycles -= 4; RD_ZPG; BBS(3);       }
OP(0df) { int arg1; _cycles -= 4; RD_ZPG; BBS(5);       }
OP(0ff) { int arg1; _cycles -= 4; RD_ZPG; BBS(7);       }

/**
 * Array of pointers to all opcode functions indexed by opcode.
 * NOTE: this is autogenerated stub.
 */
static void (*pce_cpu_op[0x100])() = 
{
	pce_cpu_op000, pce_cpu_op001, pce_cpu_op002, pce_cpu_op003,
	pce_cpu_op004, pce_cpu_op005, pce_cpu_op006, pce_cpu_op007,
	pce_cpu_op008, pce_cpu_op009, pce_cpu_op00a, pce_cpu_op00b,
	pce_cpu_op00c, pce_cpu_op00d, pce_cpu_op00e, pce_cpu_op00f,
	pce_cpu_op010, pce_cpu_op011, pce_cpu_op012, pce_cpu_op013,
	pce_cpu_op014, pce_cpu_op015, pce_cpu_op016, pce_cpu_op017,
	pce_cpu_op018, pce_cpu_op019, pce_cpu_op01a, pce_cpu_op01b,
	pce_cpu_op01c, pce_cpu_op01d, pce_cpu_op01e, pce_cpu_op01f,
	pce_cpu_op020, pce_cpu_op021, pce_cpu_op022, pce_cpu_op023,
	pce_cpu_op024, pce_cpu_op025, pce_cpu_op026, pce_cpu_op027,
	pce_cpu_op028, pce_cpu_op029, pce_cpu_op02a, pce_cpu_op02b,
	pce_cpu_op02c, pce_cpu_op02d, pce_cpu_op02e, pce_cpu_op02f,
	pce_cpu_op030, pce_cpu_op031, pce_cpu_op032, pce_cpu_op033,
	pce_cpu_op034, pce_cpu_op035, pce_cpu_op036, pce_cpu_op037,
	pce_cpu_op038, pce_cpu_op039, pce_cpu_op03a, pce_cpu_op03b,
	pce_cpu_op03c, pce_cpu_op03d, pce_cpu_op03e, pce_cpu_op03f,
	pce_cpu_op040, pce_cpu_op041, pce_cpu_op042, pce_cpu_op043,
	pce_cpu_op044, pce_cpu_op045, pce_cpu_op046, pce_cpu_op047,
	pce_cpu_op048, pce_cpu_op049, pce_cpu_op04a, pce_cpu_op04b,
	pce_cpu_op04c, pce_cpu_op04d, pce_cpu_op04e, pce_cpu_op04f,
	pce_cpu_op050, pce_cpu_op051, pce_cpu_op052, pce_cpu_op053,
	pce_cpu_op054, pce_cpu_op055, pce_cpu_op056, pce_cpu_op057,
	pce_cpu_op058, pce_cpu_op059, pce_cpu_op05a, pce_cpu_op05b,
	pce_cpu_op05c, pce_cpu_op05d, pce_cpu_op05e, pce_cpu_op05f,
	pce_cpu_op060, pce_cpu_op061, pce_cpu_op062, pce_cpu_op063,
	pce_cpu_op064, pce_cpu_op065, pce_cpu_op066, pce_cpu_op067,
	pce_cpu_op068, pce_cpu_op069, pce_cpu_op06a, pce_cpu_op06b,
	pce_cpu_op06c, pce_cpu_op06d, pce_cpu_op06e, pce_cpu_op06f,
	pce_cpu_op070, pce_cpu_op071, pce_cpu_op072, pce_cpu_op073,
	pce_cpu_op074, pce_cpu_op075, pce_cpu_op076, pce_cpu_op077,
	pce_cpu_op078, pce_cpu_op079, pce_cpu_op07a, pce_cpu_op07b,
	pce_cpu_op07c, pce_cpu_op07d, pce_cpu_op07e, pce_cpu_op07f,
	pce_cpu_op080, pce_cpu_op081, pce_cpu_op082, pce_cpu_op083,
	pce_cpu_op084, pce_cpu_op085, pce_cpu_op086, pce_cpu_op087,
	pce_cpu_op088, pce_cpu_op089, pce_cpu_op08a, pce_cpu_op08b,
	pce_cpu_op08c, pce_cpu_op08d, pce_cpu_op08e, pce_cpu_op08f,
	pce_cpu_op090, pce_cpu_op091, pce_cpu_op092, pce_cpu_op093,
	pce_cpu_op094, pce_cpu_op095, pce_cpu_op096, pce_cpu_op097,
	pce_cpu_op098, pce_cpu_op099, pce_cpu_op09a, pce_cpu_op09b,
	pce_cpu_op09c, pce_cpu_op09d, pce_cpu_op09e, pce_cpu_op09f,
	pce_cpu_op0a0, pce_cpu_op0a1, pce_cpu_op0a2, pce_cpu_op0a3,
	pce_cpu_op0a4, pce_cpu_op0a5, pce_cpu_op0a6, pce_cpu_op0a7,
	pce_cpu_op0a8, pce_cpu_op0a9, pce_cpu_op0aa, pce_cpu_op0ab,
	pce_cpu_op0ac, pce_cpu_op0ad, pce_cpu_op0ae, pce_cpu_op0af,
	pce_cpu_op0b0, pce_cpu_op0b1, pce_cpu_op0b2, pce_cpu_op0b3,
	pce_cpu_op0b4, pce_cpu_op0b5, pce_cpu_op0b6, pce_cpu_op0b7,
	pce_cpu_op0b8, pce_cpu_op0b9, pce_cpu_op0ba, pce_cpu_op0bb,
	pce_cpu_op0bc, pce_cpu_op0bd, pce_cpu_op0be, pce_cpu_op0bf,
	pce_cpu_op0c0, pce_cpu_op0c1, pce_cpu_op0c2, pce_cpu_op0c3,
	pce_cpu_op0c4, pce_cpu_op0c5, pce_cpu_op0c6, pce_cpu_op0c7,
	pce_cpu_op0c8, pce_cpu_op0c9, pce_cpu_op0ca, pce_cpu_op0cb,
	pce_cpu_op0cc, pce_cpu_op0cd, pce_cpu_op0ce, pce_cpu_op0cf,
	pce_cpu_op0d0, pce_cpu_op0d1, pce_cpu_op0d2, pce_cpu_op0d3,
	pce_cpu_op0d4, pce_cpu_op0d5, pce_cpu_op0d6, pce_cpu_op0d7,
	pce_cpu_op0d8, pce_cpu_op0d9, pce_cpu_op0da, pce_cpu_op0db,
	pce_cpu_op0dc, pce_cpu_op0dd, pce_cpu_op0de, pce_cpu_op0df,
	pce_cpu_op0e0, pce_cpu_op0e1, pce_cpu_op0e2, pce_cpu_op0e3,
	pce_cpu_op0e4, pce_cpu_op0e5, pce_cpu_op0e6, pce_cpu_op0e7,
	pce_cpu_op0e8, pce_cpu_op0e9, pce_cpu_op0ea, pce_cpu_op0eb,
	pce_cpu_op0ec, pce_cpu_op0ed, pce_cpu_op0ee, pce_cpu_op0ef,
	pce_cpu_op0f0, pce_cpu_op0f1, pce_cpu_op0f2, pce_cpu_op0f3,
	pce_cpu_op0f4, pce_cpu_op0f5, pce_cpu_op0f6, pce_cpu_op0f7,
	pce_cpu_op0f8, pce_cpu_op0f9, pce_cpu_op0fa, pce_cpu_op0fb,
	pce_cpu_op0fc, pce_cpu_op0fd, pce_cpu_op0fe, pce_cpu_op0ff
};

#endif /* __PCE_CPU_OP_TAB_INC */
// vim: ft=cpp
