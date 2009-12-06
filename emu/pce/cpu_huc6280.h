/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* cpu_huc6280.h: HuC6280 CPU emulator. */

#ifndef __PCE_CPU_HUC6280_H
#define __PCE_CPU_HUC6280_H


/* -------------------------------------------------------------------------- *
 * Data types                                                                 *
 * -------------------------------------------------------------------------- */

/**
 * NEC PCEngine HuC6280 CPU.
 */
typedef struct
{
	/* registers */
	pair ppc;                   /**< previous program counter */
	pair pc;                    /**< program counter */
	pair sp;                    /**< stack pointer */
	pair zp;                    /**< zero page address */
	pair ea;                    /**< effective address */
	uint8 a;                    /**< accumulator */
	uint8 x;                    /**< X index register (general purpose) */
	uint8 y;                    /**< Y index register (general purpose) */
	uint8 p;                    /**< status */
	uint8 mmr[8];               /**< memory mapper */

	/* IRQ */
	uint8 irq_mask;             /**< interrupt enable/disable */
	int extra_cycles;           /**< cycles used taking an interrupt */
	int nmi_state;
	int irq_state[3];
	int (*irq_callback)(int);   /**< IRQ handler callback */

	/* timer */
	int timer_status;           /**< timer status */
	int timer_ack;              /**< timer acknowledge */
	int timer_value;            /**< timer interrupt */
	int timer_load;             /**< reload value */

	/* input port */
	uint8 input_sel;            /**< input port select */
	uint8 input_clr;            /**< input port clear */

	int speed;                  /**< current CPU speed (0:3.58MHz, 1:7.16MHz)*/
	int cycle_count;            /**< cycle count */

} t_pce_cpu;

/* -------------------------------------------------------------------------- *
 * Constants                                                                  *
 * -------------------------------------------------------------------------- */

/*
 * Status register (P) flag indexes.
 */
#define _pC             0x01    /**< carry flag */
#define _pZ             0x02    /**< zero flag */
#define _pI             0x04    /**< interrupt disable flag */
#define _pD             0x08    /**< decimal mode flag */
#define _pB             0x10    /**< break flag */
#define _pT             0x20    /**< accumulator-ish mode (no clue what t means ???) */
#define _pV             0x40    /**< overflow */
#define _pN             0x80    /**< negative */

/*
 * Registry shortcuts.
 */
/* general */
#define A               pce_cpu->a
#define X               pce_cpu->x
#define Y               pce_cpu->y
#define P               pce_cpu->p

/* stack pointer */
#define S               pce_cpu->sp.b.l

/* effective address */
#define EAL             pce_cpu->ea.b.l
#define EAH             pce_cpu->ea.b.h
#define EAW             pce_cpu->ea.w.l
#define EAD             pce_cpu->ea.d

/* zero page */
#define ZPL             pce_cpu->zp.b.l
#define ZPH             pce_cpu->zp.b.h
#define ZPW             pce_cpu->zp.w.l
#define ZPD             pce_cpu->zp.d

/* program counter */
#define PCL             pce_cpu->pc.b.l
#define PCH             pce_cpu->pc.b.h
#define PCW             pce_cpu->pc.w.l
#define PCD             pce_cpu->pc.d

/*
 * IRQs.
 */
#define CLEAR_LINE      0       /**< clear IRQ line */
#define ASSERT_LINE     1       /**< assert IRQ line */

/*
 * Interrupt vectors.
 */
#define INT_VEC_RESET   0xfffe
#define INT_VEC_NMI     0xfffc
#define INT_VEC_TIMER   0xfffa
#define INT_VEC_IRQ1    0xfff8
#define INT_VEC_IRQ2    0xfff6

/**
 * Register names.
 */
/* general registers */
enum
{
	CPU_PC = 1,
	CPU_S,
	CPU_P,
	CPU_A,
	CPU_X,
	CPU_Y,
	CPU_IRQ_MASK,
	CPU_TIMER_STATE,
	CPU_NMI_STATE,
	CPU_IRQ1_STATE,
	CPU_IRQ2_STATE,
	CPU_IRQT_STATE
};

/* offset specified registers */
#define REG_PREVIOUSPC  -1          /**< previous program counter register */
#define REG_SP_CONTENTS -2          /**< stack pointer counter */

/* -------------------------------------------------------------------------- *
 * Globals                                                                    *
 * -------------------------------------------------------------------------- */

extern t_pce_cpu*   pce_cpu;

/* -------------------------------------------------------------------------- *
 * Function prototypes                                                        *
 * -------------------------------------------------------------------------- */

extern int          pce_cpu_init();
extern void         pce_cpu_reset();
extern void         pce_cpu_shutdown();
extern int          pce_cpu_exec(int);

extern unsigned int pce_cpu_reg_r(int);
extern void         pce_cpu_reg_w(int, unsigned int);

extern void         pce_cpu_set_nmi_line(int);
extern void         pce_cpu_set_irq_line(int, int);

extern int          pce_cpu_mem_r(int);
extern void         pce_cpu_mem_w(int, int);

static void         pce_cpu_set_irq_callback(int (*)(int));
static int          pce_cpu_irq_callback(int);

static int          pce_cpu_irq_r(int);
static void         pce_cpu_irq_w(int, int);
static int          pce_cpu_timer_r(int);
static void         pce_cpu_timer_w(int, int);

#endif /* __PCE_CPU_HUC6280_H */
