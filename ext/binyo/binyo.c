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

