//===-- irix/env.c - setenv and unsetenv for IRIX -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX 2001's setenv and unsetenv, which no IRIX libc has (it has putenv
// and environ); clang's IRIX <stdlib.h> declares them.
//
// setenv hands putenv a "name=value" string of its own, which the
// environment then holds: it is never freed, as in the classic BSD
// implementation, since the program may still hold pointers into it.
// unsetenv takes every entry for the name out of environ.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Weak: a program that brings its own copy (a compat/ directory) keeps it,
// while still getting the rest of this file.
#pragma weak setenv
#pragma weak unsetenv


extern char **environ;

static int valid_name(const char *name) {
  return name && *name && !strchr(name, '=');
}

int setenv(const char *name, const char *value, int overwrite) {
  size_t n, v;
  char *s;

  if (!valid_name(name)) {
    errno = EINVAL;
    return -1;
  }
  if (!overwrite && getenv(name))
    return 0;
  n = strlen(name);
  v = strlen(value);
  s = (char *)malloc(n + v + 2);
  if (!s) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(s, name, n);
  s[n] = '=';
  memcpy(s + n + 1, value, v + 1);
  return putenv(s) ? -1 : 0;
}

int unsetenv(const char *name) {
  size_t n;
  char **p, **q;

  if (!valid_name(name)) {
    errno = EINVAL;
    return -1;
  }
  n = strlen(name);
  for (p = q = environ; p && *p; ++p)
    if (!(strncmp(*p, name, n) == 0 && (*p)[n] == '='))
      *q++ = *p;
  if (q)
    *q = 0;
  return 0;
}

#endif // defined(__sgi)
