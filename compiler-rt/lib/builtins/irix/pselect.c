//===-- irix/pselect.c - pselect for IRIX ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// pselect(), which IRIX lacks (clang's IRIX <sys/select.h> declares it):
// select() with the signal mask set to SIGMASK for the duration and the
// timeout as a timespec. IRIX has no system call that sets the mask and
// waits at once, so a signal that arrives between the two is handled
// before select() starts rather than interrupting it: a program that waits
// for a signal only through pselect() may wait until the next descriptor
// event or the timeout. Programs that also learn of events through the
// descriptors (ninja: child output pipes) do not notice.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask) {
  struct timeval tv, *tvp = 0;
  sigset_t saved;
  int r, saved_errno;

  if (timeout) {
    if (timeout->tv_sec < 0 || timeout->tv_nsec < 0 ||
        timeout->tv_nsec >= 1000000000L) {
      errno = EINVAL;
      return -1;
    }
    tv.tv_sec = timeout->tv_sec;
    tv.tv_usec = (timeout->tv_nsec + 999) / 1000; // never shorter than asked
    if (tv.tv_usec == 1000000) {
      tv.tv_sec++;
      tv.tv_usec = 0;
    }
    tvp = &tv;
  }
  if (sigmask && sigprocmask(SIG_SETMASK, sigmask, &saved) != 0)
    return -1;
  r = select(nfds, readfds, writefds, exceptfds, tvp);
  saved_errno = errno;
  if (sigmask)
    sigprocmask(SIG_SETMASK, &saved, 0);
  errno = saved_errno;
  return r;
}

#endif
