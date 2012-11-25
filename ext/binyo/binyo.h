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

#include "binyo-missing.h"

#ifdef _WIN32
#define binyo_last_sys_error()	GetLastError()
#define binyo_clear_sys_error()	SetLastError(0)
#else
#define binyo_last_sys_error()	errno
#define binyo_clear_sys_error()	errno=0
#endif

/* include headers */
#include "binyo-mem.h"
#include "binyo-error.h"
#include "binyo-error-internal.h"
#include "binyo-io.h"
#include "bytelist.h"
#include "byte.h"

extern VALUE mBinyo;

extern VALUE eBinyoError;
void Init_binyo(void);
void Init_binyo_io(void);

#if defined(__cplusplus)
}
#endif

#endif /* _BINYO_H_ */

