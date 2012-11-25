/*
* binyo - Fast binary IO for Ruby
*
* Copyright (C) 2012
* Martin Bosslet <martin.bosslet@googlemail.com>
* All rights reserved.
*
* See the file 'LICENSE' for further details about licensing.
*/

#if !defined(_BINYO_OS_H_)
#define _BINYO_OS_H_

#ifdef _WIN32
#define binyo_last_sys_error()	GetLastError()
#define binyo_clear_sys_error()	SetLastError(0)
#else
#define binyo_last_sys_error()	errno
#define binyo_clear_sys_error()	errno=0
#endif

#endif /* _BINYO_OS_H_ */
