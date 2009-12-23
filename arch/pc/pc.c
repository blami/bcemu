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


static char* pc_emu_name;   /**< -e: emulator name \see EMU */
static char* pc_ui_name;    /**< -u: UI name \see UI */
static char* pc_rom_file;   /**< ROM filename */

static int pc_is_init = 0;  /**< whether emu_main() was called */

/**
 * Print short help message and exit.
 * \param exit_code     exit code
 */
static void pc_print_usage(int exit_code)
{
	printf("Usage: bc_emu [options] file\n"
		"\n"
		"  -e EMU   force `bc_emu' to use EMU\n"
		"  -u UI    force `bc_emu' to use UI\n"
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
static void pc_print_version(int exit_code)
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
static void pc_atexit()
{
	/* check whether emu_main was called */
	if(pc_is_init)
		emu_exit();

	/* TODO here should be only pc.c cleanup code. */

	if(pc_ui_name != NULL)
		xfree(pc_ui_name);
	if(pc_emu_name != NULL)
		xfree(pc_emu_name);

	if(pc_rom_file != NULL)
		xfree(pc_rom_file);

	printf("\nExiting...\n");
}

/**
 * Load content of ROM file into memory.
 */
static t_rom* pc_load_rom(const char* filename)
{
	t_rom* r;
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
		debug("couldn't seek back to file begin");
		fclose(f);
		return NULL;
	}

	buf = xmalloc(size);
	if(!fread(buf, size, 1, f))
	{
		debug("couldn't read ROM image file `%s' contents, possibly corrupted",
			filename);
		fclose(f);
		return NULL;
	}

	/* here we should stop, internal content of ROM should be parsed on
	 * architecture-independent basis by proper emulator */

	debug("ROM image file `%s' read successfully", filename);
	fclose(f);

	r = xmalloc(sizeof(t_rom));
	r->size = size;
	r->data = buf;

	return r;
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
	atexit(pc_atexit);

	/* prepare pc static fields */
	pc_emu_name = NULL;
	pc_ui_name = NULL;
	pc_rom_file = NULL;

	/* prepare bc_emu global fields */
	emu_rom = NULL;
	emu_progname = argv[0];

	/* resolve command line arguments */
	while(1)
	{
		int p = xgetopt(argc, argv, "-hVe:u:");
		if(p == -1)
			break;

		switch(p)
		{
			case 'h':
				pc_print_usage(EXIT_SUCCESS);
				break;
			case 'V':
				pc_print_version(EXIT_SUCCESS);
				break;
			case 'e':
				pc_emu_name = xstrndup(xoptarg, strlen(xoptarg));
				debug("-e: forced to use emulator `%s'", xoptarg);
				break;
			case 'u':
				pc_ui_name = xstrndup(xoptarg, strlen(xoptarg));
				debug("-u: forced to use UI `%s'", xoptarg);
				break;

			case 1:
				pc_rom_file = xstrndup(xoptarg, strlen(xoptarg));
				break;
		}
	}

	/* check and preload rom_file to global variable */
	if(!pc_rom_file)
	{
		fprintf(stderr, "%s: missing ROM image filename\n", emu_progname);
		exit(EXIT_FAILURE);
	}

	/* put ROM image file to memory (doesn't bother 2nd and 3rd gen images are
	 * around 15M max. */
	emu_rom = pc_load_rom(pc_rom_file);
	if(!emu_rom)
	{
		fprintf(stderr, "%s: couldn't load ROM image from file: %s\n",
			emu_progname,
			pc_rom_file);
		exit(EXIT_FAILURE);
	}

	/* TODO emu/ui autoselect */

	/* invoke architecture independent/safe bc_emu code */
	/* bc_emu.c */
	pc_is_init = 1;
	emu_main(pc_emu_name, pc_ui_name);
	/* called from exit_handler():
	 * bc_emu_exit(); */

	return EXIT_SUCCESS;
}
