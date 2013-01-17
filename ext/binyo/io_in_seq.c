/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#include "binyo.h"
#include <stdarg.h>

typedef struct binyo_instream_seq_st {
    binyo_instream_interface *methods;
    binyo_instream *active;
    int i;
    binyo_instream **streams;
    int num;
} binyo_instream_seq;

#define int_safe_cast(out, in)		binyo_safe_cast_instream((out), (in), BINYO_INSTREAM_TYPE_SEQ, binyo_instream_seq)

static binyo_instream_seq* int_seq_alloc(void);
static ssize_t int_seq_read(binyo_instream *in, uint8_t *buf, size_t len);
static int int_seq_seek(binyo_instream *in, off_t offset, int whence);
static void int_seq_mark(binyo_instream *in);
static void int_seq_free(binyo_instream *in);

static binyo_instream_interface binyo_interface_seq = {
    BINYO_INSTREAM_TYPE_SEQ,
    int_seq_read,
    NULL,
    NULL,
    int_seq_seek,
    int_seq_mark,
    int_seq_free
};

binyo_instream *
binyo_instream_new_seq(binyo_instream *in1, binyo_instream *in2)
{
    return binyo_instream_new_seq_n(2, in1, in2);
}

binyo_instream *
binyo_instream_new_seq_n(int num, binyo_instream *in1, binyo_instream *in2, ...)
{
    binyo_instream_seq *in;
    va_list args;
    int i = 0;

    if (num < 2) {
	binyo_error_add("At least two streams must be passed");
	return NULL;
    }

    in = int_seq_alloc();
    in->streams = ALLOC_N(binyo_instream *, num);
    in->streams[i++] = in1;
    in->streams[i++] = in2;
    va_start(args, in2);

    while (i < num) {
	in->streams[i++] = va_arg(args, binyo_instream *);
    }

    va_end(args);
    in->num = num;
    in->i = 0;
    in->active = in1;
    return (binyo_instream *) in;
}

static binyo_instream_seq*
int_seq_alloc(void)
{
    binyo_instream_seq *ret;
    ret = ALLOC(binyo_instream_seq);
    memset(ret, 0, sizeof(binyo_instream_seq));
    ret->methods = &binyo_interface_seq;
    return ret;
}

static ssize_t
int_do_read(binyo_instream_seq *in, uint8_t *buf, size_t len)
{
    ssize_t read = 0;
    size_t total = 0;

    while (total < len && ((read = binyo_instream_read(in->active, buf, len - total)) >= 0)) {
	total += read;
	buf += read;
    }

    if (read == BINYO_ERR) return BINYO_ERR;
    if (total == 0 && read == BINYO_IO_EOF) return BINYO_IO_EOF;
    return total;
}

static ssize_t
int_seq_read(binyo_instream *instream, uint8_t *buf, size_t len)
{
    ssize_t read;
    binyo_instream_seq *in;
    size_t total = 0;

    int_safe_cast(in, instream);

    if (!buf) return BINYO_ERR;

    while (total < len) {
    	read = int_do_read(in, buf, len - total);
	if (read == BINYO_ERR) return BINYO_ERR;
	if (read == BINYO_IO_EOF) {
	    in->i++;
	    if (in->i == in->num) {
		return BINYO_IO_EOF;
	    }
	    else {
		in->active = in->streams[in->i];
	    }
	}
	else {
	    total += read;
	    buf += read;
	}
    }

    return total;
}

static int
int_seq_seek(binyo_instream *instream, off_t offset, int whence)
{
    binyo_instream_seq *in;

    int_safe_cast(in, instream);
    return binyo_instream_seek(in->active, offset, whence);
}

static void
int_seq_mark(binyo_instream *instream)
{
    binyo_instream_seq *in;
    int i;

    if (!instream) return;
    int_safe_cast(in, instream);

    for(i=0; i < in->num; i++) {
	binyo_instream_mark(in->streams[i]);
    }
}

static void
int_seq_free(binyo_instream *instream)
{
    binyo_instream_seq *in;
    int i;

    if (!instream) return;
    int_safe_cast(in, instream);

    for(i=0; i < in->num; i++) {
	binyo_instream_free(in->streams[i]);
    }
    xfree(in->streams);
}

