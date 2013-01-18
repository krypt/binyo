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

VALUE cBinyoByte;

#define int_byte_get(obj, b)					\
do { 								\
    Data_Get_Struct((obj), uint8_t, (b));			\
    if (!(b)) { 						\
	rb_raise(eBinyoError, "Uninitialized byte");		\
    } 								\
} while (0)

static void
int_byte_free(uint8_t *b)
{
    if (b) BINYO_FREE(b);
}

static VALUE
binyo_byte_alloc(VALUE klass)
{
    VALUE obj;

    obj = Data_Wrap_Struct(klass, 0, int_byte_free, 0);
    return obj;
}

VALUE
binyo_byte_new(uint8_t input)
{
    VALUE obj;
    uint8_t *b;

    b = BINYO_ALLOC(uint8_t);
    *b = input;
    obj = Data_Wrap_Struct(cBinyoByte, 0, int_byte_free, b);
    return obj;
}

static VALUE
binyo_byte_initialize(VALUE self, VALUE byteval)
{
    uint8_t *b;

    b = BINYO_ALLOC(uint8_t);
    *b = NUM2INT(byteval) & 0xff;
    DATA_PTR(self) = b;
    return self;
}

static VALUE
binyo_byte_shiftr(VALUE self, VALUE vshift)
{
    uint8_t *b;

    int_byte_get(self, b);
    return binyo_byte_new(*b >> (NUM2INT(vshift)));
}

static VALUE
binyo_byte_and(VALUE self, VALUE vmask)
{
    uint8_t *b;

    int_byte_get(self, b);
    return binyo_byte_new(*b & (NUM2INT(vmask)));
}

static VALUE
binyo_byte_to_i(VALUE self)
{
    uint8_t *b;

    int_byte_get(self, b);
    return INT2NUM(*b);
}

void
Init_binyo_byte(void)
{
    cBinyoByte = rb_define_class_under(mBinyo, "Byte", rb_cObject);

    rb_define_alloc_func(cBinyoByte, binyo_byte_alloc);
    rb_define_method(cBinyoByte, "initialize", binyo_byte_initialize, 1);
    rb_define_method(cBinyoByte, ">>", binyo_byte_shiftr, 1);
    rb_define_method(cBinyoByte, "&", binyo_byte_and, 1);
    rb_define_method(cBinyoByte, "to_i", binyo_byte_to_i, 0);
}

