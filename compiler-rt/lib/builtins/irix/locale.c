//===-- irix/locale.c - POSIX 2008 locale objects for IRIX ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// newlocale, uselocale, duplocale and freelocale, which no IRIX release has;
// clang's IRIX <locale.h> declares them with locale_t and the LC_*_MASK bits.
//
// Libraries use them to work in the C locale whatever the program's is:
// newlocale(LC_NUMERIC_MASK, "C", 0), uselocale around a printf or strtod,
// then back. That is all this supports. The only locale object is the C
// locale: newlocale accepts "C", "POSIX", and "" when the environment names
// no other locale, and fails with ENOENT for anything else, as for a locale
// that is not installed. uselocale records the choice and returns the
// previous one, but IRIX's functions keep using the global locale that
// setlocale sets; that is the C locale unless the program has asked for
// another, so formatting stays right for programs that never do. The choice
// is per process, not per thread.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

struct __irix_locale {
  int unused;
};

static struct __irix_locale c_locale;
static locale_t current = LC_GLOBAL_LOCALE;

static int is_c(const char *name) {
  return name && (!strcmp(name, "C") || !strcmp(name, "POSIX"));
}

// "" means the locale the environment names, as for setlocale.
static int environment_is_c(void) {
  static const char *const vars[] = {"LC_ALL", "LC_NUMERIC", "LANG"};
  unsigned i;
  for (i = 0; i < sizeof vars / sizeof vars[0]; i++) {
    const char *v = getenv(vars[i]);
    if (v && *v)
      return is_c(v);
  }
  return 1;
}

locale_t newlocale(int mask, const char *name, locale_t base) {
  (void)base;
  if ((mask & ~LC_ALL_MASK) || !name) {
    errno = EINVAL;
    return 0;
  }
  if (!is_c(name) && !(name[0] == 0 && environment_is_c())) {
    errno = ENOENT;
    return 0;
  }
  return &c_locale;
}

locale_t duplocale(locale_t loc) {
  if (!loc) {
    errno = EINVAL;
    return 0;
  }
  return &c_locale;
}

void freelocale(locale_t loc) { (void)loc; }

locale_t uselocale(locale_t loc) {
  locale_t previous = current;
  if (loc)
    current = loc;
  return previous;
}

#endif // defined(__sgi)
