//===-- irix/compat_bsd2.c - more BSD and glibc calls IRIX lacks ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// strlcpy, strlcat and memrchr (<string.h>) and strtonum (<stdlib.h>),
// declared by clang's IRIX wrappers; packages in the IRIX pkgsrc bulk build
// failed to link without them. Weak, so a program that brings its own copy
// keeps it.
//
// daemon, _Exit and vfork are in process_bsd.c, so that a library which
// only copies strings does not pick up a reference to _exit (libpq checks).
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#pragma weak strlcpy
#pragma weak strlcat
#pragma weak memrchr
#pragma weak strtonum

size_t strlcpy(char *dst, const char *src, size_t size) {
  size_t len = strlen(src);
  if (size != 0) {
    size_t n = len < size - 1 ? len : size - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
  }
  return len;
}

size_t strlcat(char *dst, const char *src, size_t size) {
  size_t dlen = strnlen(dst, size);
  if (dlen == size)
    return size + strlen(src);
  return dlen + strlcpy(dst + dlen, src, size - dlen);
}

void *memrchr(const void *s, int c, size_t n) {
  const unsigned char *p = (const unsigned char *)s + n;
  while (n--)
    if (*--p == (unsigned char)c)
      return (void *)p;
  return NULL;
}

// OpenBSD's: a decimal number in [lo, hi], or 0 with *errstr saying why
// not ("invalid", "too small", "too large") and errno EINVAL or ERANGE.
long long strtonum(const char *s, long long lo, long long hi,
                   const char **errstr) {
  const long long llmax = 0x7fffffffffffffffLL, llmin = -llmax - 1;
  const char *why = NULL;
  long long v = 0;
  int saved = errno;
  if (lo > hi) {
    why = "invalid";
    errno = EINVAL;
  } else {
    char *end;
    errno = 0;
    v = strtoll(s, &end, 10);
    if (end == s || *end != '\0') {
      why = "invalid";
      errno = EINVAL;
    } else if ((v == llmin && errno == ERANGE) || v < lo) {
      why = "too small";
      errno = ERANGE;
    } else if ((v == llmax && errno == ERANGE) || v > hi) {
      why = "too large";
      errno = ERANGE;
    } else {
      errno = saved;
    }
    if (why)
      v = 0;
  }
  if (errstr)
    *errstr = why;
  return v;
}

#endif // __sgi
