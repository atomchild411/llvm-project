/*===---- langinfo.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX 2008's nl_langinfo_l, which IRIX lacks: compiler-rt's IRIX builtins
 * have it (irix/locale_l.c). A locale_t is LC_GLOBAL_LOCALE (IRIX's
 * nl_langinfo) or the C locale (the only one newlocale makes).
 */

#ifndef __CLANG_IRIX_LANGINFO_H
#define __CLANG_IRIX_LANGINFO_H

#include_next <langinfo.h>
#include <__irix_locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif
char *nl_langinfo_l(nl_item, locale_t);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_LANGINFO_H */
