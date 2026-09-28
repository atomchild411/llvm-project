/*===---- sys/select.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX has <sys/select.h> define struct timeval, which select() takes.
 * IRIX's (6.5.7 and 6.5.22 alike) does not: struct timeval is in
 * <sys/time.h>, so code that includes only <sys/select.h> (libX11's
 * xcb_io.c) finds the type incomplete. Include <sys/time.h> too.
 */

#ifndef __CLANG_IRIX_SYS_SELECT_H
#define __CLANG_IRIX_SYS_SELECT_H

#include_next <sys/select.h>
#include <sys/time.h>

#endif /* __CLANG_IRIX_SYS_SELECT_H */
