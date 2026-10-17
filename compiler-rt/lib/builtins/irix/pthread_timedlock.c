//===-- irix/pthread_timedlock.c - pthread_mutex_timedlock for IRIX -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX's pthread_mutex_timedlock, which IRIX's pthreads lack (rsyslog):
// pthread_mutex_trylock until the mutex is free or the absolute
// CLOCK_REALTIME time has passed, sleeping a millisecond between tries.
// clang's IRIX <pthread.h> declares it.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <pthread.h>
#include <time.h>

#pragma weak pthread_mutex_timedlock
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abstime) {
  struct timespec now, nap = {0, 1000000L};
  int r;

  for (;;) {
    r = pthread_mutex_trylock(mutex);
    if (r != EBUSY)
      return r;
    if (!abstime || abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000L)
      return EINVAL;
    if (clock_gettime(CLOCK_REALTIME, &now) != 0)
      return errno;
    if (now.tv_sec > abstime->tv_sec ||
        (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec))
      return ETIMEDOUT;
    nanosleep(&nap, NULL);
  }
}

#endif // defined(__sgi)
