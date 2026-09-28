//===-- irix/misc.c - memmem, mkdtemp, basename, dirname for IRIX ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Functions modern code expects in libc that IRIX's lacks: memmem and
// mkdtemp (POSIX 2008 and 2024), declared by clang's IRIX <string.h> and
// <stdlib.h>; and basename and dirname, which IRIX keeps in libgen (its
// <libgen.h> declares them): a program that links libgen gets IRIX's, one
// that does not gets these, with the same POSIX behaviour.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void *memmem(const void *haystack, size_t hlen, const void *needle,
             size_t nlen) {
  const unsigned char *h = (const unsigned char *)haystack;
  const unsigned char *n = (const unsigned char *)needle;
  size_t i;

  if (nlen == 0)
    return (void *)h;
  if (hlen < nlen)
    return 0;
  for (i = 0; i <= hlen - nlen; i++)
    if (h[i] == n[0] && memcmp(h + i, n, nlen) == 0)
      return (void *)(h + i);
  return 0;
}

char *mkdtemp(char *tmpl) {
  static const char chars[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static unsigned long counter;
  size_t len = tmpl ? strlen(tmpl) : 0;
  char *x;
  int tries;

  if (len < 6 || strcmp(tmpl + len - 6, "XXXXXX") != 0) {
    errno = EINVAL;
    return 0;
  }
  x = tmpl + len - 6;
  for (tries = 0; tries < 1000; tries++) {
    unsigned long v = (unsigned long)getpid() * 2654435761UL ^
                      (unsigned long)time(0) ^ (++counter * 40503UL);
    int i;
    for (i = 0; i < 6; i++, v /= 62)
      x[i] = chars[v % 62];
    if (mkdir(tmpl, 0700) == 0)
      return tmpl;
    if (errno != EEXIST)
      return 0;
  }
  errno = EEXIST;
  return 0;
}

char *basename(char *path) {
  static char dot[] = ".";
  char *end, *start;

  if (!path || !*path)
    return dot;
  end = path + strlen(path) - 1;
  while (end > path && *end == '/')
    *end-- = 0;
  if (end == path && *end == '/')
    return path;
  start = strrchr(path, '/');
  return start ? start + 1 : path;
}

char *dirname(char *path) {
  static char dot[] = ".";
  char *end;

  if (!path || !*path)
    return dot;
  end = path + strlen(path) - 1;
  while (end > path && *end == '/')
    end--;
  while (end > path && *end != '/')
    end--;
  if (end == path && *end != '/')
    return dot;
  while (end > path && *end == '/')
    end--;
  end[1] = 0;
  return path;
}

#endif // defined(__sgi)
