/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012
* Martin Bosslet <martin.bosslet@googlemail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#ifndef _BINYO_ERROR_H_
#define _BINYO_ERROR_H_

void binyo_error_add(const char *format, ...);

int binyo_has_errors(void);
int binyo_error_message(char *buf, int buf_len);
VALUE binyo_error_create(VALUE exception_class, const char *format, ...);
void binyo_error_raise(VALUE exception_class, const char *format, ...);
void binyo_error_clear(void);

#endif /* KRYPT_ERROR_H */

