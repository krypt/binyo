/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
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


