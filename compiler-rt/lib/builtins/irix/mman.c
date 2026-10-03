//===-- irix/mman.c - MAP_ANON for IRIX -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// mmap and mmap64 with MAP_ANON, which IRIX lacks; clang's IRIX
// <sys/mman.h> defines the flag and routes calls here (as __irix_mmap and
// __irix_mmap64). An anonymous mapping becomes a mapping of /dev/zero,
// IRIX's (SVR4's) anonymous memory: zero-filled, private or shared across
// fork as asked. Private ones get MAP_AUTORESRV, so swap is reserved when
// pages are first written rather than for the whole range at once. The
// descriptor argument is ignored, as POSIX allows for MAP_ANON.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// IRIX's own, under their own names (the wrapper renames the declarations
// a program sees).
extern void *__irix_libc_mmap(void *, size_t, int, int, int, off_t)
    __asm__("mmap");
extern void *__irix_libc_mmap64(void *, size_t, int, int, int, long long)
    __asm__("mmap64");
extern int __irix_libc_open(const char *, int, ...) __asm__("open");

#ifndef __IRIX_MAP_ANON
#error "clang's IRIX <sys/mman.h> wrapper defines __IRIX_MAP_ANON"
#endif

static int anon_flags(int flags) {
  flags &= ~__IRIX_MAP_ANON;
  if ((flags & MAP_TYPE) == MAP_PRIVATE)
    flags |= MAP_AUTORESRV;
  return flags;
}

void *__irix_mmap(void *addr, size_t len, int prot, int flags, int fd,
                  off_t off) {
  void *p;
  int zfd, e;
  if (!(flags & __IRIX_MAP_ANON))
    return __irix_libc_mmap(addr, len, prot, flags, fd, off);
  zfd = __irix_libc_open("/dev/zero", O_RDWR);
  if (zfd < 0)
    return MAP_FAILED;
  p = __irix_libc_mmap(addr, len, prot, anon_flags(flags), zfd, 0);
  e = errno;
  close(zfd);
  __irix_seterrno(e);
  return p;
}

void *__irix_mmap64(void *addr, size_t len, int prot, int flags, int fd,
                    long long off) {
  if (!(flags & __IRIX_MAP_ANON))
    return __irix_libc_mmap64(addr, len, prot, flags, fd, off);
  return __irix_mmap(addr, len, prot, flags, -1, 0);
}

#endif // defined(__sgi)
