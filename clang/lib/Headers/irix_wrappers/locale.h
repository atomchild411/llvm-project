/*===---- locale.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX 2008's locale objects (locale_t, newlocale, uselocale, duplocale,
 * freelocale and the LC_*_MASK bits), which no IRIX release has; modern
 * libraries (fontconfig, libxml2, GLib) use them to format and parse
 * numbers in the C locale whatever the program's locale is. compiler-rt's
 * IRIX builtins define them (irix/locale.c) with the C locale as the only
 * object: see there for what that means.
 */

#ifndef __CLANG_IRIX_LOCALE_H
#define __CLANG_IRIX_LOCALE_H

#include_next <locale.h>

#ifndef LC_CTYPE_MASK
#define LC_CTYPE_MASK (1 << LC_CTYPE)
#define LC_NUMERIC_MASK (1 << LC_NUMERIC)
#define LC_TIME_MASK (1 << LC_TIME)
#define LC_COLLATE_MASK (1 << LC_COLLATE)
#define LC_MONETARY_MASK (1 << LC_MONETARY)
#define LC_MESSAGES_MASK (1 << LC_MESSAGES)
#define LC_ALL_MASK                                                            \
  (LC_CTYPE_MASK | LC_NUMERIC_MASK | LC_TIME_MASK | LC_COLLATE_MASK |          \
   LC_MONETARY_MASK | LC_MESSAGES_MASK)

typedef struct __irix_locale *locale_t;
#define LC_GLOBAL_LOCALE ((locale_t)-1)

#ifdef __cplusplus
extern "C" {
#endif
locale_t newlocale(int, const char *, locale_t);
locale_t duplocale(locale_t);
void freelocale(locale_t);
locale_t uselocale(locale_t);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_LOCALE_H */
