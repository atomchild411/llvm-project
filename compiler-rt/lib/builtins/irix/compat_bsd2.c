//===-- irix/compat_bsd2.c - more BSD and glibc calls IRIX lacks ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// strlcpy, strlcat and memrchr (<string.h>), declared by clang's IRIX
// wrappers; packages in the IRIX pkgsrc bulk build failed to link without
// them. Weak, so a program that brings its own copy keeps it.
//
// daemon, _Exit and vfork are in process_bsd.c, so that a library which
// only copies strings does not pick up a reference to _exit (libpq checks).
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stddef.h>
#include <string.h>

#pragma weak strlcpy
#pragma weak strlcat
#pragma weak memrchr

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

#endif // __sgi
