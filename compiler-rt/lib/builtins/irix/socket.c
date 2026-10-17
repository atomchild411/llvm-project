//===-- irix/socket.c - POSIX recvmsg for IRIX ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// recvmsg for the POSIX struct msghdr that clang's IRIX <sys/socket.h>
// declares (IRIX's X/Open one): IRIX's __xpg4_recvmsg, except that the
// kernel returns a bit in msg_flags, 0x40000000, that no MSG_ flag names;
// it is cleared, so msg_flags holds only the flags POSIX defines.
//
// getsockname and getpeername: IRIX's, except that for an unnamed AF_UNIX
// socket (a socketpair's) IRIX returns a length of 0 and no family; report
// the family alone, AF_UNIX, as Linux and the BSDs do. (Every other family
// has an address, even unbound.)
//
// send, sendto and sendmsg with MSG_NOSIGNAL (Linux's and the BSDs'), which
// IRIX lacks: the wrapper defines it to 0x20000, a bit IRIX does not use, and
// binds the three here. With it, SIGPIPE is blocked for the call, and one the
// call raised (EPIPE, not pending before) is taken off again with
// sigtimedwait, so the program gets EPIPE and no signal; the mask is then
// restored. Without it they are IRIX's own (sendmsg its X/Open one).
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include "irix_errno.h"
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

extern ssize_t __xpg4_recvmsg(int, struct msghdr *, int);

ssize_t __irix_recvmsg(int s, struct msghdr *msg, int flags) {
  ssize_t n = __xpg4_recvmsg(s, msg, flags);
  if (n >= 0)
    msg->msg_flags &= ~0x40000000;
  return n;
}

extern int __xpg4_getsockname(int, struct sockaddr *, size_t *);
extern int __xpg4_getpeername(int, struct sockaddr *, size_t *);

static int unnamed_is_unix(int r, struct sockaddr *addr, size_t given,
                           size_t *len) {
  if (r == 0 && *len == 0 && addr && given >= sizeof(addr->sa_family)) {
    addr->sa_family = AF_UNIX;
    *len = sizeof(addr->sa_family);
  }
  return r;
}

// The <sys/socket.h> wrapper passes socklen_t *, 32 bits like size_t on n32.
int __irix_getsockname(int s, struct sockaddr *addr, size_t *len) {
  size_t given = *len;
  return unnamed_is_unix(__xpg4_getsockname(s, addr, len), addr, given, len);
}

int __irix_getpeername(int s, struct sockaddr *addr, size_t *len) {
  size_t given = *len;
  return unnamed_is_unix(__xpg4_getpeername(s, addr, len), addr, given, len);
}

#define NOSIGNAL 0x20000

extern ssize_t __irix_libc_send_fn(int, const void *, size_t, int)
    __asm__("send");
extern ssize_t __irix_libc_sendto_fn(int, const void *, size_t, int,
                                     const struct sockaddr *, size_t)
    __asm__("sendto");
extern ssize_t __xpg4_sendmsg(int, const struct msghdr *, int);

struct pipe_guard {
  sigset_t old;
  int was_pending;
};

static void pipe_guard_on(struct pipe_guard *g) {
  sigset_t set, pending;
  sigemptyset(&set);
  sigaddset(&set, SIGPIPE);
  sigprocmask(SIG_BLOCK, &set, &g->old);
  g->was_pending = sigpending(&pending) == 0 && sigismember(&pending, SIGPIPE);
}

// (errno: the thread's, *__oserror(), which libc set; both are restored.)
static void pipe_guard_off(struct pipe_guard *g, ssize_t r) {
  int saved = *__oserror();
  if (r < 0 && saved == EPIPE && !g->was_pending) {
    sigset_t set, pending;
    if (sigpending(&pending) == 0 && sigismember(&pending, SIGPIPE)) {
      struct timespec zero = {0, 0};
      sigemptyset(&set);
      sigaddset(&set, SIGPIPE);
      sigtimedwait(&set, 0, &zero);
    }
  }
  sigprocmask(SIG_SETMASK, &g->old, 0);
  __irix_seterrno(saved);
}

ssize_t __irix_send(int s, const void *buf, size_t len, int flags) {
  if (!(flags & NOSIGNAL))
    return __irix_libc_send_fn(s, buf, len, flags);
  struct pipe_guard g;
  pipe_guard_on(&g);
  ssize_t r = __irix_libc_send_fn(s, buf, len, flags & ~NOSIGNAL);
  pipe_guard_off(&g, r);
  return r;
}

ssize_t __irix_sendto(int s, const void *buf, size_t len, int flags,
                      const struct sockaddr *to, size_t tolen) {
  if (!(flags & NOSIGNAL))
    return __irix_libc_sendto_fn(s, buf, len, flags, to, tolen);
  struct pipe_guard g;
  pipe_guard_on(&g);
  ssize_t r = __irix_libc_sendto_fn(s, buf, len, flags & ~NOSIGNAL, to, tolen);
  pipe_guard_off(&g, r);
  return r;
}

ssize_t __irix_sendmsg(int s, const struct msghdr *msg, int flags) {
  if (!(flags & NOSIGNAL))
    return __xpg4_sendmsg(s, msg, flags);
  struct pipe_guard g;
  pipe_guard_on(&g);
  ssize_t r = __xpg4_sendmsg(s, msg, flags & ~NOSIGNAL);
  pipe_guard_off(&g, r);
  return r;
}

#endif // defined(__sgi)
