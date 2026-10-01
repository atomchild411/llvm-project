/*===---- time.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * CLOCK_MONOTONIC, which IRIX lacks: its clocks are CLOCK_REALTIME (settable,
 * so not monotonic), CLOCK_SGI_CYCLE (a hardware counter that wraps -- every
 * 43 seconds on an Indigo2 R10000) and CLOCK_SGI_FAST. GLib's main loop and
 * many others need a monotonic clock. clock_gettime and clock_getres are
 * routed to compiler-rt's IRIX builtins (irix/clock.c), which serve
 * CLOCK_MONOTONIC from times(), the clock ticks since boot (10 ms steps),
 * and hand every other clock to IRIX's own functions.
 *
 * CLOCK_PROCESS_CPUTIME_ID, also missing, likewise: the process's user and
 * system time from times(), in the same 10 ms steps. There is no
 * CLOCK_THREAD_CPUTIME_ID: IRIX keeps no CPU time per pthread.
 */

#ifndef __CLANG_IRIX_TIME_H
#define __CLANG_IRIX_TIME_H

#include_next <time.h>

/* Ids IRIX does not use (its are 1 to 3). */
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0x7f01
#define __IRIX_CLOCK_MONOTONIC CLOCK_MONOTONIC
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 0x7f02
#define __IRIX_CLOCK_PROCESS_CPUTIME_ID CLOCK_PROCESS_CPUTIME_ID
#endif

#if defined(__IRIX_CLOCK_MONOTONIC) || defined(__IRIX_CLOCK_PROCESS_CPUTIME_ID)
struct timespec;
#ifdef __cplusplus
extern "C" {
#endif
int clock_gettime(clockid_t, struct timespec *) __asm__("__irix_clock_gettime");
int clock_getres(clockid_t, struct timespec *) __asm__("__irix_clock_getres");
#ifdef __cplusplus
}
#endif
#endif

/* timegm (POSIX 2024, and every libc's), from irix/posix2008.c. */
#ifdef __cplusplus
extern "C" {
#endif
time_t timegm(struct tm *);
#ifdef __cplusplus
}
#endif

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
struct tm;
#ifdef __cplusplus
extern "C" {
#endif
struct tm *getdate(const char *);
char *strptime(const char *, const char *, struct tm *);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_TIME_H */
