//===-- irix/pthread_np.c - pthread_cond_timedwait_relative_np for IRIX ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// macOS's pthread_cond_timedwait_relative_np, for code (GLib's GCond) that
// needs a timed wait that does not depend on the date: IRIX has only the
// absolute CLOCK_REALTIME form and no pthread_condattr_setclock. The relative
// time is added to the current CLOCK_REALTIME and handed to IRIX's
// pthread_cond_timedwait, so only a date change during the wait itself
// lengthens or shortens it. clang's IRIX <pthread.h> declares it.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <pthread.h>
#include <time.h>

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond,
                                       pthread_mutex_t *mutex,
                                       const struct timespec *rel) {
  struct timespec abs;

  if (!rel || rel->tv_nsec < 0 || rel->tv_nsec >= 1000000000L)
    return EINVAL;
  if (clock_gettime(CLOCK_REALTIME, &abs) != 0)
    return errno;
  abs.tv_sec += rel->tv_sec;
  abs.tv_nsec += rel->tv_nsec;
  if (abs.tv_nsec >= 1000000000L) {
    abs.tv_sec++;
    abs.tv_nsec -= 1000000000L;
  }
  return pthread_cond_timedwait(cond, mutex, &abs);
}

#endif // defined(__sgi)
