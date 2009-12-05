/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* emu.c: Program entry point. */

#include "bc_emu.h"


t_rom*          emu_rom;            /**< pointer to memory containing ROM */
char*           emu_progname;       /**< program name (where applicable) */

/* module instances */
t_emu*          emu_emu;            /**< pointer to emulator module context */
t_ui*           emu_ui;             /**< pointer to UI module context */

/* modules interface */
t_video*        emu_video;          /**< video data interface structure */
t_audio*        emu_audio;          /**< audio data interface structure */
t_input*        emu_input;          /**< input data interface structure */

/*
 * NOTE:
 * This file acts only as holder of arch non-specific code. main() or other
 * de-facto arch-specific routines are part of architecture .c file in arch/
 * directory.
 */

/* -------------------------------------------------------------------------- *
 * Program life-cycle                                                         *
 * -------------------------------------------------------------------------- */

/**
 * Architecture-independent main() called from main() or its equivalent.
 * \param emu_name          emulator name
 * \param ui_name           UI frontend name
 * \returns                 1 if exited normally, otherwise 0
 */
int emu_main(char* emu_name, char* ui_name)
{
	debug("entering emu main...");

	/* get emulator */
	emu_emu = module_emu_find(emu_name);
	assert(emu_emu);
	assert(emu_emu->init && emu_emu->shutdown);

	/* get UI */
	emu_ui = module_ui_find(ui_name);
	assert(emu_ui);
	assert(emu_ui->init && emu_ui->shutdown);

	/* prepare interface */
	emu_video = xmalloc(sizeof(t_video));
	emu_audio = xmalloc(sizeof(t_audio));
	emu_input = xmalloc(sizeof(t_input));
	memset(emu_video, 0, sizeof(t_video));
	memset(emu_audio, 0, sizeof(t_audio));
	memset(emu_input, 0, sizeof(t_input));

	/* initialize */
	emu_ui->init();
	emu_emu->init();

	/* application main-loop */
	debug("entering emu main-loop...");
	while(1)
	{
		/* update input (read keys) */
		emu_ui->update_input();

		/* execute emulation for one frame */
		emu_emu->frame();

		/* update video (render buffer) and sound (play buffer) */
		emu_ui->update_video();
		emu_ui->update_audio();

		/* handle input related to UI */
		if(emu_input->quit)
			break; /* quit */
		if(emu_input->reset)
			emu_emu->reset(); /* reset */
	}
	debug("exiting emu main-loop...");

	/* shutdown */
	emu_emu->shutdown();
	emu_ui->shutdown();

	return 1;
}

/**
 * Architecture-independent shutdown called after finishing emu_main().
 */
void emu_exit()
{
	debug("exiting emu main...");

	/* cleanup ROM memory */
	if(emu_rom != NULL)
	{
		if(emu_rom->data != NULL)
			xfree(emu_rom->data);

		xfree(emu_rom);
	}

	/* emu / ui modules are pointers to modules_* array so cleaning
	 * them is REALLY BAD idea. */

	/* cleanup interface memory */
	if(emu_video != NULL)
		xfree(emu_video);
	if(emu_audio != NULL)
		xfree(emu_audio);
	if(emu_input != NULL)
		xfree(emu_input);

}
