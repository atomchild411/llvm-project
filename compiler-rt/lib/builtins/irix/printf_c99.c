//===-- irix/printf_c99.c - C99 printf formats on IRIX's pre-C99 libc -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX's printf family predates C99's length modifiers: it does not know
// z (size_t), t (ptrdiff_t), j (intmax_t) or hh (char). It does not stop at
// one either: it takes the letter as the conversion, so the argument list
// falls out of step with the format, and a later %s prints whatever the
// misread argument points at -- or faults. clang's IRIX <stdio.h> wrapper
// sends printf, fprintf, sprintf and their v- forms here (by asm label, so
// the name printf still means printf to the compiler), and its snprintf
// and vsnprintf use __irix_c99_fmt too. Each rewrites the format into one
// IRIX understands -- the same argument sizes, spelled the old way -- and
// calls IRIX's own function with the arguments untouched.
//
// An L before a floating conversion goes too: clang's long double is a
// double on IRIX and is passed as one, where IRIX's %Lf reads MIPSpro's
// pair of doubles.
//
// Its own file, apart from libc_compat.c, so that a program is only given
// these when it prints.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct __file_s FILE_; // IRIX's FILE, by pointer only

// IRIX's own functions, under names of ours: <stdio.h> sends the usual
// names back here.
extern int __irix_libc_vfprintf(FILE_ *, const char *, char *)
    __asm__("vfprintf");
extern int __irix_libc_vprintf(const char *, char *) __asm__("vprintf");
extern int __irix_libc_vsprintf(char *, const char *, char *)
    __asm__("vsprintf");

// The length modifiers C99 added, as IRIX spells the same sizes.
#define SIZE_MOD (sizeof(size_t) == 8 ? "l" : "")
#define PTRDIFF_MOD (sizeof(ptrdiff_t) == 8 ? "l" : "")
#define INTMAX_MOD "ll"

// Rewrite FMT for IRIX. Returns FMT itself when nothing needs changing;
// otherwise a copy in BUF (N bytes) or, when that is too small, in memory
// returned through *HEAP, which the caller frees.
const char *__irix_c99_fmt(const char *fmt, char *buf, size_t n,
                           char **heap) {
  const char *p;
  char *o;
  size_t len;

  *heap = 0;
  for (p = fmt; (p = strchr(p, '%')) != 0;) {
    ++p;
    if (*p == '%') {
      ++p;
      continue;
    }
    p += strspn(p, "0123456789.*$-+ #'");
    if (*p == 'z' || *p == 't' || *p == 'j' || (p[0] == 'h' && p[1] == 'h') ||
        (*p == 'L' && p[1] && strchr("eEfFgGaA", p[1])))
      break;
  }
  if (p == 0)
    return fmt; // no C99 modifier, no L: IRIX takes it as it is

  // Every rewrite is no longer than the original, except j -> ll.
  len = strlen(fmt);
  if (2 * len + 1 <= n) {
    o = buf;
  } else {
    o = *heap = (char *)malloc(2 * len + 1);
    if (o == 0)
      return fmt;
  }
  buf = o;
  for (p = fmt; *p;) {
    size_t k;
    if (*p != '%') {
      *o++ = *p++;
      continue;
    }
    *o++ = *p++;
    if (*p == '%') {
      *o++ = *p++;
      continue;
    }
    k = strspn(p, "0123456789.*$-+ #'");
    memcpy(o, p, k);
    o += k;
    p += k;
    if (*p == 'z' || *p == 't' || *p == 'j') {
      const char *m = *p == 'z' ? SIZE_MOD : *p == 't' ? PTRDIFF_MOD
                                                       : INTMAX_MOD;
      while (*m)
        *o++ = *m++;
      ++p;
    } else if (p[0] == 'h' && p[1] == 'h') {
      // A char is promoted to int; h prints it as the same value.
      *o++ = 'h';
      p += 2;
    } else if (*p == 'L' && p[1] && strchr("eEfFgGaA", p[1])) {
      // clang's long double is double on IRIX, passed as a double; IRIX's
      // %Lf would read a pair of doubles.
      ++p;
    }
  }
  *o = 0;
  return buf;
}

#define WITH_FMT(call)                                                         \
  char sbuf[256];                                                              \
  char *heap;                                                                  \
  int r;                                                                       \
  fmt = __irix_c99_fmt(fmt, sbuf, sizeof sbuf, &heap);                         \
  r = call;                                                                    \
  free(heap);                                                                  \
  return r

int __irix_c99_vfprintf(FILE_ *f, const char *fmt, va_list ap) {
  WITH_FMT(__irix_libc_vfprintf(f, fmt, ap));
}
int __irix_c99_vprintf(const char *fmt, va_list ap) {
  WITH_FMT(__irix_libc_vprintf(fmt, ap));
}
int __irix_c99_vsprintf(char *s, const char *fmt, va_list ap) {
  WITH_FMT(__irix_libc_vsprintf(s, fmt, ap));
}

int __irix_c99_fprintf(FILE_ *f, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vfprintf(f, fmt, ap);
  va_end(ap);
  return r;
}
int __irix_c99_printf(const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vprintf(fmt, ap);
  va_end(ap);
  return r;
}
int __irix_c99_sprintf(char *s, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vsprintf(s, fmt, ap);
  va_end(ap);
  return r;
}

#endif // __sgi
