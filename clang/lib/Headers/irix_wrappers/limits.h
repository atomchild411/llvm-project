/*===---- limits.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_LIMITS_H
#define __CLANG_IRIX_LIMITS_H

/* IRIX's HUGE_VAL reads a union in libc (__huge_val.d), which is no
 * constant expression: static initializers with it (graphviz) did not
 * compile. Define it first as the constant, which IRIX's headers keep. */
#ifndef HUGE_VAL
#define HUGE_VAL __builtin_huge_val()
#endif

#include_next <limits.h>

/* The minimum values POSIX.1-2001 added, which IRIX's <limits.h> predates
 * (glib falls back to _POSIX_HOST_NAME_MAX when HOST_NAME_MAX is not
 * defined).  They are the standard's numbers, not IRIX's actual limits:
 * those come from sysconf() and pathconf(). */
#ifndef _POSIX_HOST_NAME_MAX
#define _POSIX_HOST_NAME_MAX 255
#endif
#ifndef _POSIX_SYMLINK_MAX
#define _POSIX_SYMLINK_MAX 255
#endif
#ifndef _POSIX_SYMLOOP_MAX
#define _POSIX_SYMLOOP_MAX 8
#endif
#ifndef _POSIX_RE_DUP_MAX
#define _POSIX_RE_DUP_MAX 255
#endif
#ifndef _POSIX_CLOCKRES_MIN
#define _POSIX_CLOCKRES_MIN 20000000
#endif

/* PATH_MAX and NAME_MAX, which IRIX's <limits.h> gives only in SGI mode
 * (in POSIX they may come from pathconf() alone), while code built with
 * -D_POSIX_C_SOURCE commonly expects them, as glibc always has them:
 * IRIX's own values, wherever its <limits.h> shows POSIX names. */
#if _POSIX90 && _NO_ANSIMODE
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#endif

/* IOV_MAX: IRIX answers it only through sysconf(_SC_IOV_MAX). Code that
 * wants a constant gets the guaranteed minimum, _XOPEN_IOV_MAX -- which IRIX
 * gives only in X/Open UX and XPG5 modes, not plain XPG4 (_XOPEN_SOURCE
 * without a value: redis), where glibc has IOV_MAX too: its value there. */
#if !defined(IOV_MAX)
#if defined(_XOPEN_IOV_MAX)
#define IOV_MAX _XOPEN_IOV_MAX
#elif defined(_XOPEN_SOURCE)
#define IOV_MAX 16
#endif
#endif

/* POSIX's: a login name's length with its null, which IRIX gives without
 * it as LOGNAME_MAX (8). */
#ifndef LOGIN_NAME_MAX
#define LOGIN_NAME_MAX 9
#endif

/* C99's long long limits, which clang's <limits.h> gives only in C99 and
 * C++11, and IRIX only as LONGLONG_MIN and so on: in the GNU C89 modes and
 * C++98 too, as glibc and the BSDs give them unless strictly ANSI C (clamav
 * builds with -std=gnu90). */
#if !defined(__STRICT_ANSI__) || defined(__cplusplus)
#ifndef LLONG_MAX
#define LLONG_MAX __LONG_LONG_MAX__
#endif
#ifndef LLONG_MIN
#define LLONG_MIN (-__LONG_LONG_MAX__ - 1LL)
#endif
#ifndef ULLONG_MAX
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)
#endif
#endif

#endif /* __CLANG_IRIX_LIMITS_H */
