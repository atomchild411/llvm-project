//===-- irix/libc_compat.c - C95/C99 functions IRIX 6.5.7's libc lacks ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX before 6.5.22 has only C89's wide-character functions. These are the
// C95 and C99 ones C and C++ libraries use, declared by clang's IRIX
// <wchar.h> wrapper, POSIX 2008's strnlen, declared by its <string.h>, and
// C99's strtoimax, strtoumax and imaxabs, which IRIX's <inttypes.h> declares
// (in its own API mode only) but no IRIX library defines. They live here, in the builtins every IRIX link takes,
// so that a program built for 6.5.7 runs there; a libc that has them wins,
// since an archive member is only taken for a symbol nothing else defines.
// Calls reach them by name, too: clang lowers __builtin_wmemcmp and friends
// to calls when it cannot fold them.
//
// The restartable conversions sit on the stateless mbtowc and wctomb: no
// multibyte encoding IRIX has keeps shift state, so an mbstate_t is only
// ever in its initial state.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
  for (; n; ++s, --n)
    if (*s == c)
      return (wchar_t *)s;
  return 0;
}
int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) {
  for (; n; ++a, ++b, --n)
    if (*a != *b)
      return *a < *b ? -1 : 1;
  return 0;
}
wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n) {
  return (wchar_t *)memcpy(d, s, n * sizeof(wchar_t));
}
wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n) {
  return (wchar_t *)memmove(d, s, n * sizeof(wchar_t));
}
wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i)
    d[i] = c;
  return d;
}

wint_t btowc(int c) {
  char b = (char)c;
  wchar_t w;
  if (c == EOF || mbtowc(&w, &b, 1) != 1)
    return WEOF;
  return (wint_t)w;
}
int wctob(wint_t c) {
  char b[MB_LEN_MAX];
  if (c == WEOF || wctomb(b, (wchar_t)c) != 1)
    return EOF;
  return (unsigned char)b[0];
}

int iswblank(wint_t c) { return __iswblank(c); }

/* The restartable conversions. No encoding IRIX has keeps shift state, so an
 * mbstate_t is only ever in its initial state. */
int mbsinit(const mbstate_t *ps) { return 1; }
size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
  wchar_t w;
  int r;
  if (s == 0)
    return 0;
  if (n == 0)
    return (size_t)-2;
  r = mbtowc(&w, s, n);
  if (r < 0) {
    /* An incomplete character reads as an invalid one to mbtowc. */
    if (n < (size_t)MB_CUR_MAX)
      return (size_t)-2;
    return (size_t)-1;
  }
  if (pwc)
    *pwc = w;
  return (size_t)r;
}
size_t mbrlen(const char *s, size_t n, mbstate_t *ps) {
  return mbrtowc(0, s, n, ps);
}
size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps) {
  char b[MB_LEN_MAX];
  int r = wctomb(s ? s : b, s ? wc : L'\0');
  return r < 0 ? (size_t)-1 : (size_t)r;
}
size_t mbsrtowcs(wchar_t *d, const char **src, size_t len, mbstate_t *ps) {
  const char *s = *src;
  size_t n = 0;
  while (d == 0 || n < len) {
    wchar_t w;
    int r = mbtowc(&w, s, MB_CUR_MAX);
    if (r < 0) {
      if (d)
        *src = s;
      return (size_t)-1;
    }
    if (d)
      d[n] = w;
    if (r == 0) {
      if (d)
        *src = 0;
      return n;
    }
    s += r;
    ++n;
  }
  *src = s;
  return n;
}
size_t wcsrtombs(char *d, const wchar_t **src, size_t len, mbstate_t *ps) {
  const wchar_t *s = *src;
  size_t n = 0;
  for (;; ++s) {
    char b[MB_LEN_MAX];
    int r = wctomb(b, *s);
    if (r < 0) {
      if (d)
        *src = s;
      return (size_t)-1;
    }
    if (d) {
      if (n + (size_t)r > len) {
        *src = s;
        return n;
      }
      memcpy(d + n, b, (size_t)r);
    }
    if (*s == L'\0') {
      if (d)
        *src = 0;
      return n;
    }
    n += (size_t)r;
  }
}

float wcstof(const wchar_t *__restrict s, wchar_t **__restrict end) {
  return (float)wcstod(s, end);
}
long double wcstold(const wchar_t *__restrict s, wchar_t **__restrict end) {
  return wcstod(s, end);
}

/* The format and the output are converted to and from the multibyte
 * encoding around vsnprintf, so %ls and %lc arguments are not supported.
 * Returns the characters written, or -1 if they did not fit, as C99 says. */
int vswprintf(wchar_t *__restrict s, size_t n,
              const wchar_t *__restrict fmt,
                                __builtin_va_list ap) {
  size_t flen = wcstombs(0, fmt, 0);
  char *f, *buf;
  int len;
  size_t w;
  __builtin_va_list ap2;
  if (flen == (size_t)-1 || n == 0)
    return -1;
  f = (char *)malloc(flen + 1);
  if (f == 0)
    return -1;
  wcstombs(f, fmt, flen + 1);
  __builtin_va_copy(ap2, ap);
  len = vsnprintf(0, 0, f, ap2); /* C99 vsnprintf (see <stdio.h>) */
  __builtin_va_end(ap2);
  buf = len < 0 ? 0 : (char *)malloc((size_t)len + 1);
  if (buf == 0) {
    free(f);
    return -1;
  }
  vsnprintf(buf, (size_t)len + 1, f, ap);
  free(f);
  w = mbstowcs(s, buf, n);
  free(buf);
  if (w == (size_t)-1 || w >= n) {
    s[n - 1] = L'\0';
    return -1;
  }
  return (int)w;
}
int swprintf(wchar_t *__restrict s, size_t n,
             const wchar_t *__restrict fmt, ...) {
  __builtin_va_list ap;
  int r;
  __builtin_va_start(ap, fmt);
  r = vswprintf(s, n, fmt, ap);
  __builtin_va_end(ap);
  return r;
}

/* IRIX streams have no orientation: report none. */
int fwide(FILE *f, int mode) { return 0; }

size_t strnlen(const char *s, size_t n) {
  size_t i;
  for (i = 0; i < n && s[i]; ++i)
    ;
  return i;
}

// intmax_t is long long under n32 and long under n64: 64 bits either way,
// so strtoll and strtoull have the right range and errno behaviour.
intmax_t strtoimax(const char *__restrict s, char **__restrict end, int base) {
  return strtoll(s, end, base);
}

uintmax_t strtoumax(const char *__restrict s, char **__restrict end,
                    int base) {
  return strtoull(s, end, base);
}

intmax_t imaxabs(intmax_t j) { return j < 0 ? -j : j; }

#endif // defined(__sgi)
