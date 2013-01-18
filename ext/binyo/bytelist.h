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

#if !defined(_BYTELIST_H_)
#define _BYTELIST_H_

extern VALUE cBinyoByteList;

struct binyo_bytelist_st;
typedef struct binyo_bytelist_st binyo_bytelist;

binyo_bytelist * binyo_bytelist_new_size(size_t size);
binyo_bytelist * binyo_bytelist_new_size_default(size_t size, uint8_t default_value);
binyo_bytelist * binyo_bytelist_new_bytes(uint8_t *bytes, size_t len);
void binyo_bytelist_free(binyo_bytelist *bytelist);

void Init_binyo_bytelist(void);

#endif /* _BYTELIST_H_ */


