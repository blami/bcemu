/*
 * debug.h: debug macros
 * Copyright © 2006-2010 Ondrej Balaz, <ondra@blami.net>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/* debug.h: Debug macros */

#ifndef __DEBUG_H
#define __DEBUG_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef DEBUG
/**
 * Debug macro. If is macro DEBUG defined print debug message to stderr,
 * otherwise does nothing.
 * \param format        debug message format
 * \param arg           arguments used as wildcards in format message
 */
#define debug(format, arg...) \
{ \
	fprintf(stderr, "debug: %s:%.4d %s(): " format "\n",\
		__FILE__, __LINE__, __func__, ##arg); \
}
#else
#define debug(format, arg...)
#endif /* DEBUG */

/**
 * Error macro. Prints formated error and if possible then calls perror().
 * \param format        error message format
 * \param arg           arguments used as wildcards in format message
 */
#define error(format, arg...) \
{ \
	fprintf(stderr, "error: %s(): " format "\n",\
		__func__, ##arg); \
	\
	if(errno) \
		perror("error"); \
}

/**
 * Fatal error macro. Calls error macro, and then exits with EXIT_FAILURE.
 * \param format        error message format
 * \param arg           arguments used as wildcards in format message
 * \see error
 */
#define fatal(format, arg...) \
{ \
	error(format, ##arg); \
	fprintf(stderr, "fatal error, exiting.\n"); \
	\
	exit(EXIT_FAILURE); \
}

#endif /* __DEBUG_H */
