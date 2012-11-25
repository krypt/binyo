/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012
* Martin Bosslet <martin.bosslet@googlemail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#if !defined(_BINYO_H_)
#define _BINYO_H_

#include RUBY_EXTCONF_H

#if defined(__cplusplus)
extern "C" {
#endif

#define RSTRING_NOT_MODIFIED 1
#define RUBY_READONLY_STRING 1

#include <ruby.h>

#if defined(HAVE_RUBY_IO_H)
#include <ruby/io.h>
#endif


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

/* include headers */
#include "binyo-error.h"
#include "binyo-io.h"
#include "bytelist.h"
#include "byte.h"

extern VALUE mBinyo;

extern VALUE eBinyoError;
void Init_binyo(void);

#if defined(__cplusplus)
}
#endif

#endif /* _BINYO_H_ */

