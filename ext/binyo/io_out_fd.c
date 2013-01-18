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

#include <unistd.h>
#include <errno.h>
#include "binyo.h"

typedef struct binyo_outstream_fd_st {
    binyo_outstream_interface *methods;
    int fd;
} binyo_outstream_fd;

#define int_safe_cast(out, in)		binyo_safe_cast_outstream((out), (in), BINYO_OUTSTREAM_TYPE_FD, binyo_outstream_fd)

static binyo_outstream_fd* int_fd_alloc(void);
static ssize_t int_fd_write(binyo_outstream *out, uint8_t *buf, size_t len);
static void int_fd_free(binyo_outstream *out);

static binyo_outstream_interface binyo_interface_fd = {
    BINYO_OUTSTREAM_TYPE_FD,
    int_fd_write,
    NULL,
    NULL,
    int_fd_free
};

binyo_outstream *
binyo_outstream_new_fd_io(VALUE value)
{
    rb_io_t *fptr;
    GetOpenFile(value, fptr);
    rb_io_check_writable(fptr);
    return binyo_outstream_new_fd(fptr->fd);
}

binyo_outstream *
binyo_outstream_new_fd(int fd)
{
    binyo_outstream_fd *out;

    out = int_fd_alloc();
    out->fd = fd;
    return (binyo_outstream *) out;
}

static binyo_outstream_fd*
int_fd_alloc(void)
{
    binyo_outstream_fd *ret;
    ret = ALLOC(binyo_outstream_fd);
    memset(ret, 0, sizeof(binyo_outstream_fd));
    ret->methods = &binyo_interface_fd;
    return ret;
}

static ssize_t
int_fd_write(binyo_outstream *outstream, uint8_t *buf, size_t len)
{
    int fd;
    ssize_t w;
    binyo_outstream_fd *out;
   
    int_safe_cast(out, outstream); 

    if (!buf) return BINYO_ERR;

    fd = out->fd;
    binyo_clear_sys_error();
    /* no need to increase out->num_written */
    w = write(fd, buf, len);
    
    if (w < 0) {
	binyo_add_io_error();
	return BINYO_ERR;
    }
    else {
    	return w;
    }
}

static void
int_fd_free(binyo_outstream *out)
{
    /* do not close the fd, should be done explicitly */
}

