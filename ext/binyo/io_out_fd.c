/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
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

