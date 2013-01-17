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

