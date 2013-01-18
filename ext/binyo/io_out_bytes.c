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

typedef struct binyo_outstream_bytes_st {
    binyo_outstream_interface *methods;
    binyo_byte_buffer *buffer;
} binyo_outstream_bytes;

#define int_safe_cast(out, in)		binyo_safe_cast_outstream((out), (in), BINYO_OUTSTREAM_TYPE_BYTES, binyo_outstream_bytes)

static binyo_outstream_bytes* int_bytes_alloc(void);
static ssize_t int_bytes_write(binyo_outstream *out, uint8_t *buf, size_t len);
static void int_bytes_free(binyo_outstream *out);

static binyo_outstream_interface binyo_interface_bytes = {
    BINYO_OUTSTREAM_TYPE_BYTES,
    int_bytes_write,
    NULL,
    NULL,
    int_bytes_free
};

binyo_outstream *
binyo_outstream_new_bytes(void)
{
    binyo_outstream_bytes *out;

    out = int_bytes_alloc();
    out->buffer = binyo_buffer_new();
    return (binyo_outstream *) out;
}

binyo_outstream *
binyo_outstream_new_bytes_size(size_t size)
{
    binyo_outstream_bytes *out;

    out = int_bytes_alloc();
    out->buffer = binyo_buffer_new_size(size);
    return (binyo_outstream *) out;
}

binyo_outstream *
binyo_outstream_new_bytes_prealloc(uint8_t *b, size_t len)
{
    binyo_outstream_bytes *out;

    out = int_bytes_alloc();
    out->buffer = binyo_buffer_new_prealloc(b, len);
    return (binyo_outstream *) out;
}

size_t
binyo_outstream_bytes_get_bytes_free(binyo_outstream *outstream, uint8_t **bytes)
{
    binyo_outstream_bytes *out;
    size_t len;

    int_safe_cast(out, outstream);
    len = binyo_buffer_get_bytes_free(out->buffer, bytes);
    xfree(out);
    return len;
}

static binyo_outstream_bytes *
int_bytes_alloc(void)
{
    binyo_outstream_bytes *ret;
    ret = ALLOC(binyo_outstream_bytes);
    memset(ret, 0, sizeof(binyo_outstream_bytes));
    ret->methods = &binyo_interface_bytes;
    return ret;
}

static ssize_t
int_bytes_write(binyo_outstream *outstream, uint8_t *buf, size_t len)
{
    binyo_outstream_bytes *out;

    int_safe_cast(out, outstream);
    return binyo_buffer_write(out->buffer, buf, len);
}

static void
int_bytes_free(binyo_outstream *outstream)
{
    binyo_outstream_bytes *out;

    if (!outstream) return;
    int_safe_cast(out, outstream);
    if (out->buffer)
	binyo_buffer_free(out->buffer);
}

