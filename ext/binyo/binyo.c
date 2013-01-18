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

VALUE mBinyo;

VALUE eBinyoError;


void *
binyo_alloc(size_t size)
{
    void *mem;

    if (!(mem = malloc(size)))
	rb_memerror();
    return mem;
}

void *
binyo_alloc_n(size_t n, size_t size)
{
    void *mem;
    size_t target_size;

    if (n > 0 && n > SIZE_MAX / size)
	rb_raise(rb_eArgError, "Malloc size too large");
    target_size = n * size;
    if (!(mem = malloc(target_size)))
	rb_memerror();
    return mem;
}

void *
binyo_realloc_n(void *ptr, size_t n, size_t size)
{
    void *mem;
    size_t target_size;

    if (!ptr)
	rb_raise(rb_eArgError, "Realloc pointer is NULL");
    if (n > 0 && n > SIZE_MAX / size)
	rb_raise(rb_eArgError, "Realloc size too large");
    target_size = n * size;
    mem = realloc(ptr, target_size);
    if (!mem) {
	free(ptr);
	rb_memerror();
    }
    return mem;
}

void
binyo_free(void *ptr)
{
    if (ptr)
	free(ptr);
}

void
Init_binyo(void) 
{
    mBinyo = rb_define_module("Binyo");

    eBinyoError = rb_define_class_under(mBinyo, "BinyoError", rb_eStandardError);

    Init_binyo_io();
    Init_binyo_bytelist();
    Init_binyo_byte();
}

