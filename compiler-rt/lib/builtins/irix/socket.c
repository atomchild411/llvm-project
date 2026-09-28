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
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <sys/socket.h>
#include <sys/types.h>

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

#endif // defined(__sgi)
