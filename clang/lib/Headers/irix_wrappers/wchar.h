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
 * wctob, iswblank, the restartable conversions, wcstof and wcstold, swprintf
 * and vswprintf, fwide), which compiler-rt's builtins define.
 */

#ifndef __CLANG_IRIX_WCHAR_H
#define __CLANG_IRIX_WCHAR_H

#include_next <wchar.h>

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
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict);
int vswprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict,
              __builtin_va_list);
int swprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...);
int fwide(FILE *, int);
#ifdef __cplusplus
}
#endif
#endif /* __IRIX_VERSION__ < 60522 */

#endif /* __CLANG_IRIX_WCHAR_H */
