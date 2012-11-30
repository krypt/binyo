/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012
* Martin Bosslet <martin.bosslet@googlemail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#ifndef _BINYO_ERROR_INTERNAL_H_
#define _BINYO_ERROR_INTERNAL_H_

void binyo_add_io_error(void);
void binyo_error_add(const char *format, ...);

VALUE binyo_error_create(VALUE exception_class, const char *format, ...);
void binyo_error_raise(VALUE exception_class, const char *format, ...);

#endif /* BINYO_ERROR_INTERNAL_H */

