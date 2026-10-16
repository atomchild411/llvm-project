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
// that does not gets these, with the same POSIX behaviour. And the BSDs' (and
// glibc's) wait4, declared by clang's IRIX <sys/wait.h>.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Weak: a program that brings its own copy (a compat/ directory) keeps it,
// while still getting the rest of this file.
#pragma weak memmem
#pragma weak mkdtemp
#pragma weak basename
#pragma weak dirname
#pragma weak wait4


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
    __irix_seterrno(EINVAL);
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
  __irix_seterrno(EEXIST);
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

// wait4: IRIX has wait3, which waits for any child, and waitpid, which
// reports no usage. For one child, its usage is what the children's total
// (getrusage) grew by while waiting: exact unless another thread reaps a child
// meanwhile.
pid_t wait4(pid_t pid, int *status, int options, struct rusage *ru) {
  struct rusage before, after;
  pid_t r;

  if (!ru)
    return waitpid(pid, status, options);
  if (pid == -1)
    return wait3(status, options, ru);
  getrusage(RUSAGE_CHILDREN, &before);
  r = waitpid(pid, status, options);
  if (r <= 0)
    return r;
  getrusage(RUSAGE_CHILDREN, &after);
  memset(ru, 0, sizeof(*ru));
#define __IRIX_TV_SUB(f)                                                       \
  do {                                                                         \
    ru->f.tv_sec = after.f.tv_sec - before.f.tv_sec;                           \
    ru->f.tv_usec = after.f.tv_usec - before.f.tv_usec;                        \
    if (ru->f.tv_usec < 0) {                                                   \
      ru->f.tv_sec--;                                                          \
      ru->f.tv_usec += 1000000;                                                \
    }                                                                          \
  } while (0)
  __IRIX_TV_SUB(ru_utime);
  __IRIX_TV_SUB(ru_stime);
#undef __IRIX_TV_SUB
  ru->ru_maxrss = after.ru_maxrss;
  ru->ru_minflt = after.ru_minflt - before.ru_minflt;
  ru->ru_majflt = after.ru_majflt - before.ru_majflt;
  ru->ru_inblock = after.ru_inblock - before.ru_inblock;
  ru->ru_oublock = after.ru_oublock - before.ru_oublock;
  ru->ru_nvcsw = after.ru_nvcsw - before.ru_nvcsw;
  ru->ru_nivcsw = after.ru_nivcsw - before.ru_nivcsw;
  return r;
}

#endif // defined(__sgi)
