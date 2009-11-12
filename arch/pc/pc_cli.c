/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pc_cli.c: Personal Computer (unix/mac/win based) CLI architecture. */

#include "bc_emu.h"


/**
 * Print short help message and exit.
 * \param exit_code     exit code
 */
void print_usage(int exit_code)
{
	printf("Usage: bc_emu [options] file\n"
		"\n"
		"Mandatory arguments to long options are mandatory for short too.\n"
		"  -e, --emulator=EMU   force `bc_emu' to use EMULATOR core\n"
		"  -u, --ui=UI          force `bc_emu' to use UI frontend\n"
		"  -h, --help           show this message and exit\n"
		"  -V, --version        show version information and exit\n"
		"\n"
		"Emulator core is selected according to file's extension. You may\n"
		"need to toggle it using --emulator parameter.\n"
		"\n"
		"Report bugs to ondra@blami.net\n");

}

/**
 * Print version info and exit.
 */
void print_version(int exit_code)
{

}

/**
 * Program entry point. Initialize modules, load ROM, run emulator.
 * \param argc          command line argument count
 * \param argv[]        command line argument values
 * \return              exit code
 */
int main(int argc, char** argv)
{
	printf("bc_emu: portable video game emulator\n"
		"Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>\n"
		"http://www.blami.net/prj/bc_emu\n"
		"\n"
		"This is free software licensed under MIT license. See LICENSE.\n");

	debug("init");

	debug("shutdown");
}
