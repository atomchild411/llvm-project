//===-- irix/wscanf.c - C95's wide scanf for IRIX -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// swscanf, fwscanf and wscanf, and C99's v forms, which IRIX's libc lacks
// (wxWidgets calls them unconditionally); clang's IRIX <wchar.h> declares
// them. The string and the format go to multibyte, and from there to the
// C99 scanf (irix/scanf_c99.c): a narrow scanf's %ls, %lc and %l[ store
// wide characters converted from the multibyte input, as the wide ones do
// from theirs, and its %s, %c and %[ store multibyte, as theirs do. Only %n
// differs: it counts bytes, not wide characters. The streams IRIX has are
// byte streams, which the narrow scanf reads the same way.
//
// Weak, as in irix/libc_compat.c: a libc that has them wins.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#pragma weak vswscanf
#pragma weak swscanf
#pragma weak vfwscanf
#pragma weak fwscanf
#pragma weak vwscanf
#pragma weak wscanf

// A malloc'd multibyte copy of ws, or NULL (errno EILSEQ or ENOMEM).
static char *narrow(const wchar_t *ws) {
  size_t n = wcstombs(NULL, ws, 0);
  char *s;
  if (n == (size_t)-1) {
    errno = EILSEQ;
    return NULL;
  }
  if ((s = malloc(n + 1)) != NULL)
    wcstombs(s, ws, n + 1);
  return s;
}

int vswscanf(const wchar_t *__restrict ws, const wchar_t *__restrict wfmt,
             va_list ap) {
  char *s = narrow(ws), *fmt = s != NULL ? narrow(wfmt) : NULL;
  int r = EOF;
  if (fmt != NULL)
    r = vsscanf(s, fmt, ap);
  free(s);
  free(fmt);
  return r;
}

int swscanf(const wchar_t *__restrict ws, const wchar_t *__restrict wfmt,
            ...) {
  va_list ap;
  int r;
  va_start(ap, wfmt);
  r = vswscanf(ws, wfmt, ap);
  va_end(ap);
  return r;
}

int vfwscanf(FILE *__restrict f, const wchar_t *__restrict wfmt, va_list ap) {
  char *fmt = narrow(wfmt);
  int r = EOF;
  if (fmt != NULL)
    r = vfscanf(f, fmt, ap);
  free(fmt);
  return r;
}

int fwscanf(FILE *__restrict f, const wchar_t *__restrict wfmt, ...) {
  va_list ap;
  int r;
  va_start(ap, wfmt);
  r = vfwscanf(f, wfmt, ap);
  va_end(ap);
  return r;
}

int vwscanf(const wchar_t *__restrict wfmt, va_list ap) {
  return vfwscanf(stdin, wfmt, ap);
}

int wscanf(const wchar_t *__restrict wfmt, ...) {
  va_list ap;
  int r;
  va_start(ap, wfmt);
  r = vfwscanf(stdin, wfmt, ap);
  va_end(ap);
  return r;
}

#endif // defined(__sgi)
