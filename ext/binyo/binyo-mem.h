/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
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

