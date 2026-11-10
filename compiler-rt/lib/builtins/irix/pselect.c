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
// waits at once, so this emulates what programs rely on pselect() for,
// for the signals SIGMASK unblocks:
//
// - One already pending when pselect() is called is delivered, and
//   pselect() fails with EINTR without waiting, as the real one does.
//   (Before, it was handled once the mask came off and select() then
//   slept on: gmake -j hung with its children's SIGCHLD lost so.)
// - Their handlers have SA_RESTART cleared while select() waits: IRIX
//   restarts select() after an SA_RESTART handler, where pselect() must
//   fail with EINTR. Threads may wait at once: a count per signal says
//   when the last one is done and the flag goes back. In a thread that
//   has such a signal unblocked outside pselect(), a slow call may fail
//   with EINTR instead of restarting while another thread waits here.
//
// What is left is a window of a few instructions between the mask coming
// off and select() entering the kernel: a signal arriving just then is
// handled, and select() waits for the next descriptor event or the
// timeout.
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
#include <time.h>

#ifndef NSIG
#define NSIG 65
#endif

// The signals whose handlers have SA_RESTART cleared, with how many waits
// need it so and the handler it was cleared for.
static volatile int pselect_lock;
static int pselect_waiters[NSIG];
static void (*pselect_handler[NSIG])();

static void pselect_acquire(void) {
  while (__sync_lock_test_and_set(&pselect_lock, 1))
    ;
}

static void pselect_release(void) { __sync_lock_release(&pselect_lock); }

static void restart_off(int s) {
  struct sigaction sa;
  pselect_acquire();
  if (pselect_waiters[s]++ == 0) {
    pselect_handler[s] = 0;
    if (sigaction(s, 0, &sa) == 0 && (sa.sa_flags & SA_RESTART) &&
        sa.sa_handler != SIG_DFL && sa.sa_handler != SIG_IGN) {
      sa.sa_flags &= ~SA_RESTART;
      if (sigaction(s, &sa, 0) == 0)
        pselect_handler[s] = (void (*)())sa.sa_handler;
    }
  }
  pselect_release();
}

static void restart_on(int s) {
  struct sigaction sa;
  pselect_acquire();
  if (--pselect_waiters[s] == 0 && pselect_handler[s]) {
    // Not if the program has changed the handler in the meantime.
    if (sigaction(s, 0, &sa) == 0 &&
        (void (*)())sa.sa_handler == pselect_handler[s] &&
        !(sa.sa_flags & SA_RESTART)) {
      sa.sa_flags |= SA_RESTART;
      sigaction(s, &sa, 0);
    }
    pselect_handler[s] = 0;
  }
  pselect_release();
}

#pragma weak pselect
int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
            const struct timespec *timeout, const sigset_t *sigmask) {
  struct timeval tv, *tvp = 0;
  sigset_t current, unblocked, pending, saved;
  int r, s, any = 0, now = 0, saved_errno;

  if (timeout) {
    if (timeout->tv_sec < 0 || timeout->tv_nsec < 0 ||
        timeout->tv_nsec >= 1000000000L) {
      __irix_seterrno(EINVAL);
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
  if (!sigmask) {
    r = select(nfds, readfds, writefds, exceptfds, tvp);
    saved_errno = errno;
    __irix_seterrno(saved_errno);
    return r;
  }

  // The signals SIGMASK unblocks: blocked now, not in SIGMASK.
  if (sigprocmask(SIG_BLOCK, 0, &current) != 0)
    return -1;
  sigemptyset(&unblocked);
  for (s = 1; s < NSIG; s++)
    if (sigismember(&current, s) == 1 && sigismember(sigmask, s) == 0) {
      sigaddset(&unblocked, s);
      any = 1;
    }
  if (any)
    for (s = 1; s < NSIG; s++)
      if (sigismember(&unblocked, s) == 1)
        restart_off(s);

  // One of them pending already: deliver it, and fail with EINTR.
  if (any && sigpending(&pending) == 0)
    for (s = 1; s < NSIG && !now; s++)
      if (sigismember(&unblocked, s) == 1 && sigismember(&pending, s) == 1)
        now = 1;

  if (sigprocmask(SIG_SETMASK, sigmask, &saved) != 0) {
    saved_errno = errno;
    r = -1;
  } else {
    if (now) {
      r = -1;
      saved_errno = EINTR;
    } else {
      r = select(nfds, readfds, writefds, exceptfds, tvp);
      saved_errno = errno;
    }
    sigprocmask(SIG_SETMASK, &saved, 0);
  }

  if (any)
    for (s = 1; s < NSIG; s++)
      if (sigismember(&unblocked, s) == 1)
        restart_on(s);
  __irix_seterrno(saved_errno);
  return r;
}

#endif
