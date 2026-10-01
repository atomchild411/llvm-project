/*===---- sys/types.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * suseconds_t, the type of struct timeval's tv_usec: IRIX 6.5.22's
 * <sys/types.h> defines it (as long) only under XPG5 (_XOPEN5), and 6.5.7's
 * never does, though tv_usec is a long in both. Code written since assumes
 * it (libXt). Define it wherever the system header has not: everywhere but
 * a 6.5.22 root (known by its <internal/wchar_core.h>) in XPG5 mode.
 *
 * blksize_t, the type of struct stat's st_blksize (a long on IRIX): no IRIX
 * release defines it. Code written since uses it (GLib's GIO).
 *
 * With _GNU_SOURCE, _DEFAULT_SOURCE or _BSD_SOURCE, glibc's <sys/types.h>
 * also gives the BSD types (u_char, ...) and <sys/select.h>'s fd_set, as
 * IRIX's does in SGI mode; code that asks for them alongside
 * _POSIX_C_SOURCE or _XOPEN_SOURCE (Python's select module) expects them.
 */

#ifndef __CLANG_IRIX_SYS_TYPES_H
#define __CLANG_IRIX_SYS_TYPES_H

#include_next <sys/types.h>

#if !_XOPEN5 || !__has_include(<internal/wchar_core.h>)
typedef long suseconds_t;
#endif

#ifndef __CLANG_IRIX_BLKSIZE_T
#define __CLANG_IRIX_BLKSIZE_T
typedef long blksize_t;
#endif

#if defined(_GNU_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE)
#include <sys/bsd_types.h>
#include <sys/select.h>
#endif

#endif /* __CLANG_IRIX_SYS_TYPES_H */
