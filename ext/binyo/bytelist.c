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

VALUE cBinyoByteList;

struct binyo_bytelist_st
{
    uint8_t *bytes;
    size_t len;
    size_t limit;
}; 

#define int_bytelist_get(obj, bytelist)				\
do { 								\
    Data_Get_Struct((obj), binyo_bytelist, (bytelist));		\
    if (!(bytelist)) { 						\
	rb_raise(eBinyoError, "Uninitialized byte list");	\
    } 								\
} while (0)

static binyo_bytelist *
int_bytelist_new()
{
    binyo_bytelist *bytelist;

    bytelist = BINYO_ALLOC(binyo_bytelist);
    memset(bytelist, 0, sizeof(binyo_bytelist));
    return bytelist;
}

binyo_bytelist *
binyo_bytelist_new_size(size_t size)
{
    binyo_bytelist *bytelist = int_bytelist_new();

    bytelist->bytes = BINYO_ALLOC_N(uint8_t, size);
    bytelist->limit = size;
    return bytelist;
}

binyo_bytelist *
binyo_bytelist_new_size_default(size_t size, uint8_t default_value)
{
    size_t i;
    binyo_bytelist *bytelist = binyo_bytelist_new_size(size);

    for (i=0; i < size; i++) {
	bytelist->bytes[i] = default_value;
    }

    bytelist->len = size;
    return bytelist;
}

binyo_bytelist *
binyo_bytelist_new_bytes(uint8_t *bytes, size_t len)
{
    binyo_bytelist *bytelist = binyo_bytelist_new_size(len);

    memcpy(bytelist->bytes, bytes, len);
    bytelist->len = len;
    return bytelist;
}

void
binyo_bytelist_free(binyo_bytelist *bytelist)
{
    if (bytelist) {
       if (bytelist->bytes)
	   BINYO_FREE(bytelist->bytes);
       BINYO_FREE(bytelist);
    }
}

static VALUE
binyo_bytelist_alloc(VALUE klass)
{
    VALUE obj;

    obj = Data_Wrap_Struct(klass, 0, binyo_bytelist_free, 0);
    return obj;
}

static VALUE
binyo_bytelist_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE vfirst;
    VALUE vsize = Qnil;
    binyo_bytelist *bytelist;
    
    rb_scan_args(argc, argv, "11", &vfirst, &vsize);

    if (NIL_P(vfirst))
	rb_raise(rb_eArgError, "First argument may not be nil");
    if (argc == 2 && NIL_P(vsize))
	rb_raise(rb_eArgError, "Size argument may not be nil");

    if (TYPE(vfirst) == T_STRING) {
	bytelist = binyo_bytelist_new_bytes((uint8_t *) RSTRING_PTR(vfirst), (size_t) RSTRING_LEN(vfirst));
    } else {
	bytelist = binyo_bytelist_new_size((size_t) NUM2LONG(vfirst));
    }

    DATA_PTR(self) = bytelist;
    return self;
}

static VALUE
binyo_bytelist_get_index(VALUE self, VALUE vidx)
{
    binyo_bytelist *bytelist;
    size_t i = (size_t) NUM2LONG(vidx);

    int_bytelist_get(self, bytelist);

    if (i >= bytelist->len)
	rb_raise(eBinyoError, "Index out of bounds: %lu", i); /* TODO treat it like Array */

    return INT2NUM(bytelist->bytes[i]);
}

static VALUE
binyo_bytelist_set_index(VALUE self, VALUE vidx, VALUE byteval)
{
    binyo_bytelist *bytelist;
    size_t i = (size_t) NUM2LONG(vidx);

    int_bytelist_get(self, bytelist);

    if (i >= bytelist->limit)
	rb_raise(eBinyoError, "Index out of bounds: %lu", i); /* TODO treat it like Array */

    bytelist->bytes[i] = NUM2INT(byteval) & 0xff;
    if (i >= bytelist->len)
	bytelist->len = i + 1;

    return byteval;
}

static VALUE
binyo_bytelist_push(VALUE self, VALUE byteval)
{
    binyo_bytelist *bytelist;

    int_bytelist_get(self, bytelist);

    if (bytelist->len == bytelist->limit)
	rb_raise(eBinyoError, "Cannot append to ByteList"); /* TODO auto-resize */

    bytelist->bytes[bytelist->len++] = NUM2INT(byteval) & 0xff;
    return self;
}

static VALUE
binyo_bytelist_each(VALUE self)
{
    binyo_bytelist *bytelist;
    size_t i;

    if (!rb_block_given_p()) {
	/* TODO */
	rb_raise(eBinyoError, "No block given");
    }

    int_bytelist_get(self, bytelist);

    for (i=0; i < bytelist->len; i++) {
	rb_yield(INT2NUM(bytelist->bytes[i]));
    }

    return self;
}

static VALUE
binyo_bytelist_to_s(VALUE self)
{
    binyo_bytelist *bytelist;

    int_bytelist_get(self, bytelist);

    return rb_str_new((const char *)bytelist->bytes, (long) bytelist->len);
}

void
Init_binyo_bytelist(void)
{
    cBinyoByteList = rb_define_class_under(mBinyo, "ByteList", rb_cObject);

    rb_define_alloc_func(cBinyoByteList, binyo_bytelist_alloc);
    rb_define_method(cBinyoByteList, "initialize", binyo_bytelist_initialize, -1);
    rb_define_method(cBinyoByteList, "each", binyo_bytelist_each, 0);
    rb_define_method(cBinyoByteList, "[]", binyo_bytelist_get_index, 1);
    rb_define_method(cBinyoByteList, "[]=", binyo_bytelist_set_index, 2);
    rb_define_method(cBinyoByteList, "<<", binyo_bytelist_push, 1);
    rb_define_method(cBinyoByteList, "to_s", binyo_bytelist_to_s, 0);
}

