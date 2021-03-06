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

typedef struct binyo_instream_fd_st {
    binyo_instream_interface *methods;
    int fd;
} binyo_instream_fd;

#define int_safe_cast(out, in)	binyo_safe_cast_instream((out), (in), BINYO_INSTREAM_TYPE_FD, binyo_instream_fd)
    
static binyo_instream_fd *int_fd_alloc(void);
static ssize_t int_fd_read(binyo_instream *in, uint8_t *buf, size_t len);
static ssize_t int_fd_gets(binyo_instream *in, char *line, size_t len);
static int int_fd_seek(binyo_instream *in, off_t offset, int whence);
static void int_fd_free(binyo_instream *in);

static binyo_instream_interface binyo_interface_fd = {
    BINYO_INSTREAM_TYPE_FD,
    int_fd_read,
    NULL,
    int_fd_gets,
    int_fd_seek,
    NULL,
    int_fd_free
};

binyo_instream *
binyo_instream_new_fd_io(VALUE value)
{
    rb_io_t *fptr;
    GetOpenFile(value, fptr);
    rb_io_check_byte_readable(fptr);
    return binyo_instream_new_fd(fptr->fd);
}

binyo_instream *
binyo_instream_new_fd(int fd)
{
    binyo_instream_fd *in;

    in = int_fd_alloc();
    in->fd = fd;
    return (binyo_instream *) in;
}

static binyo_instream_fd*
int_fd_alloc(void)
{
    binyo_instream_fd *ret;
    ret = ALLOC(binyo_instream_fd);
    memset(ret, 0, sizeof(binyo_instream_fd));
    ret->methods = &binyo_interface_fd;
    return ret;
}

static ssize_t
int_fd_read(binyo_instream *instream, uint8_t *buf, size_t len)
{
    int fd;
    ssize_t r;
    binyo_instream_fd *in;

    int_safe_cast(in, instream);
    if (!buf) return BINYO_ERR;

    fd = in->fd;
    binyo_clear_sys_error();
    r = read(fd, buf, len);
    
    if (r == -1) {
	binyo_add_io_error();
	return BINYO_ERR; 
    }
    else if (r == 0) {
	return BINYO_IO_EOF;
    }
    else {
    	return r;
    }
}

static ssize_t
int_fd_gets(binyo_instream *instream, char *line, size_t len)
{
    int fd;
    binyo_instream_fd *in;
    ssize_t ret = 0, r = 0;
    char *p = line;
    char *end = line + len;

    int_safe_cast(in, instream);
    if (!line) return BINYO_ERR;

    fd = in->fd;
    binyo_clear_sys_error();

    while ( (p < end) &&
	    ((r = read(fd, p, 1)) == 1) &&
	    (*p != '\n') ) {
	    p++;
	    ret++;
    }

    if (r == -1) {
	return BINYO_ERR;
    }
    
    if (ret == 0 && r == 0)
	return BINYO_IO_EOF;

    if (*p == '\n' && *(p - 1) == '\r')
	ret--;

    return ret;
}

static int
int_fd_seek(binyo_instream *instream, off_t offset, int whence)
{
    int fd;
    long off;
    binyo_instream_fd *in;

    int_safe_cast(in, instream);
    fd = in->fd;
    off = lseek(fd, offset, whence);

    if (off == -1) 
	return BINYO_ERR;
    return BINYO_OK;
}

static void
int_fd_free(binyo_instream *instream)
{
    /* do not close the fd, should be done explicitly */
}

