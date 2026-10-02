//===-- irix/fcntl.c - F_DUPFD_CLOEXEC for IRIX ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// fcntl() with F_DUPFD_CLOEXEC, which IRIX's lacks; clang's IRIX <fcntl.h>
// defines F_DUPFD_CLOEXEC to a command IRIX does not use and routes fcntl()
// here, as __irix_fcntl. The descriptor is duplicated with F_DUPFD, then
// marked close-on-exec: two steps, as before F_DUPFD_CLOEXEC existed, so not
// atomic against a fork in another thread between them. Every other command
// goes to IRIX's fcntl() unchanged.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <fcntl.h>
#include <stdarg.h>

#ifndef __IRIX_F_DUPFD_CLOEXEC
#error "clang's IRIX <fcntl.h> wrapper defines __IRIX_F_DUPFD_CLOEXEC"
#endif

// IRIX's own fcntl, under its own name (the wrapper renames the one
// programs see).
extern int __irix_libc_fcntl(int, int, ...) __asm__("fcntl");

int __irix_fcntl(int fd, int cmd, ...) {
  // IRIX's commands take an int, a pointer or nothing: all one argument
  // slot, the size of a long, in every IRIX ABI.
  va_list ap;
  va_start(ap, cmd);
  long arg = va_arg(ap, long);
  va_end(ap);
  if (cmd == __IRIX_F_DUPFD_CLOEXEC) {
    int nfd = __irix_libc_fcntl(fd, F_DUPFD, (int)arg);
    if (nfd >= 0)
      __irix_libc_fcntl(nfd, F_SETFD, FD_CLOEXEC);
    return nfd;
  }
  return __irix_libc_fcntl(fd, cmd, arg);
}

#endif // defined(__sgi)
