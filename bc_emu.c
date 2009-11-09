/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* bc_emu.c: Program entry point. */

#include "bc_emu.h"


#ifndef NOMAIN
/**
 * Program entry point.
 * \param argc          command line arguments count
 * \param argv          pointer to array of command line argument values
 * \return              exit status
 */
int main(int argc, char** argv)
{
	printf("hello world");
	debug("begin");


	debug("exit");
	return EXIT_SUCCESS;
}

#endif /* NOMAIN */
