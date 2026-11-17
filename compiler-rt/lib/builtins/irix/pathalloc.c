//===-- irix/pathalloc.c - realpath and getcwd that allocate -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX's realpath() needs a buffer (POSIX 2008's allocates the result when
// resolved_path is NULL), and its getcwd(NULL, 0) fails (glibc and the BSDs
// allocate one as large as needed). Programs written for those stop with
// "realpath failed" or "getcwd failed". clang's IRIX <stdlib.h> and
// <unistd.h> send both calls here (by asm label); everything but the NULL
// cases goes to IRIX's own.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

extern char *__irix_libc_realpath(const char *, char *) __asm__("realpath");
extern char *__irix_libc_getcwd(char *, size_t) __asm__("getcwd");

static char *pathalloc_dup(const char *s) {
  char *r = strdup(s);
  if (r == NULL)
    errno = ENOMEM;
  return r;
}

char *__irix_realpath(const char *path, char *resolved) {
  char buf[PATH_MAX];
  if (resolved != NULL)
    return __irix_libc_realpath(path, resolved);
  if (__irix_libc_realpath(path, buf) == NULL)
    return NULL;
  return pathalloc_dup(buf);
}

char *__irix_getcwd(char *buf, size_t size) {
  char tmp[PATH_MAX];
  if (buf != NULL || size != 0)
    return __irix_libc_getcwd(buf, size);
  if (__irix_libc_getcwd(tmp, sizeof(tmp)) == NULL)
    return NULL;
  return pathalloc_dup(tmp);
}

#endif // defined(__sgi)
