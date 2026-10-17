/*===---- fnmatch.h - IRIX wrapper ------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's fnmatch has POSIX's three flags. The BSDs' and glibc's
 * FNM_CASEFOLD (and its Solaris and POSIX 2024 name FNM_IGNORECASE),
 * FNM_LEADING_DIR and glibc's FNM_FILE_NAME are defined here, with the
 * BSDs' values, and fnmatch is bound to compiler-rt's irix/fnmatch.c, which
 * handles a call with either of the first two itself and passes the rest to
 * IRIX's. FNM_NOMATCH stays IRIX's -2.
 */

#ifndef __CLANG_IRIX_FNMATCH_H
#define __CLANG_IRIX_FNMATCH_H

#define fnmatch __irix_libc_fnmatch
#include_next <fnmatch.h>
#undef fnmatch

#ifndef FNM_LEADING_DIR
#define FNM_LEADING_DIR 0x08
#endif
#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD 0x10
#endif
#ifndef FNM_IGNORECASE
#define FNM_IGNORECASE FNM_CASEFOLD
#endif
#ifndef FNM_FILE_NAME
#define FNM_FILE_NAME FNM_PATHNAME
#endif

#ifdef __cplusplus
extern "C" {
#endif
int fnmatch(const char *, const char *, int) __asm__("__irix_fnmatch");
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_FNMATCH_H */
