/*===---- __irix_locale_t.h - locale_t for the IRIX wrappers ----------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX 2008's locale_t, which no IRIX release has: <locale.h> defines it,
 * and <ctype.h>, <string.h> and <wchar.h> need it, without the rest of
 * <locale.h>, for the *_l functions. compiler-rt's IRIX builtins implement
 * the objects (locale.c) and the functions (locale_l.c).
 */

#ifndef __CLANG_IRIX_LOCALE_T_H
#define __CLANG_IRIX_LOCALE_T_H
typedef struct __irix_locale *locale_t;
#endif /* __CLANG_IRIX_LOCALE_T_H */
