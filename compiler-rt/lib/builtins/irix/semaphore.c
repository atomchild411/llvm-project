//===-- irix/semaphore.c - sem_timedwait for IRIX -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX's sem_timedwait, which IRIX's libc lacks (it has sem_wait,
// sem_trywait and sem_post); clang's IRIX <semaphore.h> declares it. Weak,
// so a program's own copy is kept.
//
// IRIX has no timed wait to build it on: it tries the semaphore and, while
// it is taken, sleeps at most a millisecond before trying again, until the
// absolute CLOCK_REALTIME deadline. A post is seen within that millisecond.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#pragma weak sem_timedwait

int sem_timedwait(sem_t *sem, const struct timespec *abstime) {
  for (;;) {
    struct timeval now;
    struct timespec nap;
    long long left_ns;
    if (sem_trywait(sem) == 0)
      return 0;
    if (errno != EAGAIN)
      return -1;
    // Checked only when it would block, as POSIX has it.
    if (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000) {
      __irix_seterrno(EINVAL);
      return -1;
    }
    gettimeofday(&now, 0);
    left_ns = ((long long)abstime->tv_sec - now.tv_sec) * 1000000000LL +
              (abstime->tv_nsec - (long long)now.tv_usec * 1000);
    if (left_ns <= 0) {
      __irix_seterrno(ETIMEDOUT);
      return -1;
    }
    nap.tv_sec = 0;
    nap.tv_nsec = left_ns < 1000000 ? (long)left_ns : 1000000;
    if (nanosleep(&nap, 0) != 0 && errno == EINTR)
      return -1;
  }
}

#endif // defined(__sgi)
