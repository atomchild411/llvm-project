/*===---- search.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_SEARCH_H
#define __CLANG_IRIX_SEARCH_H

#include_next <search.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with POSIX's
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
/* X/Open modes declare them (as POSIX does) themselves, except 6.5.7 in
 * X/Open 7 mode. */
#if !_SGIAPI && (!defined(_XOPEN_SOURCE) ||                                 \
                 (!__has_include(<internal/wchar_core.h>) &&                 \
                  _XOPEN_SOURCE + 0 >= 700))
#ifdef __cplusplus
extern "C" {
#endif
void insque(void *, void *);
void remque(void *);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_SEARCH_H */
