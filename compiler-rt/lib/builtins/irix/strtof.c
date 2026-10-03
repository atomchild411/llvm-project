//===-- irix/strtof.c - C99 strtof for IRIX -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// C99's strtof, on strtod. IRIX 6.5.7 has none; 6.5.22 has one. clang's IRIX
// <stdlib.h> declares strtof as this function either way, so programs built
// for the 6.5.7 floor behave the same on every release.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"

extern double __irix_libc_strtod(const char *, char **) __asm__("strtod");

float __irix_strtof(const char *nptr, char **endptr) {
  double d = __irix_libc_strtod(nptr, endptr);
  float f = (float)d;
  // Out of float's range, though not double's.
  if (__builtin_isinf(f) && !__builtin_isinf(d))
    __irix_seterrno(ERANGE);
  else if (f == 0 && d != 0)
    __irix_seterrno(ERANGE);
  return f;
}

#endif // __sgi
