/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012
* Martin Bosslet <martin.bosslet@googlemail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
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
    int w;

    if (!buf) return -1;

    vbuf = rb_str_new((const char *)buf, len);
    w = int_io_rb_write(outstream, vbuf, &ret);
    if (!w) {
	binyo_error_add("Error while writing to IO");
	return -1;
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
    return state == 0;
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

