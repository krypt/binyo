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

typedef struct binyo_instream_io_st {
    binyo_instream_interface *methods;
    VALUE io;
} binyo_instream_io;

#define int_safe_cast(out, in)		binyo_safe_cast_instream((out), (in), BINYO_INSTREAM_TYPE_IO_GENERIC, binyo_instream_io)

static binyo_instream_io* int_io_alloc(void);
static ssize_t int_io_read(binyo_instream *in, uint8_t *buf, size_t len);
static int int_io_rb_read(binyo_instream *in, VALUE vlen, VALUE vbuf, VALUE *out);
static int int_io_seek(binyo_instream *in, off_t offset, int whence);
static void int_io_mark(binyo_instream *in);
static void int_io_free(binyo_instream *in);

static binyo_instream_interface binyo_interface_io_generic = {
    BINYO_INSTREAM_TYPE_IO_GENERIC,
    int_io_read,
    int_io_rb_read,
    NULL,
    int_io_seek,
    int_io_mark,
    int_io_free
};

binyo_instream *
binyo_instream_new_io_generic(VALUE io)
{
    binyo_instream_io *in;

    in = int_io_alloc();
    in->io = io;
    return (binyo_instream *) in;
}

static binyo_instream_io*
int_io_alloc(void)
{
    binyo_instream_io *ret;
    ret = ALLOC(binyo_instream_io);
    memset(ret, 0, sizeof(binyo_instream_io));
    ret->methods = &binyo_interface_io_generic;
    return ret;
}

static VALUE
int_io_rb_protected_read(VALUE args)
{
    VALUE io, vbuf, vlen;
    io = rb_ary_entry(args, 0);
    vlen = rb_ary_entry(args, 1);
    vbuf = rb_ary_entry(args, 2);
    return rb_funcall(io, sBinyo_ID_READ, 2, vlen, vbuf);
}

static int
int_io_rb_read_impl(binyo_instream_io *in, VALUE vlen, VALUE vbuf, VALUE *out)
{
    VALUE args = rb_ary_new();
    int state = 0;
    rb_ary_push(args, in->io);
    rb_ary_push(args, vlen);
    rb_ary_push(args, vbuf);
    *out = rb_protect(int_io_rb_protected_read, args, &state);
    return state == 0;
}

static ssize_t
int_io_read(binyo_instream *instream, uint8_t *buf, size_t len)
{
    VALUE read, vlen, vbuf;
    binyo_instream_io *in;

    int_safe_cast(in, instream);

    if (!buf) return -2;

    vlen = LONG2NUM(len);
    vbuf = rb_str_new2("");
    rb_enc_associate(vbuf, rb_ascii8bit_encoding());

    if (!int_io_rb_read_impl(in, vlen, vbuf, &read)) {
	binyo_error_add("Error while reading from IO");
	return -2;
    }
    
    if (NIL_P(read)) {
	return -1;
    }
    else {
	ssize_t r = (ssize_t) RSTRING_LEN(read);
	memcpy(buf, RSTRING_PTR(read), r);
	return r;
    }
}

static int
int_io_rb_read(binyo_instream *instream, VALUE vlen, VALUE vbuf, VALUE *out)
{
    binyo_instream_io *in;

    int_safe_cast(in, instream);
    return int_io_rb_read_impl(in, vlen, vbuf, out);
}

static VALUE
int_whence_sym_for(int whence)
{
    switch (whence) {
	case SEEK_CUR:
	    return sBinyo_ID_SEEK_CUR;
	case SEEK_SET:
	    return sBinyo_ID_SEEK_SET;
	case SEEK_END:
	    return sBinyo_ID_SEEK_END;
	default:
	    binyo_error_add("Unknown whence: %d", whence);
	    return Qnil;
    }
}

/* TODO: rb_protect */
static int
int_io_seek(binyo_instream *instream, off_t offset, int whence)
{
    VALUE io, whencesym;
    binyo_instream_io *in;

    int_safe_cast(in, instream);

    io = in->io;
    whencesym = int_whence_sym_for(whence);
    if (NIL_P(whencesym)) return 0;
    rb_funcall(io, sBinyo_ID_SEEK, 2, LONG2NUM(offset), whencesym);
    return 1;
}

static void
int_io_mark(binyo_instream *instream)
{
    binyo_instream_io *in;

    if (!instream) return;
    int_safe_cast(in, instream);

    rb_gc_mark(in->io);
}

static void
int_io_free(binyo_instream *instream)
{
    /* do nothing */
}

