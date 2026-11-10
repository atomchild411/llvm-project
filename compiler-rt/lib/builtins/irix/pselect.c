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
// timeout as a timespec. IRIX's libc and the libraries beside it have no
// pselect, ppoll or other call that sets the mask and waits at once.
//
// Setting the mask and then calling select() loses a signal that was pending
// when the mask was set: sigprocmask() delivers it (the handler runs) and
// select() then sleeps with nothing left to interrupt it. A program that
// waits for SIGCHLD only through pselect(), as GNU make does, hangs. So the
// wait is done with every signal blocked instead, and signals are looked for
// with sigpending():
//
//   - block all signals, remembering the mask;
//   - if a signal that SIGMASK does not block is pending, set SIGMASK so the
//     handler runs, restore the mask and return -1/EINTR, as a real pselect
//     does for a signal that was already pending;
//   - otherwise select() for at most one clock tick (10 ms) and look again,
//     until a descriptor is ready, an error, or the timeout has passed.
//
// A signal cannot be lost: it stays pending until the check sees it. What is
// given up is promptness: one that arrives during the wait is delivered at
// the next tick boundary, up to one tick (10 ms) plus the time select()
// takes to return (also tick-rounded, so up to about 20 ms in all), instead
// of at once. A tick is the finest wait select() offers, so a shorter slice
// would only spin. The wait costs 100 wake-ups a second while idle.
//
// What is still not as with a real pselect:
//   - Signals that cannot be blocked (SIGKILL, SIGSTOP) act at once, as ever.
//   - The timeout is measured with times(), in ticks, and is never short: the
//     call returns after at least TIMEOUT and at most about two ticks more.
//   - A signal that arrives after the last check but before the mask is
//     restored is delivered by the restore, when the descriptors were ready;
//     the call then returns the ready count, not EINTR. (A real pselect can
//     do the same when a descriptor becomes ready first.)
//   - With a null SIGMASK this is plain select().
//   - pending is the calling thread's view; if IRIX leaves a signal sent to
//     the process pending on another thread, this thread does not see it. Not
//     tested with several threads.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

// A signal pending that SIGMASK would let through?
static int deliverable(const sigset_t *pending, const sigset_t *mask) {
  int s;
  for (s = 1; s < NSIG; ++s)
    if (sigismember(pending, s) == 1 && sigismember(mask, s) != 1)
      return 1;
  return 0;
}

#pragma weak pselect
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask) {
  struct timeval tv;
  sigset_t saved, all, pending;
  fd_set r0, w0, x0;
  unsigned long long want = 0; // ticks to wait, when there is a timeout
  unsigned long start = 0, hz = 0;
  struct tms t;
  int r, saved_errno;

  if (timeout) {
    if (timeout->tv_sec < 0 || timeout->tv_nsec < 0 ||
        timeout->tv_nsec >= 1000000000L) {
      __irix_seterrno(EINVAL);
      return -1;
    }
  }
  if (!sigmask) {
    struct timeval *tvp = 0;
    if (timeout) {
      tv.tv_sec = timeout->tv_sec;
      tv.tv_usec = (timeout->tv_nsec + 999) / 1000; // never shorter than asked
      if (tv.tv_usec == 1000000) {
        tv.tv_sec++;
        tv.tv_usec = 0;
      }
      tvp = &tv;
    }
    return select(nfds, readfds, writefds, exceptfds, tvp);
  }

  hz = (unsigned long)sysconf(_SC_CLK_TCK);
  if (hz == 0 || hz > 1000000)
    hz = 100;
  if (timeout) {
    // whole ticks, rounded up, and one more because start is the tick we are
    // in, which may be nearly over
    want = ((unsigned long long)timeout->tv_sec * hz) +
           ((unsigned long long)timeout->tv_nsec * hz + 999999999ULL) /
               1000000000ULL +
           1;
    start = (unsigned long)times(&t);
  }
  if (readfds)
    r0 = *readfds;
  if (writefds)
    w0 = *writefds;
  if (exceptfds)
    x0 = *exceptfds;

  sigfillset(&all);
  if (sigprocmask(SIG_BLOCK, &all, &saved) != 0)
    return -1;
  for (;;) {
    unsigned long long left = 1;
    if (sigpending(&pending) == 0 && deliverable(&pending, sigmask)) {
      // IRIX does not run the handler on return from sigprocmask: the signal
      // is delivered at the next system call that can see it (which is why
      // the select() after a sigprocmask() sees it, or with SA_RESTART
      // restarts and goes on sleeping). A select() that cannot wait is such
      // a call, and cannot hang if another thread took the signal.
      sigprocmask(SIG_SETMASK, sigmask, 0);
      tv.tv_sec = 0;
      tv.tv_usec = 0;
      select(0, 0, 0, 0, &tv); // the handler runs here
      sigprocmask(SIG_SETMASK, &saved, 0);
      __irix_seterrno(EINTR);
      return -1;
    }
    if (timeout) {
      unsigned long long done = (unsigned long)times(&t) - start;
      if (done > 0xffffffffULL / 2) // the tick count went back: start over
        done = 0;
      left = done >= want ? 0 : want - done;
    }
    // one tick, or nothing left to wait for (a final look at the descriptors)
    tv.tv_sec = 0;
    tv.tv_usec = left ? (long)(1000000UL / hz) : 0;
    if (readfds)
      *readfds = r0;
    if (writefds)
      *writefds = w0;
    if (exceptfds)
      *exceptfds = x0;
    r = select(nfds, readfds, writefds, exceptfds, &tv);
    if (r != 0 || (timeout && left == 0))
      break;
  }
  saved_errno = errno;
  sigprocmask(SIG_SETMASK, &saved, 0);
  __irix_seterrno(saved_errno);
  return r;
}

#endif
