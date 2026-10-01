/*===---- wchar.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <wchar.h> names its extra character classes _E1 to _E6, names C++
 * libraries use for template parameters (libc++'s expected<_T2, _E2>). For
 * C++, spell out the class masks built from them, and drop the names.
 *
 * Before 6.5.22 it also has only C89's wide-character functions. Declare
 * C95's and C99's that C and C++ libraries use (the wmem* family, btowc and
 * wctob, iswblank, the restartable conversions, wcstof, the wide printf
 * family, fwide), which compiler-rt's builtins define.
 */

#ifndef __CLANG_IRIX_WCHAR_H
#define __CLANG_IRIX_WCHAR_H

/* IRIX 6.5.22's <wchar.h> uses va_list (vsscanf, vfscanf, ...) without
 * including <stdarg.h>, and our _VA_LIST_ keeps it from declaring its own:
 * declare just va_list (POSIX allows it here), as clang's <stdarg.h> does. */
#define __need_va_list
#include <stdarg.h>

/* IRIX 6.5.22's <wchar.h> sets its guard, then includes <wctype.h> before it
 * declares anything, and <wctype.h> includes <wchar.h> again. In C++ that is
 * libc++'s, whose body then uses wcschr and friends before IRIX has declared
 * them. Declare them first: they live in internal/wchar_core.h (guarded;
 * 6.5.7 has no such file), after the headers IRIX's <wchar.h> itself
 * includes ahead of it, in the same order. */
/* IRIX's wcstok is XPG4's, wcstok(s, delim) (6.5.22 gives C95's only in
 * X/Open 5 mode, through an inline on _xpg5_wcstok): keep its declarations
 * under another name and declare C95's below. */
#define wcstok __irix_libc_wcstok

#if defined(__cplusplus) && __has_include(<internal/wchar_core.h>)
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <locale_attr.h>
#include <internal/wchar_core.h>
#endif

#include_next <wchar.h>
#undef wcstok

/* C95's wcstok is IRIX's wcstok_r, in every IRIX libc. */
#ifdef __cplusplus
extern "C"
#endif
wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict,
                wchar_t **__restrict) __asm__("wcstok_r");

#if defined(__cplusplus) && defined(_E1)
#undef _ISwprint
#undef _ISwgraph
#undef _ISwphonogram
#undef _ISwideogram
#undef _ISwenglish
#undef _ISwnumber
#undef _ISwspecial
#undef _ISwother
#define _ISwprint (_ISprint | 0x00003300)
#define _ISwgraph (_ISgraph | 0x00003300)
#define _ISwphonogram 0x00000100
#define _ISwideogram 0x00000200
#define _ISwenglish 0x00000400
#define _ISwnumber 0x00000800
#define _ISwspecial 0x00001000
#define _ISwother 0x00002000
#undef _E1
#undef _E2
#undef _E3
#undef _E4
#undef _E5
#undef _E6
#endif

#if __IRIX_VERSION__ < 60522
/* Defined in compiler-rt's builtins (irix/libc_compat.c), which every IRIX
 * link takes. */
#ifdef __cplusplus
extern "C" {
#endif
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);
int wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemset(wchar_t *, wchar_t, size_t);
wint_t btowc(int);
int wctob(wint_t);
int iswblank(wint_t);
int mbsinit(const mbstate_t *);
size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
size_t mbrlen(const char *, size_t, mbstate_t *);
size_t wcrtomb(char *, wchar_t, mbstate_t *);
size_t mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);
size_t wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
float wcstof(const wchar_t *__restrict, wchar_t **__restrict);
int vswprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict,
              __builtin_va_list);
int swprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...);
int fwide(FILE *, int);
int vfwprintf(FILE *__restrict, const wchar_t *__restrict, __builtin_va_list);
int fwprintf(FILE *__restrict, const wchar_t *__restrict, ...);
int vwprintf(const wchar_t *__restrict, __builtin_va_list);
int wprintf(const wchar_t *__restrict, ...);
#ifdef __cplusplus
}
#endif
#endif /* __IRIX_VERSION__ < 60522 */

/* POSIX 2008's, which no IRIX has: compiler-rt's (irix/libc_compat.c). */
#ifdef __cplusplus
extern "C" {
#endif
size_t wcsnlen(const wchar_t *, size_t);
wchar_t *wcsdup(const wchar_t *);
int wcscasecmp(const wchar_t *, const wchar_t *);
#ifdef __cplusplus
}
#endif

/* clang's long double is a double on IRIX, so wcstold is wcstod (6.5.22's
 * own returns MIPSpro's long double, a pair of doubles). */
#ifdef __cplusplus
extern "C"
#endif
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict)
    __asm__("wcstod");

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
long long wcstoll(const wchar_t *, wchar_t **, int);
unsigned long long wcstoull(const wchar_t *, wchar_t **, int);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_WCHAR_H */
