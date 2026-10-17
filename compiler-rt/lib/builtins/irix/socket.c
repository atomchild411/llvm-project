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
// it is cleared, so msg_flags holds only the flags POSIX defines. With
// Linux's MSG_CMSG_CLOEXEC (0x40000 in the wrapper), the descriptors that
// came with SCM_RIGHTS are made close-on-exec after the call.
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
// socket, socketpair and accept4 with SOCK_NONBLOCK and SOCK_CLOEXEC, which
// IRIX lacks: the wrapper defines them to the BSDs' values and binds the
// three here. The descriptors are opened without them, then given
// O_NONBLOCK and FD_CLOEXEC with fcntl (not atomic: a fork in another thread
// just then can inherit one). Without them socket and socketpair are IRIX's.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include "irix_errno.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

extern ssize_t __xpg4_recvmsg(int, struct msghdr *, int);

#define CMSG_CLOEXEC 0x40000

// Every descriptor in the SCM_RIGHTS messages of msg made close-on-exec.
// The kernel packs the messages with no padding past 4-byte alignment.
static void rights_cloexec(struct msghdr *msg) {
  char *p = (char *)msg->msg_control;
  size_t left = msg->msg_controllen;
  while (p && left >= sizeof(struct cmsghdr)) {
    struct cmsghdr *c = (struct cmsghdr *)p;
    size_t len = c->cmsg_len, step;
    if (len < sizeof(struct cmsghdr) || len > left)
      break;
    if (c->cmsg_level == SOL_SOCKET && c->cmsg_type == SCM_RIGHTS) {
      int *fds = (int *)(c + 1);
      size_t i, n = (len - sizeof(struct cmsghdr)) / sizeof(int);
      for (i = 0; i < n; i++)
        fcntl(fds[i], F_SETFD, FD_CLOEXEC);
    }
    step = (len + 3) & ~(size_t)3;
    if (step >= left)
      break;
    p += step;
    left -= step;
  }
}

ssize_t __irix_recvmsg(int s, struct msghdr *msg, int flags) {
  ssize_t n = __xpg4_recvmsg(s, msg, flags & ~CMSG_CLOEXEC);
  if (n >= 0) {
    msg->msg_flags &= ~0x40000000;
    if (flags & CMSG_CLOEXEC) {
      int saved = *__oserror();
      rights_cloexec(msg);
      __irix_seterrno(saved);
    }
  }
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

#define TYPE_CLOEXEC 0x10000000
#define TYPE_NONBLOCK 0x20000000
#define TYPE_FLAGS (TYPE_CLOEXEC | TYPE_NONBLOCK)

extern int __irix_libc_socket_fn(int, int, int) __asm__("socket");
extern int __irix_libc_socketpair_fn(int, int, int, int *)
    __asm__("socketpair");
extern int __xpg4_accept(int, struct sockaddr *, size_t *);

// Sets the flags on fd; on failure closes it and returns -1 (errno kept).
static int type_flags(int fd, int flags) {
  if ((flags & TYPE_CLOEXEC) && fcntl(fd, F_SETFD, FD_CLOEXEC) < 0)
    goto fail;
  if (flags & TYPE_NONBLOCK) {
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0 || fcntl(fd, F_SETFL, fl | O_NONBLOCK) < 0)
      goto fail;
  }
  return fd;
fail: {
  int saved = *__oserror();
  close(fd);
  __irix_seterrno(saved);
  return -1;
}
}

int __irix_socket(int domain, int type, int protocol) {
  int s = __irix_libc_socket_fn(domain, type & ~TYPE_FLAGS, protocol);
  if (s < 0 || !(type & TYPE_FLAGS))
    return s;
  return type_flags(s, type & TYPE_FLAGS);
}

int __irix_socketpair(int domain, int type, int protocol, int sv[2]) {
  int r = __irix_libc_socketpair_fn(domain, type & ~TYPE_FLAGS, protocol, sv);
  if (r < 0 || !(type & TYPE_FLAGS))
    return r;
  if (type_flags(sv[0], type & TYPE_FLAGS) < 0) {
    int saved = *__oserror();
    close(sv[1]);
    __irix_seterrno(saved);
    return -1;
  }
  if (type_flags(sv[1], type & TYPE_FLAGS) < 0) {
    int saved = *__oserror();
    close(sv[0]);
    __irix_seterrno(saved);
    return -1;
  }
  return 0;
}

// socklen_t is 32 bits, like size_t, on n32.
int accept4(int s, struct sockaddr *__restrict addr,
            socklen_t *__restrict len, int flags) {
  if (flags & ~TYPE_FLAGS) {
    __irix_seterrno(EINVAL);
    return -1;
  }
  int fd = __xpg4_accept(s, addr, (size_t *)len);
  if (fd < 0 || !flags)
    return fd;
  return type_flags(fd, flags);
}

#endif // defined(__sgi)
