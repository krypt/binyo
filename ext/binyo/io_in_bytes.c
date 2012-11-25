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

struct binyo_byte_ary_st {
    uint8_t *p;
    size_t len;
};

typedef struct binyo_instream_bytes_st {
    binyo_instream_interface *methods;
    struct binyo_byte_ary_st *src;
    size_t num_read;
} binyo_instream_bytes;

#define int_safe_cast(out, in)		binyo_safe_cast_instream((out), (in), BINYO_INSTREAM_TYPE_BYTES, binyo_instream_bytes)

static binyo_instream_bytes* int_bytes_alloc(void);
static ssize_t int_bytes_read(binyo_instream *in, uint8_t *buf, size_t len);
static ssize_t int_bytes_gets(binyo_instream *in, char *line, size_t len);
static int int_bytes_seek(binyo_instream *in, off_t offset, int whence);
static void int_bytes_free(binyo_instream *in);

static binyo_instream_interface binyo_interface_bytes = {
    BINYO_INSTREAM_TYPE_BYTES,
    int_bytes_read,
    NULL,
    int_bytes_gets,
    int_bytes_seek,
    NULL,
    int_bytes_free
};

binyo_instream *
binyo_instream_new_bytes(uint8_t *bytes, size_t len)
{
    binyo_instream_bytes *in;
    struct binyo_byte_ary_st *byte_ary;

    in = int_bytes_alloc();
    byte_ary = ALLOC(struct binyo_byte_ary_st);
    byte_ary->p = bytes;
    byte_ary->len = len;
    in->src = byte_ary;
    return (binyo_instream *) in;
}

static binyo_instream_bytes*
int_bytes_alloc(void)
{
    binyo_instream_bytes *ret;
    ret = ALLOC(binyo_instream_bytes);
    memset(ret, 0, sizeof(binyo_instream_bytes));
    ret->methods = &binyo_interface_bytes;
    return ret;
}

static ssize_t
int_bytes_read(binyo_instream *instream, uint8_t *buf, size_t len)
{
    struct binyo_byte_ary_st *src;
    size_t to_read;
    binyo_instream_bytes *in;

    int_safe_cast(in, instream);

    if (!buf) return -2;

    src = in->src;

    if (in->num_read == src->len)
	return -1;

    to_read = src->len - in->num_read < len ? src->len - in->num_read : len;
    memcpy(buf, src->p, to_read);
    src->p += to_read;
    in->num_read += to_read;
    return to_read;
}

static ssize_t
int_bytes_gets(binyo_instream *instream, char *line, size_t len)
{
    struct binyo_byte_ary_st *src;
    binyo_instream_bytes *in;
    ssize_t ret = 0;
    size_t to_read;
    char *d;
    char *end;

    int_safe_cast(in, instream);
    src = in->src;

    if (in->num_read == src->len)
	return -1;

    d = line;
    to_read = src->len - in->num_read < len ? src->len - in->num_read : len;
    end = d + to_read;

    while (d < end) {
	*d = *(src->p);    
	src->p++;
	if (*d == '\n') {
            in->num_read++;
	    break;
        }
	d++;
	ret++;
    }
    in->num_read += ret;

    if (*d == '\n' && *(d - 1) == '\r')
	ret--;

    return ret;
}

static int
int_bytes_set_pos(binyo_instream_bytes *in, off_t offset, size_t num_read)
{
    struct binyo_byte_ary_st *src = in->src;

    if (src->len - offset <= num_read) {
	binyo_error_add("Unreachable seek position");
	return 0;
    }
    src->p += offset;
    in->num_read += offset;
    return 1;
}

/* TODO check overflow */
static int
int_bytes_seek(binyo_instream *instream, off_t offset, int whence)
{
    struct binyo_byte_ary_st *src;
    size_t num_read;
    binyo_instream_bytes *in;

    int_safe_cast(in, instream);

    src = in->src;
    num_read = in->num_read;

    switch (whence) {
	case SEEK_CUR:
	    return int_bytes_set_pos(in, offset, num_read);
	case SEEK_SET:
	    return int_bytes_set_pos(in, offset - num_read, num_read);
	case SEEK_END:
	    return int_bytes_set_pos(in, offset + src->len - num_read, num_read);
	default:
	    binyo_error_add("Unknown whence: %d", whence);
	    return 0;
    }
}

static void
int_bytes_free(binyo_instream *instream)
{
    binyo_instream_bytes *in;

    if (!instream) return;
    int_safe_cast(in, instream);

    xfree(in->src);
}

