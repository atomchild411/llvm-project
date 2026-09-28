//===-- irix/cloexec.c - O_CLOEXEC for IRIX -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// open() with O_CLOEXEC, which IRIX's lacks; clang's IRIX <fcntl.h> defines
// O_CLOEXEC to a bit IRIX does not use and routes open() here, as
// __irix_open. The file is opened without the bit, then marked close-on-exec
// with fcntl: two steps, as before O_CLOEXEC existed, so not atomic against a
// fork in another thread between them.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __IRIX_O_CLOEXEC
#error "clang's IRIX <fcntl.h> wrapper defines __IRIX_O_CLOEXEC"
#endif

// IRIX's own open, under its own name (the wrapper renames the one programs
// see).
extern int __irix_libc_open(const char *, int, ...) __asm__("open");

int __irix_open(const char *path, int flags, ...) {
  mode_t mode = 0;
  int fd;

  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
  fd = __irix_libc_open(path, flags & ~__IRIX_O_CLOEXEC, mode);
  if (fd >= 0 && (flags & __IRIX_O_CLOEXEC))
    fcntl(fd, F_SETFD, FD_CLOEXEC);
  return fd;
}

#endif // defined(__sgi)
