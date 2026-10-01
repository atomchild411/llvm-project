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

/* In SGI mode IRIX's <sys/select.h> includes all of <string.h>, for
 * FD_ZERO's memset, so every program that includes <sys/types.h> gets the
 * string functions declared (glibc's and the BSDs' do not): cabextract,
 * which defines static ones of its own, did not compile. Keep <string.h>
 * (IRIX's and our wrapper) out unless it is already in, and give FD_ZERO
 * the builtin. */
#if !defined(__STRING_H__)
#define __CLANG_IRIX_SELECT_NO_STRING
#define __STRING_H__
#define __CLANG_IRIX_STRING_H
#endif
#include_next <sys/select.h>
#ifdef __CLANG_IRIX_SELECT_NO_STRING
#undef __STRING_H__
#undef __CLANG_IRIX_STRING_H
#undef __CLANG_IRIX_SELECT_NO_STRING
#undef FD_ZERO
#define FD_ZERO(p) __builtin_memset((void *)(p), 0, sizeof(*(p)))
#endif
#include <sys/time.h>

#endif /* __CLANG_IRIX_SYS_SELECT_H */
