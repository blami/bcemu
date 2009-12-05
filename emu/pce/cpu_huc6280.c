/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* cpu_huc6280.c: NEC PCEngine HuC6280 CPU emulator */

#include "bc_emu.h"
#include "emu/pce/pce_main.h"
#include "emu/pce/cpu_huc6280.h"
#include "emu/pce/vce_huc6260.h"
#include "emu/pce/vdc_huc6270.h"
#include "emu/pce/psg.h"
#include "emu/pce/rom.h"


t_pce_cpu* pce_cpu = NULL;

/* include instruction set + opcode table */
#include "cpu_instr_util.inc"
#include "cpu_instr.inc"
#include "cpu_op_tab.inc"

/* -------------------------------------------------------------------------- *
 * Init, shutdown, reset routines                                             *
 * -------------------------------------------------------------------------- */

/**
 * Initialize CPU.
 */
int pce_cpu_init()
{
	debug("CPU init");
	assert(pce);

	pce_cpu = xmalloc(sizeof(t_pce_cpu));
	pce_cpu_reset();

	return 1;
}

/**
 * Reset CPU to initial state.
 */
void pce_cpu_reset()
{
	int i;

	debug("CPU reset");
	assert(pce_cpu);

	memset(pce_cpu, 0, sizeof(t_pce_cpu));

	/* setup status flag register */
	pce_cpu->p = _pI | _pZ;
	/* setup stack pointer */
	pce_cpu->sp.d = 0x1ff;

	PCL = RDMEM(INT_VEC_RESET);
	PCH = RDMEM((INT_VEC_RESET+1));

	/* setup timer */
	pce_cpu->timer_status = 0;
	pce_cpu->timer_ack = 1;

	/* set all IRQ states to CLEAR_LINE */
	for (i = 0; i < 3; i++)
		pce_cpu->irq_state[i] = CLEAR_LINE;

	/* set speed to 7.16MHz */
	pce_cpu->speed = 1;

	pce_cpu->cycle_count = 0;

	/* set interrupt callback */
	pce_cpu_set_irq_callback(&pce_cpu_irq_callback);
}

/**
 * Shutdown CPU.
 */
void pce_cpu_shutdown()
{
	debug("CPU shutdown");
	assert(pce_cpu);

	/* cleanup emu_pce_cpu */
	xfree(pce_cpu);
}

/* -------------------------------------------------------------------------- *
 * CPU                                                                        *
 * -------------------------------------------------------------------------- */

/**
 * Exec next n cycles (455 cycles makes one frame in NTSC in
 * pce_cpu->speed == 1 mode).
 * \param cycles    instruction cycles avalaible to exec
 * \return          remaining cycles
 */
int pce_cpu_exec(int cycles)
{
	int in, last, delta;

	//debug("CPU exec cycles=%d", cycles);
	assert(pce && pce_cpu);

	/* store cycles count */
	pce_cpu->cycle_count = cycles;

	/* remove extra cycles taken by irq */
	pce_cpu->cycle_count -= pce_cpu->extra_cycles;
	pce_cpu->extra_cycles = 0;

	last = pce_cpu->cycle_count;

	/* process instructions */
	do
	{
		/* update previous program counter value */
		pce_cpu->ppc = pce_cpu->pc;

		/* read and execute opcode */
		in = RDOP();

		/* TODO this is the right place for debugger hook */
		//debug("CPU cycle: %d", pce_cpu->cycle_count);
		/*debug("CPU state: PC=%04x P=%04x S=%04x A=%04x X=%04x Y=%04x",
			pce_cpu_reg_r(CPU_PC),
			pce_cpu_reg_r(CPU_P),
			pce_cpu_reg_r(CPU_S),
			pce_cpu_reg_r(CPU_A),
			pce_cpu_reg_r(CPU_X),
			pce_cpu_reg_r(CPU_Y));*/
		//debug("CPU opcode: %03x", in);

		PCW++;
		pce_cpu_op[in]();

		/* process timer */
		if(pce_cpu->timer_status)
		{
			delta = last - pce_cpu->cycle_count;
			pce_cpu->timer_value -= delta;

			/* cause IRQ if needed */
			if(pce_cpu->timer_value <= 0 && pce_cpu->timer_ack == 1)
			{
				pce_cpu->timer_status = 0;
				pce_cpu->timer_ack = 0;

				pce_cpu_set_irq_line(2,ASSERT_LINE);
			}
		}
		last = pce_cpu->cycle_count;

		if(pce_cpu->pc.d == pce_cpu->ppc.d)
		{
			if(pce_cpu->cycle_count > 0)
				pce_cpu->cycle_count = 0;
			pce_cpu->extra_cycles = 0;

			return cycles;
		}

	} while(pce_cpu->cycle_count > 0);

	/* cycles taken by irqs */
	pce_cpu->cycle_count -= pce_cpu->extra_cycles;
	pce_cpu->extra_cycles = 0;

	return cycles - pce_cpu->cycle_count;
}

/**
 * Get named register content.
 * \param name      register name (use CPU_ constants)
 * \return          register content
 */
unsigned int pce_cpu_reg_r(int name)
{
	assert(pce && pce_cpu);

	switch(name)
	{
		/* general */
		case CPU_PC:
			return PCD;
		case CPU_S:
			return S;
		case CPU_P:
			return P;
		case CPU_A:
			return A;
		case CPU_X:
			return X;
		case CPU_Y:
			return Y;
		case CPU_IRQ_MASK:
			return pce_cpu->irq_mask;
		case CPU_TIMER_STATE:
			return pce_cpu->timer_status;
		case CPU_NMI_STATE:
			return pce_cpu->nmi_state;
		case CPU_IRQ1_STATE:
			return pce_cpu->irq_state[0];
		case CPU_IRQ2_STATE:
			return pce_cpu->irq_state[1];
		case CPU_IRQT_STATE:
			return pce_cpu->irq_state[2];
		
		/* offset */
		case REG_PREVIOUSPC:
			return pce_cpu->ppc.d;
		default:
			if(name <= REG_SP_CONTENTS)
			{
				unsigned int offset = S + 2 * (REG_SP_CONTENTS - name);
				debug("offset register RDMEM: %x", offset);

				if(offset < 0x1ff)
					return RDMEM(offset) | (RDMEM(offset+1) << 8);
			}
	}

	debug("couldn't get register=%d", name);
	return 0;
}

/**
 * Set named register content to value.
 * \param name      register name (use _CPU constants)
 * \param value     value to be stored in register
 */
void pce_cpu_reg_w(int name, unsigned int value)
{
	assert(pce_cpu);

	switch(name)
	{
		/* general */
		case CPU_PC:
			PCW = value;
			break;
		case CPU_S:
			S = value;
			break;
		case CPU_P:
			P = value;
			break;
		case CPU_A:
			A = value;
			break;
		case CPU_X:
			X = value;
			break;
		case CPU_Y:
			Y = value;
			break;
		case CPU_IRQ_MASK:
			pce_cpu->irq_mask = value;
			CHECK_IRQ_LINES;
			break;
		case CPU_TIMER_STATE:
			pce_cpu->timer_status = value;
			break;
		case CPU_NMI_STATE:
			pce_cpu_set_nmi_line(value);
			break;
		case CPU_IRQ1_STATE:
			pce_cpu_set_irq_line(0, value);
			break;
		case CPU_IRQ2_STATE:
			pce_cpu_set_irq_line(1, value);
			break;
		case CPU_IRQT_STATE:
			pce_cpu_set_irq_line(2, value);
			break;

		/* offset */
		default:
			if(name <= REG_SP_CONTENTS)
			{
				unsigned int offset = S + 2 * (REG_SP_CONTENTS - name);

				debug("offset register WRMEM: %x", offset);
				if(offset < 0x1ff)
				{
					WRMEM(offset, value & 0xff);
					WRMEM(offset+1, (value >> 8) & 0xff);
				}
			}
			else
			{
				debug("couldn't set register=%d", name);
			}
	}
}

/**
 * Set NMI (non-maskable interrupt) line state.
 * \param state         new NMI line state
 */
void pce_cpu_set_nmi_line(int state)
{
	assert(pce_cpu);

	if(pce_cpu->nmi_state == state)
		return;

	pce_cpu->nmi_state = state;

	if(state != CLEAR_LINE)
	{
		DO_INTERRUPT(INT_VEC_NMI);
	}
}

/**
 * Set IRQ (interrupt request) line irqline state.
 * \param irqline       IRQ line selector
 * \param state         new IRQ line state
 */
void pce_cpu_set_irq_line(int irqline, int state)
{
	assert(pce_cpu);

	pce_cpu->irq_state[irqline] = state;

	if(state == CLEAR_LINE)
		return;

	/* IRQ lines check */
	CHECK_IRQ_LINES;
}

/**
 * Set IRQ callback function pointer.
 * \param callback      callback to function which handles IRQs
 */
static void pce_cpu_set_irq_callback(int (*callback)(int irqline))
{
	assert(pce_cpu);

	pce_cpu->irq_callback = callback;
}

/**
 * Default IRQ callback. Effective only for lines IRQ1 and IRQ2 (not timer).
 * \param irqline       IRQ line where request happened
 */
static int pce_cpu_irq_callback(int irqline)
{
	debug("CPU irq callback: IRQ%d (line=%d)", irqline+1, irqline);
	return 0;
}

/**
 * Read IRQ status.
 * \param offset        offset (0=IRQ mask, 1=IRQ status)
 * \return              IRQ status
 */
static int pce_cpu_irq_r(int offset)
{
	int status;

	assert(pce_cpu);

	switch(offset)
	{
		case 0:
			return pce_cpu->irq_mask;
		case 1:
			status=0;
			if(pce_cpu->irq_state[1] != CLEAR_LINE) status |= 1; /* IRQ 2 */
			if(pce_cpu->irq_state[0] != CLEAR_LINE) status |= 2; /* IRQ 1 */
			if(pce_cpu->irq_state[2] != CLEAR_LINE) status |= 4; /* TIMER */
			return status;
	}

	return 0;
}

/**
 * Write IRQ status.
 * \param offset        offset (0=IRQ mask, 1=IRQ status)
 * \param data          data to be written
 */
static void pce_cpu_irq_w(int offset, int data)
{
	assert(pce_cpu);

	switch(offset)
	{
		case 0:
			pce_cpu->irq_mask = data & 0x7;
			CHECK_IRQ_LINES;
			break;
		case 1:
			pce_cpu->timer_value = pce_cpu->timer_load;
			pce_cpu->timer_ack = 1;
			break;
	}
}

/**
 * Read timer.
 * \param offset        offset (0=timer value, 1=timer status)
 */
static int pce_cpu_timer_r(int offset)
{
	assert(pce_cpu);

	switch(offset) 
	{
		case 0:
			return (pce_cpu->timer_value / 1024) & 127;
		case 1:
			return pce_cpu->timer_status;
	}

	return 0;
}

/**
 * Write timer.
 * \param offset        offset (0=timer value, 1=timer status)
 * \param data          data to be written
 */
static void pce_cpu_timer_w(int offset, int data)
{
	assert(pce_cpu);

	switch(offset)
	{
		case 0:
			pce_cpu->timer_load = pce_cpu->timer_value =
				((data & 127) + 1) * 1024;
			return;
		case 1:
			if(data&1)
			{
				if(pce_cpu->timer_status==0)
					pce_cpu->timer_value = pce_cpu->timer_load;
			}
			pce_cpu->timer_status=data&1;
			return;
	}
}

/**
 * Input port write.
 * FIXME
 * \param data          data to be written to input port
 */
static void pce_cpu_input_w(uint8 data)
{
	/*
	joy_sel = (data & 1);
	joy_clr = (data >> 1) & 1;
	*/
}

/**
 * Input port read.
 * FIXME
 * \return              data read from input port
 */
static uint8 pce_cpu_input_r()
{
	uint8 temp = 0xFF;

	/* decode data and setup masks */
	/*
	if(input.pad[joy_cnt] & INPUT_LEFT)   temp &= ~0x80;
	if(input.pad[joy_cnt] & INPUT_DOWN)   temp &= ~0x40;
	if(input.pad[joy_cnt] & INPUT_RIGHT)  temp &= ~0x20;
	if(input.pad[joy_cnt] & INPUT_UP)     temp &= ~0x10;
	if(input.pad[joy_cnt] & INPUT_RUN)    temp &= ~0x08;
	if(input.pad[joy_cnt] & INPUT_SELECT) temp &= ~0x04;
	if(input.pad[joy_cnt] & INPUT_B2)     temp &= ~0x02;
	if(input.pad[joy_cnt] & INPUT_B1)     temp &= ~0x01;

	if(joy_sel & 1) temp >>= 4;
	temp &= 0x0F;
	*/
	return temp;
}


/**
 * Internal read I/O page (CPU peripherals) address decision logic.
 * \param addr      page address
 * \return          page address content
 */
static int pce_cpu_iopage_r(int addr)
{
	assert(pce && pce_cpu);

	//debug("CPU iopage/read: addr=%08x", addr);

	switch(addr & 0x1C00)
	{
		case 0x0000: /* 0x0000-0x0003 VDC */
			if(addr <= 0x0003)
				return pce_vdc_r(addr);
			break;
		case 0x0400: /* 0x0400-0x0405 VCE */
			if(addr <= 0x0405)
				return pce_vce_r(addr);
			break;
		case 0x0800: /* 0x0800-0x0809 PSG */
			break;
		case 0x0C00: /* 0x0C00-0x0C01 timer */
			if(addr == 0x0C00 || addr == 0x0C01)
				return pce_cpu_timer_r(addr & 1);
			break;
		case 0x1000: /* 0x1000 input port */
			if(addr == 0x1000)
				return pce_cpu_input_r();
			break;
		case 0x1400: /* 0x1402-0x1403 IRQ */
			if(addr == 0x1402 || addr == 0x1403)
				return pce_cpu_irq_r(addr & 1);
			break;
	}

	debug("CPU UNKNOWN iopage/read %04X (PC:%08X)", addr,
		pce_cpu_reg_r(CPU_PC));
	return (0x00);
}

/**
 * Internal write I/O page (CPU peripherals) address decision logic.
 * \param addr          page address
 * \param data          data to be written
 * \see pce_cpu_mem_w
 */
static void pce_cpu_iopage_w(int addr, int data)
{
	assert(pce_cpu);

	//debug("CPU iopage/write: addr=%08x data=%d", addr, data);

	switch(addr & 0x1C00)
	{
		case 0x0000: /* 0x0000-0x0003 VDC */
			if(addr <= 0x0003)
			{
				pce_vdc_w(addr, data);
				return;
			}
			break;
		case 0x0400: /* 0x0400-0x0405 VCE */
			if(addr <= 0x0405)
			{
				pce_vce_w(addr, data);
				return;
			}
			break;
		case 0x0800: /* 0x0800-0x0805 PSG */
			if(addr <= 0x0809)
			{
				pce_psg_w(addr, data);
				return;
			};
			break;
		case 0x0C00: /* 0x0C00-0x0C01 timer */
			if(addr == 0x0C00 || addr == 0x0C01)
			{
				pce_cpu_timer_w(addr & 1, data);
				return;
			};
			break;
		case 0x1000: /* 0x1000 input port */
			if(addr == 0x1000)
			{
				pce_cpu_input_w(data);
				return;
			}
			break;
		case 0x1400: /* 0x1402-0x1403 IRQ */
			if(addr == 0x1402 || addr == 0x1403)
			{
				pce_cpu_irq_w(addr & 1, data);
				return;
			};
			break;
	}

	debug("CPU UNKNOWN iopage/write %02X: %04X (PC:%08X)", data, addr,
		pce_cpu_reg_r(CPU_PC));
}

/**
 * Read memory.
 * \param addr          memory address to be be read
 * \return              memory content
 */
int pce_cpu_mem_r(int addr)
{
	uint8 page;

	assert(pce && pce_cpu);

	//debug("CPU mem/read: addr=%08x page=%08x", addr, page);

	/* lower ROM */
	if(addr <= 0x0FFFFF)
		return pce->rom[addr];

	page = (addr >> 13) & 0xFF;

	/* ROM */
	if(page <= 0x7F)
		return pce->rom[addr];

	/* RAM */
	if(page == 0xF8 || page == 0xF9 || page == 0xFA || page == 0xFB)
		return pce->ram[(addr & 0x7FFF)];

	/* I/O */
	if(page == 0xFF)
		return pce_cpu_iopage_r(addr & 0x1FFF);

	debug("CPU UNKNOWN mem/read %02X:%04X (PC:%08X)", page, addr & 0x1FFFF,
		pce_cpu_reg_r(CPU_PC));
	return (0xFF);
}

/**
 * Write memory.
 * \param addr          memory address to write to
 * \param data          data to be written
 */
void pce_cpu_mem_w(int addr, int data)
{
	assert(pce && pce_cpu);

	uint8 page = (addr >> 13) & 0xFF;

	//debug("CPU mem/write: addr=%08x page=%08x data=%d", addr, page, data);

	/* RAM */
	if(page == 0xF8 || page == 0xF9 || page == 0xFA || page == 0xFB)
	{
		pce->ram[(addr & 0x7FFF)] = data;
		return;
	}

	/* I/O */
	if(page == 0xFF)
	{
		pce_cpu_iopage_w(addr & 0x1FFF, data);
		return;
	}

	debug("CPU UNKNOWN mem/write %02X: %02X:%04X (PC:%08X)", data, page,
		addr & 0x1FFFF, pce_cpu_reg_r(CPU_PC));
}
