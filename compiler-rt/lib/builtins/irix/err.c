//===-- irix/err.c - <err.h> for IRIX -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// err, errx, warn, warnx and their va_list forms, which IRIX lacks; clang's
// IRIX <err.h> declares them. As on the BSDs: "prog: message: strerror\n"
// on stderr (without ": strerror" for the x forms), the err ones then
// exiting with the given status.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char *getprogname(void);

static void message(int with_errno, int e, const char *fmt, va_list ap) {
  fprintf(stderr, "%s: ", getprogname());
  if (fmt) {
    vfprintf(stderr, fmt, ap);
    if (with_errno)
      fputs(": ", stderr);
  }
  if (with_errno)
    fputs(strerror(e), stderr);
  fputc('\n', stderr);
}

void vwarn(const char *fmt, va_list ap) { message(1, errno, fmt, ap); }
void vwarnx(const char *fmt, va_list ap) { message(0, 0, fmt, ap); }

void verr(int status, const char *fmt, va_list ap) {
  message(1, errno, fmt, ap);
  exit(status);
}

void verrx(int status, const char *fmt, va_list ap) {
  message(0, 0, fmt, ap);
  exit(status);
}

void warn(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwarn(fmt, ap);
  va_end(ap);
}

void warnx(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vwarnx(fmt, ap);
  va_end(ap);
}

void err(int status, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verr(status, fmt, ap);
}

void errx(int status, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verrx(status, fmt, ap);
}

#endif // defined(__sgi)
