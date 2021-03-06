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

#include "binyo-io-buffer.h"
#include "binyo-error.h"
#include "binyo-error-internal.h"

binyo_byte_buffer *
binyo_buffer_new(void)
{
    binyo_byte_buffer *ret;
    ret = ALLOC(binyo_byte_buffer);
    memset(ret, 0, sizeof(binyo_byte_buffer));
    return ret;
}

binyo_byte_buffer *
binyo_buffer_new_size(size_t size)
{
    binyo_byte_buffer *ret;
    ret = binyo_buffer_new();
    ret->init_size = size;
    return ret;
}

binyo_byte_buffer *
binyo_buffer_new_prealloc(uint8_t *b, size_t len)
{
    binyo_byte_buffer *ret;
    ret = binyo_buffer_new();
    ret->data = b;
    ret->limit = len;
    ret->prealloc = 1;
    return ret;
}

static const size_t BINYO_BUF_MAX = SIZE_MAX / BINYO_BYTE_BUFFER_GROWTH_FACTOR;

static int
int_buffer_grow(binyo_byte_buffer *buffer, size_t cur_len)
{
    size_t new_size;

    if (buffer->prealloc) {
	binyo_error_add("Cannot grow preallocated buffer");
	return BINYO_ERR;
    }

    if (buffer->data == NULL) {
	size_t alloc_size = buffer->init_size > cur_len ? buffer->init_size : cur_len;
	buffer->data = ALLOC_N(uint8_t, alloc_size);
	buffer->limit = alloc_size;
	return BINYO_OK;
    }

    /* avoid infinite loop for limit == 1 */
    new_size = buffer->limit == 1 ? 2 : buffer->limit;

    while (new_size - buffer->size < cur_len) {
	if (new_size >= BINYO_BUF_MAX) {
	    binyo_error_add("Cannot grow buffer");
	    return BINYO_ERR;
	}
    	new_size *= BINYO_BYTE_BUFFER_GROWTH_FACTOR;
    }

    REALLOC_N(buffer->data, uint8_t, new_size);
    buffer->limit = new_size; 
    return BINYO_OK;
}

ssize_t
binyo_buffer_write(binyo_byte_buffer *buffer, uint8_t *b, size_t len)
{
    if (!b) return BINYO_ERR;
    if (len == 0) return 0;
    if (len > SSIZE_MAX) return BINYO_ERR;

    if (buffer->limit - buffer->size < len) {
	if (int_buffer_grow(buffer, len) == BINYO_ERR)
	    return BINYO_ERR;
    }

    memcpy(buffer->data + buffer->size, b, len);
    buffer->size += len;
    return (ssize_t) len;
}

void
binyo_buffer_free_secure(binyo_byte_buffer *buffer)
{
    if (buffer && buffer->data) {
	memset(buffer->data, 0, buffer->limit);
    }
    binyo_buffer_free(buffer);
}

void
binyo_buffer_free(binyo_byte_buffer *buffer)
{
    if (!buffer) return;
    if (buffer->data && (!buffer->prealloc))
	xfree(buffer->data);
    xfree(buffer);
}

size_t
binyo_buffer_get_bytes_free(binyo_byte_buffer *buffer, uint8_t **out)
{
    size_t ret;

    if (!buffer) return 0;

    *out = buffer->data;
    ret = buffer->size;
    xfree(buffer);
    return ret;
}

