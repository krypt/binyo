/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#if !defined(_BYTE_H_)
#define _BYTE_H_

extern VALUE cBinyoByte;

VALUE binyo_byte_new(uint8_t input);

void Init_binyo_byte(void);

#endif /* _BYTE_H_ */


