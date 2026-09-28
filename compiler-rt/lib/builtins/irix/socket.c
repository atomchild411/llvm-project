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

#endif // defined(__sgi)
