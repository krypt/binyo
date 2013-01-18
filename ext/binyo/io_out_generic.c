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

#include "binyo.h"

typedef struct binyo_outstream_io_st {
    binyo_outstream_interface *methods;
    VALUE io;
} binyo_outstream_io;

#define int_safe_cast(out, in)		binyo_safe_cast_outstream((out), (in), BINYO_OUTSTREAM_TYPE_IO_GENERIC, binyo_outstream_io)

static binyo_outstream_io* int_io_alloc(void);
static ssize_t int_io_write(binyo_outstream *out, uint8_t *buf, size_t len);
static int int_io_rb_write(binyo_outstream *out, VALUE vbuf, VALUE *ret);
static void int_io_mark(binyo_outstream *out);
static void int_io_free(binyo_outstream *out);

static binyo_outstream_interface binyo_interface_io = {
    BINYO_OUTSTREAM_TYPE_IO_GENERIC,
    int_io_write,
    int_io_rb_write,
    int_io_mark,
    int_io_free
};

binyo_outstream *
binyo_outstream_new_io_generic(VALUE io)
{
    binyo_outstream_io *out;

    out = int_io_alloc();
    out->io = io;
    return (binyo_outstream *) out;
}

static binyo_outstream_io *
int_io_alloc(void)
{
    binyo_outstream_io *ret;
    ret = ALLOC(binyo_outstream_io);
    memset(ret, 0, sizeof(binyo_outstream_io));
    ret->methods = &binyo_interface_io;
    return ret;
}

static ssize_t
int_io_write(binyo_outstream *outstream, uint8_t *buf, size_t len)
{
    VALUE vbuf, ret;

    if (!buf) return BINYO_ERR;

    vbuf = rb_str_new((const char *)buf, len);
    if(int_io_rb_write(outstream, vbuf, &ret) == BINYO_ERR) {
	binyo_error_add("Error while writing to IO");
	return BINYO_ERR;
    }
    return NUM2LONG(ret);
}

static VALUE
int_io_rb_protected_write(VALUE args)
{
    VALUE io, vbuf;
    io = rb_ary_entry(args, 0);
    vbuf = rb_ary_entry(args, 1);
    return rb_funcall(io, sBinyo_ID_WRITE, 1, vbuf);
}

static int
int_io_rb_write(binyo_outstream *outstream, VALUE vbuf, VALUE *ret)
{
    binyo_outstream_io *out;
    VALUE args;
    int state = 0;

    int_safe_cast(out, outstream);
    args = rb_ary_new();
    rb_ary_push(args, out->io);
    rb_ary_push(args, vbuf);
    *ret = rb_protect(int_io_rb_protected_write, args, &state);
    return state == 0 ? BINYO_OK : BINYO_ERR;
}

static void
int_io_mark(binyo_outstream *outstream)
{
    binyo_outstream_io *out;

    if (!outstream) return;
    int_safe_cast(out, outstream);
    rb_gc_mark(out->io);
}

static void
int_io_free(binyo_outstream *outstream)
{
    /* do nothing */
}

