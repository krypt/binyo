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

typedef struct binyo_instream_cache_st {
    binyo_instream_interface *methods;
    binyo_instream *inner;
    binyo_outstream *bytes;
} binyo_instream_cache;

#define int_safe_cast(out, in)		binyo_safe_cast_instream((out), (in), BINYO_INSTREAM_TYPE_CACHE, binyo_instream_cache)

static binyo_instream_cache* int_cache_alloc(void);
static ssize_t int_cache_read(binyo_instream *in, uint8_t *buf, size_t len);
static int int_cache_seek(binyo_instream *in, off_t offset, int whence);
static void int_cache_mark(binyo_instream *in);
static void int_cache_free(binyo_instream *in);

static binyo_instream_interface binyo_interface_cache = {
    BINYO_INSTREAM_TYPE_CACHE,
    int_cache_read,
    NULL,
    NULL,
    int_cache_seek,
    int_cache_mark,
    int_cache_free
};

binyo_instream *
binyo_instream_new_cache(binyo_instream *original)
{
    binyo_instream_cache *in;

    in = int_cache_alloc();
    in->inner = original;
    in->bytes = binyo_outstream_new_bytes_size(1024);
    return (binyo_instream *) in;
}

size_t
binyo_instream_cache_get_bytes(binyo_instream *instream, uint8_t **out)
{
    binyo_instream_cache *in;
    size_t ret;

    int_safe_cast(in, instream);
    ret = binyo_outstream_bytes_get_bytes_free(in->bytes, out);
    in->bytes = binyo_outstream_new_bytes_size(1024);
    return ret;
}

static binyo_instream_cache*
int_cache_alloc(void)
{
    binyo_instream_cache *ret;
    ret = ALLOC(binyo_instream_cache);
    memset(ret, 0, sizeof(binyo_instream_cache));
    ret->methods = &binyo_interface_cache;
    return ret;
}

static ssize_t
int_cache_read(binyo_instream *instream, uint8_t *buf, size_t len)
{
    ssize_t read;
    binyo_instream_cache *in;

    int_safe_cast(in, instream);

    if (!buf) return -2;

    read = binyo_instream_read(in->inner, buf, len);
    if (read > 0)
	binyo_outstream_write(in->bytes, buf, read);
    return read;
}

static int
int_cache_seek(binyo_instream *instream, off_t offset, int whence)
{
    binyo_instream_cache *in;

    int_safe_cast(in, instream);
    return binyo_instream_seek(in->inner, offset, whence);
}

static void
int_cache_mark(binyo_instream *instream)
{
    binyo_instream_cache *in;

    int_safe_cast(in, instream);
    binyo_instream_mark(in->inner);
}

static void
int_cache_free(binyo_instream *instream)
{
    binyo_instream_cache *in;

    if (!instream) return;

    int_safe_cast(in, instream);
    binyo_instream_free(in->inner);
    binyo_outstream_free(in->bytes);
}

void 
binyo_instream_cache_free_wrapper(binyo_instream *instream)
{
    binyo_instream_cache *in;

    if (!instream) return;

    int_safe_cast(in, instream);
    binyo_outstream_free(in->bytes);
    xfree(in);
}

