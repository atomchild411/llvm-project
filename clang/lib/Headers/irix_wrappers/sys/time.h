/*===---- sys/time.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX declares gettimeofday, utimes, getitimer and setitimer only in SGI,
 * BSD and X/Open UX modes (6.5.22 also in X/Open 5 mode); POSIX code
 * (_POSIX_C_SOURCE alone) finds none of them, nor select, which POSIX has
 * <sys/time.h> make visible too. They are all in libc.
 */

#ifndef __CLANG_IRIX_SYS_TIME_H
#define __CLANG_IRIX_SYS_TIME_H

#include_next <sys/time.h>
#include <sys/select.h>

#if !_SGIAPI && !_XOPEN4UX && !defined(_BSD_TYPES) &&                         \
    !defined(_BSD_COMPAT) &&                                                  \
    !(__has_include(<internal/wchar_core.h>) && _XOPEN5)
struct itimerval;
struct timeval;
#ifdef __cplusplus
extern "C" {
#endif
int gettimeofday(struct timeval *, void *);
int utimes(const char *, const struct timeval *); /* [2] */
int getitimer(int, struct itimerval *);
int setitimer(int, const struct itimerval *, struct itimerval *);
#ifdef __cplusplus
}
#endif
#endif

/* BSD's, in IRIX's libc, which its header declares only in SGI mode:
 * declared here outside it, with IRIX's own prototypes (glibc gives them
 * with _DEFAULT_SOURCE; Python, among others, uses them). */
#if !_SGIAPI
struct timeval;
#ifdef __cplusplus
extern "C" {
#endif
int adjtime(struct timeval *, struct timeval *);
int settimeofday(struct timeval *, ...);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_SYS_TIME_H */
