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

#define int_check_stream(io) 		if (!(io) || !(io)->methods) \
						    rb_raise(eBinyoError, "Stream not initialized properly")

#define int_check_stream_has(io, m) 		if (!(io) || !(io)->methods || !(io)->methods->m) \
						    rb_raise(eBinyoError, "Stream not initialized properly")

VALUE sBinyo_ID_SEEK_CUR, sBinyo_ID_SEEK_SET, sBinyo_ID_SEEK_END;
ID sBinyo_ID_READ, sBinyo_ID_SEEK, sBinyo_ID_WRITE, sBinyo_ID_CLOSE;

void
binyo_add_io_error(void)
{
    int err = binyo_last_sys_error();
    binyo_error_add("Error stream IO: %d", err);
}

/* instream */

static int
int_read_all(binyo_instream *in, VALUE vbuf, VALUE *out)
{
    uint8_t *buf;
    ssize_t r;

    buf = ALLOC_N(uint8_t, BINYO_IO_BUF_SIZE);

    while ((r = binyo_instream_read(in, buf, BINYO_IO_BUF_SIZE)) >= 0) {
	rb_str_buf_cat(vbuf, (const char *) buf, r);
    }

    xfree(buf);
    if (r == BINYO_ERR) return BINYO_ERR;
    *out = vbuf;
    return BINYO_OK;
}

static int
int_rb_read_generic(binyo_instream *in, VALUE vlen, VALUE vbuf, VALUE *out)
{
    long len;
    size_t tlen;
    ssize_t r;
    uint8_t *buf;

    if (NIL_P(vbuf)) {
	vbuf = rb_str_new2("");
	rb_enc_associate(vbuf, rb_ascii8bit_encoding());
    }

    if (NIL_P(vlen))
	 return int_read_all(in, vbuf, out);

    len = NUM2LONG(vlen);
    if (len < 0) {
	binyo_error_add("Negative length given");
	return BINYO_ERR;
    }
    if ((size_t) len > SIZE_MAX) {
	binyo_error_add("Size too large: %ld", len);
	return BINYO_ERR;
    }

    tlen = (size_t) len;
    if (len == 0) {
	rb_str_resize(vbuf, 0);
	*out = vbuf;
	return BINYO_OK;
    }

    buf = ALLOC_N(uint8_t, tlen);
    r = binyo_instream_read(in, buf, tlen);

    if (r == BINYO_ERR) {
	binyo_error_add("Error while reading from stream");
	xfree(buf);
	return BINYO_ERR;
    }
    else if (r == BINYO_IO_EOF) {
	xfree(buf);
	rb_str_resize(vbuf, 0);
	*out = Qnil;
	return BINYO_OK;
    }
    else {
	rb_str_buf_cat(vbuf, (const char *)buf, r);
	xfree(buf);
	*out = vbuf;
	return BINYO_OK;
    }
}

int
binyo_instream_rb_read(binyo_instream *in, VALUE vlen, VALUE vbuf, VALUE *out)
{
    int_check_stream(in);

    if (in->methods->rb_read) {
	return in->methods->rb_read(in, vlen, vbuf, out);
    }
    else {
	return int_rb_read_generic(in, vlen, vbuf, out);
    }
}

ssize_t 
binyo_instream_read(binyo_instream *in, uint8_t *buf, size_t len)
{
    int_check_stream_has(in, read);

    if (len > SSIZE_MAX) {
	binyo_error_add("Size too large: %ld", len);
	return BINYO_ERR;
    }
    return in->methods->read(in, buf, len);
}

static ssize_t
int_gets_generic(binyo_instream *in, char *line, size_t len)
{
    ssize_t ret = 0, r = 0;
    char *p = line;
    char *end = line + len;

    if (!line) return BINYO_ERR;

    while (p < end) {
	if ((r = in->methods->read(in, (uint8_t *) p, 1)) < 0)
	    break;
	if (r == 1) {
	    if (*p == '\n')
		break;
	    p++;
	    ret++;
	}
    }

    if (r == BINYO_ERR) return BINYO_ERR;
    if (ret == 0 && r == BINYO_IO_EOF) return BINYO_IO_EOF;

    /* normalize CRLF */
    if (*p == '\n' && *(p - 1) == '\r')
       ret--;	

    return ret;
}

ssize_t
binyo_instream_gets(binyo_instream *in, char *line, size_t len)
{
    int_check_stream(in);
    if (len > SSIZE_MAX) {
	binyo_error_add("Size too large: %ld", len);
	return BINYO_ERR;
    }
    if (in->methods->gets) {
	return in->methods->gets(in, line, len);
    }
    else {
	return int_gets_generic(in, line, len);
    }
}

int
binyo_instream_seek(binyo_instream *in, off_t offset, int whence)
{
    int_check_stream_has(in, seek);
    return in->methods->seek(in, offset, whence);
}

void
binyo_instream_mark(binyo_instream *in)
{
    int_check_stream(in);
    if (in->methods->mark)
	in->methods->mark(in);
}

void
binyo_instream_free(binyo_instream *in)
{
    int_check_stream(in);
    if (in->methods->free)
	in->methods->free(in);
    xfree(in);
}

static binyo_instream *
int_instream_common_new(VALUE value)
{
    int type;

    type = TYPE(value);

    if (type == T_STRING) {
	return binyo_instream_new_bytes((uint8_t *)RSTRING_PTR(value), RSTRING_LEN(value));
    }
    else {
	if (type == T_FILE) {
	    return binyo_instream_new_fd_io(value);
	}
	else if (rb_respond_to(value, sBinyo_ID_READ)) {
	    return binyo_instream_new_io_generic(value);
	}
    }
    return NULL;
}

binyo_instream *
binyo_instream_new_value(VALUE value)
{
    return int_instream_common_new(value);
}

/* end instream */

/* outstream */

ssize_t 
binyo_outstream_write(binyo_outstream *out, uint8_t *buf, size_t len)
{
    int_check_stream_has(out, write);
    if (len > SSIZE_MAX) {
	binyo_error_add("Size too large: %ld", len);
	return BINYO_ERR;
    }
    return out->methods->write(out, buf, len);
}

int
binyo_outstream_rb_write(binyo_outstream *out, VALUE vbuf, VALUE *ret)
{
    int_check_stream(out);

    if (out->methods->rb_write) {
	return out->methods->rb_write(out, vbuf, ret);
    }
    else {
	ssize_t w;
	w = binyo_outstream_write(out, (uint8_t *) RSTRING_PTR(vbuf), RSTRING_LEN(vbuf));
	if (w == BINYO_ERR) {
	    binyo_error_add("Error while writing to stream");
	    return BINYO_ERR;
	}
	*ret = LONG2NUM(w);
	return BINYO_OK;
    }
}

void
binyo_outstream_mark(binyo_outstream *out)
{
    int_check_stream(out);
    if (out->methods->mark)
	out->methods->mark(out);
}

void
binyo_outstream_free(binyo_outstream *out)
{
    int_check_stream(out);
    if (out->methods->free)
	out->methods->free(out);
    xfree(out);
}

binyo_outstream *
binyo_outstream_new_value(VALUE value)
{
    int type;

    type = TYPE(value);

    if (type == T_FILE)
	return binyo_outstream_new_fd_io(value);
    if (rb_respond_to(value, sBinyo_ID_WRITE))
	return binyo_outstream_new_io_generic(value);
    binyo_error_add("Value cannot be converted into a stream");
    return NULL;
}

/* end outstream */

void
Init_binyo_io(void)
{
    sBinyo_ID_SEEK = rb_intern("seek");
    sBinyo_ID_READ = rb_intern("read");
    sBinyo_ID_WRITE = rb_intern("write");
    sBinyo_ID_CLOSE = rb_intern("close");

    sBinyo_ID_SEEK_CUR = rb_const_get(rb_cIO, rb_intern("SEEK_CUR"));
    sBinyo_ID_SEEK_SET = rb_const_get(rb_cIO, rb_intern("SEEK_SET"));
    sBinyo_ID_SEEK_END = rb_const_get(rb_cIO, rb_intern("SEEK_END"));
}

