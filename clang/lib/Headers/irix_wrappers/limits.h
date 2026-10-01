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

#endif /* __CLANG_IRIX_LIMITS_H */
