//===-- irix/wcsftime.c - C95's wcsftime for IRIX -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// C95's wcsftime, with a wide format, as __irix_wcsftime: clang's IRIX
// <wchar.h> names every call to wcsftime so. IRIX's own wcsftime is
// XPG4's, with a char * format (Python's time.strftime passed it a wide
// one and got ""); 6.5.22 adds C95's as _xpg5_wcsftime, which earlier
// releases lack. The format goes to multibyte, through strftime, and the
// result back to wide characters.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

size_t __irix_wcsftime(wchar_t *s, size_t max, const wchar_t *fmt,
                       const struct tm *tm) {
  size_t flen, fcap, ocap, n, w;
  char *mfmt, *out;

  if (max == 0)
    return 0;
  flen = wcslen(fmt);
  fcap = flen * MB_LEN_MAX + 1;
  ocap = max * MB_LEN_MAX + 1;
  if ((mfmt = (char *)malloc(fcap)) == 0)
    return 0;
  if (wcstombs(mfmt, fmt, fcap) == (size_t)-1 || (out = (char *)malloc(ocap)) == 0) {
    free(mfmt);
    return 0;
  }
  n = strftime(out, ocap, mfmt, tm);
  free(mfmt);
  // 0 is also an empty result (an empty format): no wide characters.
  if (n == 0) {
    free(out);
    s[0] = 0;
    return 0;
  }
  w = mbstowcs(s, out, max);
  free(out);
  // All of it and the terminating null must fit, as in strftime.
  if (w == (size_t)-1 || w >= max)
    return 0;
  return w;
}

#endif // defined(__sgi)
