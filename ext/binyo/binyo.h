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

