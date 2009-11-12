/*
 * xmalloc.c: memory allocation wrappers
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

/* xmalloc.c: Memory allocation wrappers. */

#include "debug.h"
#include <malloc.h>
#include <string.h>


/* malloc.h */

/**
 *  Allocate memory block of specified length. If malloc() fails crash
 *  otherwise return pointer.
 *  \param size         size of allocated block of memory
 *  \return             pointer to allocated memory
 */
void*
xmalloc(size_t size)
{
	void* ptr;
	if(!(ptr = malloc(size)))
		fatal("malloc() failed!");

	debug("%p (%u bytes)", ptr, (unsigned) size);
	return ptr;
}

/**
 *  Reallocate memory at given address to new size. If fails crash, otherwise
 *  return pointer.
 *  \param ptr          pointer to existing block of memory
 *  \param size         desired size of new block
 *  \return             pointer to allocated memory
 */
void*
xrealloc(void* ptr, size_t size)
{
	void* n_ptr;
	if(!ptr)
	{
		debug("memory realloc() on NULL pointer!");
		return xmalloc(size);
	}

	if(!(n_ptr = realloc(ptr, size)))
		fatal("realloc() failed!");

	ptr = n_ptr; //?
	debug("%p (%u bytes)", ptr, (unsigned) size);
	return n_ptr;
}

/**
 *  Free allocated memory.
 *  \param ptr          pointer to allocated memory
 */
void
xfree(void* ptr)
{
	if(!ptr)
	{
		debug("nothing to free()!");
		return;
	}

	debug("%p", ptr);
	free(ptr);
}


/* string.h */

/**
 *  Duplicate string with allocation.
 *  \param s            pointer to existing string
 *  \return             pointer to duplicate
 */
char*
xstrdup(const char* s)
{
	char* n_s = NULL;

	if(!s)
	{
		debug("strdup() called on NULL string!");
		return NULL;
	}

	n_s = xmalloc(strlen(s) + 1);
	if(!(n_s = strcpy(n_s, s)))
		fatal("strcpy() failed!");

	n_s[strlen(s)] = '\0';

	return n_s;
}

/**
 *  Duplicate string with specified length.
 *  \param s            pointer to existing string
 *  \param n            length to duplicate
 *  \return             pointer to a duplicate of same length if n is lower or
 *                      equal to original length, otherwise up to length n
 */
char*
xstrndup(const char* s, size_t n)
{
	char* n_s = NULL;

	if(!s)
	{
		debug("strndup() called on NULL string!");
		return NULL;
	}

	n_s = xmalloc(n + 1);
	if(!(n_s = strncpy(n_s, s, n)))
		fatal("strncpy() failed!");

	n_s[n + 1] = '\0';

	return n_s;
}
