/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012-2013
* Martin Bosslet <martin.bosslet@gmail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#ifndef _BINYO_MISSING_H_
#define _BINYO_MISSING_H_

#ifndef HAVE_RB_IO_CHECK_BYTE_READABLE 
#define rb_io_check_byte_readable(fptr)		rb_io_check_readable(fptr)
#endif

#endif /* BINYO_MISSING_H */

