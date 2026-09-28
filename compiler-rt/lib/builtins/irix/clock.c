//===-- irix/clock.c - CLOCK_MONOTONIC for IRIX ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// clock_gettime and clock_getres with CLOCK_MONOTONIC, which IRIX lacks;
// clang's IRIX <time.h> routes calls here (as __irix_clock_gettime and
// __irix_clock_getres) and defines CLOCK_MONOTONIC to an id IRIX does not
// use. Every other clock goes to IRIX's own functions.
//
// The monotonic time is times(), the clock ticks since boot: it never goes
// back when the date is set, and its 32 bits of ticks (100 a second) last
// 497 days. Its steps are 10 ms. CLOCK_SGI_CYCLE is finer but wraps -- every
// 43 seconds on an Indigo2 R10000 -- so it cannot serve alone.
//
// CLOCK_PROCESS_CPUTIME_ID, which IRIX also lacks, is the user and system
// time times() reports for the process, in the same steps.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

// IRIX's own, under their own names (the <time.h> wrapper renames the
// declarations a program sees).
extern int __irix_libc_clock_gettime(clockid_t, struct timespec *)
    __asm__("clock_gettime");
extern int __irix_libc_clock_getres(clockid_t, struct timespec *)
    __asm__("clock_getres");

#if !defined(__IRIX_CLOCK_MONOTONIC) || !defined(__IRIX_CLOCK_PROCESS_CPUTIME_ID)
#error "clang's IRIX <time.h> wrapper defines the __IRIX_CLOCK_ ids"
#endif

static long ticks_per_second(void) {
  static long hz;
  if (!hz) {
    hz = sysconf(_SC_CLK_TCK);
    if (hz <= 0)
      hz = 100;
  }
  return hz;
}

int __irix_clock_gettime(clockid_t id, struct timespec *ts) {
  if (id == __IRIX_CLOCK_MONOTONIC || id == __IRIX_CLOCK_PROCESS_CPUTIME_ID) {
    struct tms t;
    unsigned long ticks = (unsigned long)times(&t);
    long hz = ticks_per_second();
    if (!ts) {
      errno = EFAULT;
      return -1;
    }
    if (id == __IRIX_CLOCK_PROCESS_CPUTIME_ID)
      ticks = (unsigned long)t.tms_utime + (unsigned long)t.tms_stime;
    ts->tv_sec = (time_t)(ticks / (unsigned long)hz);
    ts->tv_nsec = (long)(ticks % (unsigned long)hz) * (1000000000L / hz);
    return 0;
  }
  return __irix_libc_clock_gettime(id, ts);
}

int __irix_clock_getres(clockid_t id, struct timespec *ts) {
  if (id == __IRIX_CLOCK_MONOTONIC || id == __IRIX_CLOCK_PROCESS_CPUTIME_ID) {
    if (ts) {
      ts->tv_sec = 0;
      ts->tv_nsec = 1000000000L / ticks_per_second();
    }
    return 0;
  }
  return __irix_libc_clock_getres(id, ts);
}

#endif // defined(__sgi)
