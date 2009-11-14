/******************************************************************************
 * bc_emu: portable video game emulator                                       *
 * Copyright © 2008-2010 Ondrej Balaz, <ondra@blami.net>                      *
 * http://www.blami.net/prj/bc_emu                                            *
 *                                                                            *
 * This is free software licensed under MIT license. See LICENSE.             *
 ******************************************************************************/

/* pc.c: PC architecture (unix/mac/win). */

#include "bc_emu.h"
#include "pc.h"
#include "xgetopt.h"


char* prog_name;        /* program executable name */

char* emu_name;         /* -e: emulator name \see EMU */
char* ui_name;          /* -u: UI name \see UI */
char* dbg_name;         /* -d: debugger name \see DBG */

char* in_file;          /* ROM filename */

static int main_call = 0;

/**
 * Print short help message and exit.
 * \param exit_code     exit code
 */
static void print_usage(int exit_code)
{
	printf("Usage: bc_emu [options] file\n"
		"\n"
		"  -e EMU   force `bc_emu' to use EMU core\n"
		"  -u UI    force `bc_emu' to use UI frontend\n"
		"  -d       enable ROM debugger\n"
		"  -h       show this message and exit\n"
		"  -V       show version information and exit\n"
		"\n"
		"Emulator core is selected according to file's extension. You may\n"
		"need to toggle it using -e parameter.\n"
		"To obtain list of available EMU and UI options type `bc_emu -V'.\n"
		"\n"
		"Report bugs to ondra@blami.net\n");

	exit(exit_code);
}

/**
 * Print version info and exit.
 * \param exit_code     exit code
 */
static void print_version(int exit_code)
{
	printf("Version: %s (%s) %s\n"
		"Build OS: %s\n"
		"Build CPU: %s\n"
		"============================================================\n"
		"Architecture: %s\n"
		"Emulators supported: %s\n"
		"UIs supported: %s\n"
		,
		VERSION,
		CODENAME,
#ifdef DEBUG
		"DEBUG",
#else
		"",
#endif /* DEBUG */
		BUILD_OS,
		BUILD_CPU,
		ARCH,
		EMU,
		UI);

	exit(exit_code);
}

/**
 * Cleanup before exit.
 */
static void exit_handler()
{
	if(main_call)
		bc_emu_exit();

	/* TODO here should be only pc.c cleanup code. */

	if(ui_name != NULL)
		xfree(ui_name);
	if(emu_name != NULL)
		xfree(emu_name);

	if(in_file != NULL)
		xfree(in_file);

	printf("\nExiting...\n");
}

/**
 * Load content of ROM file into memory.
 * \param filename      ROM image filename
 * \return              pointer to block of memory containing ROM image if
 *                      success, otherwise NULL.
 */
static uint8* load_rom(char* filename)
{
	uint8* buf = NULL;
	FILE* f = NULL;
	int size;

	debug("loading ROM image: %s", filename);

	if(!(f = fopen(filename, "r")))
		return NULL;

	/* get file size, portable way (ANSI C) */
	if(fseek(f, 0, SEEK_END) != 0)
	{
		debug("couldn't seek to file end");
		fclose(f);
		return NULL;
	}

	size = ftell(f);
	debug("ROM image size is: %d bytes", size);

	/* seek back to file begin and read it to buffer */
	if(fseek(f, 0, SEEK_SET) != 0)
	{
		debug("couldn't seek to file begin");
		fclose(f);
		return NULL;
	}

	buf = xmalloc(size);
	if(!fread(buf, size, 1, f))
	{
		debug("couldn't read file contents, possibly corrupted");
		fclose(f);
		return NULL;
	}

	/* here we should stop, internal content of ROM should be parsed on
	 * architecture-independent basis by proper emulator */

	debug("ROM image file read successfully\n");
	fclose(f);

	return buf;
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
		"This is free software licensed under MIT license. See LICENSE.\n"
		"\n");

	/* register exit handler */
	atexit(exit_handler);

	prog_name = argv[0];

	emu_name = NULL;
	ui_name = NULL;

	in_file = NULL;

	/* resolve command line arguments */
	while(1)
	{
		int p = getopt(argc, argv, "-hVe:u:d:");
		if(p == -1)
			break;

		switch(p)
		{
			case 'h':
				print_usage(EXIT_SUCCESS);
				break;
			case 'V':
				print_version(EXIT_SUCCESS);
				break;
			case 'e':
				emu_name = xstrndup(optarg, strlen(optarg));
				debug("-e: forced emulator `%s'", optarg);
				break;
			case 'u':
				ui_name = xstrndup(optarg, strlen(optarg));
				debug("-u: forced UI frontend `%s'", optarg);
				break;
			case 'd':
				debug("debbuger: %s", optarg);
				/* FIXME implement */
				debug("THIS FEATURE IS NOT IMPLEMENTED YET!");
				break;

			case 1:
				in_file = xstrndup(optarg, strlen(optarg));
				break;
		}
	}

	/* check and preload in_file to global variable */
	if(!in_file)
	{
		fprintf(stderr, "%s: missing ROM image filename\n", prog_name);
		exit(EXIT_FAILURE);
	}

	/* put ROM image file to memory (doesn't bother 2nd and 3rd gen images are
	 * around 15M max. */
	bc_emu_rom = load_rom(in_file);
	if(!bc_emu_rom)
	{
		fprintf(stderr, "%s: couldn't load ROM image from file: %s\n",
			prog_name,
			in_file);
		exit(EXIT_FAILURE);
	}

	/* invoke architecture independent/safe bc_emu code */
	/* bc_emu.c */
	main_call = 1;
	bc_emu_main(emu_name, ui_name, NULL); /* FIXME implement debugger */
	/* called from exit_handler():
	 * bc_emu_exit(); */

	return EXIT_SUCCESS;
}
