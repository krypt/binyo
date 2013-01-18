/*
 * binyo - Fast binary IO for Ruby
 *
 * Copyright (c) 2012-2013
 * Martin Bosslet <martin.bosslet@gmail.com>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _BINYO_MEM_H_
#define _BINYO_MEM_H_

/* memory utilities */
#define BINYO_ALLOC(type) ((type *)binyo_alloc(sizeof(type)))
#define BINYO_ALLOC_N(type, n) ((type *)binyo_alloc_n((n), sizeof(type)))
#define BINYO_ALLOCA_N(type, n) ((type *)alloca(sizeof(type) * (n)))
#define BINYO_REALLOC_N(ptr, type, n) ((ptr)=(type *)binyo_realloc_n((void *)(ptr), (n), sizeof(type)))
#define BINYO_FREE binyo_free

void *binyo_alloc(size_t size);
void *binyo_alloc_n(size_t n, size_t size);
void *binyo_realloc_n(void *ptr, size_t n, size_t size);
void binyo_free(void *ptr);

#endif /* _BINYO_MEM_H_ */

