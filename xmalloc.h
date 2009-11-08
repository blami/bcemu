/*
 * xmalloc.h: memory allocation wrappers
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

/* xmalloc.h: Memory allocation wrappers headers. */

#ifndef __XMALLOC_H
#define __XMALLOC_H

/*
 * Function prototypes.
 */

/* malloc.h */
extern void*    xmalloc(size_t);
extern void*    xrealloc(void*, size_t);
extern void     xfree(void*);

/* string.h */
extern char*    xstrdup(const char*);
extern char*    xstrndup(const char*, size_t);


#endif /* __XMALLOC_H */
