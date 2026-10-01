//===-- irix/scanf_c99.c - C99 scanf formats and v*scanf on IRIX ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX's scanf family predates C99, as its printf does (printf_c99.c): it
// does not know the length modifiers z, t and j, and its L reads a long
// double as MIPSpro has it, a pair of doubles -- 16 bytes stored where
// clang's long double, a double on IRIX, has 8. Nor does IRIX's libc have
// C99's vscanf, vfscanf and vsscanf at all.
//
// clang's IRIX <stdio.h> sends scanf, fscanf, sscanf and the v- forms here.
// Each rewrites the format (z and t to nothing, j to ll, L to l before a
// floating conversion) and calls IRIX's scanf, fscanf or sscanf. Every
// argument of a scanf conversion is a pointer, so the v- forms take as many
// pointers from the va_list as the format has assigning conversions and
// pass them on (up to MAXARGS).
//
// Not handled: hh (IRIX would store a short where the caller has a char);
// it is passed through unchanged.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct __file_s FILE_; // IRIX's FILE, by pointer only

extern int __irix_libc_scanf(const char *, ...) __asm__("scanf");
extern int __irix_libc_fscanf(FILE_ *, const char *, ...) __asm__("fscanf");
extern int __irix_libc_sscanf(const char *, const char *, ...)
    __asm__("sscanf");

#define SIZE_MOD (sizeof(size_t) == 8 ? "l" : "")
#define PTRDIFF_MOD (sizeof(ptrdiff_t) == 8 ? "l" : "")
#define INTMAX_MOD "ll"
#define MAXARGS 64

// Rewrite FMT into OUT (at least 2 * strlen(FMT) + 1 bytes) and count the
// conversions that assign, i.e. take a pointer.
static int rewrite(const char *p, char *o) {
  int n = 0;
  while (*p) {
    int assign = 1;
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
    if (*p == '*') {
      assign = 0;
      *o++ = *p++;
    }
    k = strspn(p, "0123456789");
    memcpy(o, p, k);
    o += k;
    p += k;
    if (*p == 'z' || *p == 't' || *p == 'j') {
      const char *m = *p == 'z' ? SIZE_MOD : *p == 't' ? PTRDIFF_MOD
                                                       : INTMAX_MOD;
      while (*m)
        *o++ = *m++;
      ++p;
    } else if (*p == 'L' && p[1] && strchr("eEfFgGaA", p[1])) {
      *o++ = 'l';
      ++p;
    }
    while (*p == 'h' || *p == 'l' || *p == 'L' || *p == 'q')
      *o++ = *p++;
    if (*p == '[') {
      // A scan set: ']' right after '[' or '[^' belongs to the set.
      *o++ = *p++;
      if (*p == '^')
        *o++ = *p++;
      if (*p == ']')
        *o++ = *p++;
      while (*p && *p != ']')
        *o++ = *p++;
      if (*p)
        *o++ = *p++;
    } else if (*p) {
      *o++ = *p++;
    }
    n += assign;
  }
  *o = 0;
  return n;
}

#define WITH_FMT(fmt, ap, call)                                                \
  char sbuf[512];                                                              \
  char *nf = sbuf, *heap = 0;                                                  \
  void *a[MAXARGS];                                                            \
  int i, n, r;                                                                 \
  size_t len = strlen(fmt);                                                    \
  if (2 * len + 1 > sizeof sbuf) {                                             \
    nf = heap = (char *)malloc(2 * len + 1);                                   \
    if (nf == 0)                                                               \
      return -1;                                                               \
  }                                                                            \
  n = rewrite(fmt, nf);                                                        \
  if (n > MAXARGS) {                                                           \
    free(heap);                                                                \
    return -1;                                                                 \
  }                                                                            \
  for (i = 0; i < n; i++)                                                      \
    a[i] = va_arg(ap, void *);                                                 \
  for (; i < MAXARGS; i++)                                                     \
    a[i] = 0;                                                                  \
  r = call;                                                                    \
  free(heap);                                                                  \
  return r

#define ARGS                                                                   \
  a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11],    \
      a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19], a[20], a[21],    \
      a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29], a[30], a[31],    \
      a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39], a[40], a[41],    \
      a[42], a[43], a[44], a[45], a[46], a[47], a[48], a[49], a[50], a[51],    \
      a[52], a[53], a[54], a[55], a[56], a[57], a[58], a[59], a[60], a[61],    \
      a[62], a[63]

int __irix_c99_vsscanf(const char *s, const char *fmt, va_list ap) {
  WITH_FMT(fmt, ap, __irix_libc_sscanf(s, nf, ARGS));
}
int __irix_c99_vfscanf(FILE_ *f, const char *fmt, va_list ap) {
  WITH_FMT(fmt, ap, __irix_libc_fscanf(f, nf, ARGS));
}
int __irix_c99_vscanf(const char *fmt, va_list ap) {
  WITH_FMT(fmt, ap, __irix_libc_scanf(nf, ARGS));
}

int __irix_c99_sscanf(const char *s, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vsscanf(s, fmt, ap);
  va_end(ap);
  return r;
}
int __irix_c99_fscanf(FILE_ *f, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vfscanf(f, fmt, ap);
  va_end(ap);
  return r;
}
int __irix_c99_scanf(const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = __irix_c99_vscanf(fmt, ap);
  va_end(ap);
  return r;
}

#endif // __sgi
