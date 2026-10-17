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

/* POSIX's struct timeval and (XSI) struct itimerval, which IRIX declares
 * only in SGI, BSD and X/Open UX modes: C99 code (-std=c99: xz in lzma-sys)
 * uses them with gettimeofday. IRIX's layout, under IRIX's own guard, which
 * <sys/resource.h> shares. */
#ifndef _TIMEVAL_T
#define _TIMEVAL_T
struct timeval {
#if _MIPS_SZLONG == 64
  __int32_t : 32;
#endif
  time_t tv_sec;
  long tv_usec;
};
#endif
#if !(_XOPEN4UX || defined(_BSD_TYPES) || defined(_BSD_COMPAT)) &&            \
    !defined(ITIMER_REAL)
#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2
#define ITIMER_MAX 3
struct itimerval {
  struct timeval it_interval;
  struct timeval it_value;
};
#endif

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

/* timeradd and timersub, the BSD and glibc macros IRIX lacks. */
#ifndef timeradd
#define timeradd(a, b, result)                                                 \
  do {                                                                         \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                              \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                           \
    if ((result)->tv_usec >= 1000000) {                                        \
      ++(result)->tv_sec;                                                      \
      (result)->tv_usec -= 1000000;                                            \
    }                                                                          \
  } while (0)
#endif
#ifndef timersub
#define timersub(a, b, result)                                                 \
  do {                                                                         \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                              \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                           \
    if ((result)->tv_usec < 0) {                                               \
      --(result)->tv_sec;                                                      \
      (result)->tv_usec += 1000000;                                            \
    }                                                                          \
  } while (0)
#endif

/* The BSDs' and glibc's timeval <-> timespec conversions (libfprint,
 * pdns-recursor), outside strict standard modes as there. */
#if (_SGIAPI || defined(_BSD_COMPAT) || defined(_BSD_SOURCE) ||               \
     defined(_DEFAULT_SOURCE) || defined(_GNU_SOURCE)) &&                     \
    !defined(TIMEVAL_TO_TIMESPEC)
#define TIMEVAL_TO_TIMESPEC(tv, ts)                                            \
  do {                                                                         \
    (ts)->tv_sec = (tv)->tv_sec;                                               \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;                                      \
  } while (0)
#define TIMESPEC_TO_TIMEVAL(tv, ts)                                            \
  do {                                                                         \
    (tv)->tv_sec = (ts)->tv_sec;                                               \
    (tv)->tv_usec = (ts)->tv_nsec / 1000;                                      \
  } while (0)
#endif

#endif /* __CLANG_IRIX_SYS_TIME_H */
