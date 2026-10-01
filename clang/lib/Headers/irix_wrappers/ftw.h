/*===---- ftw.h - IRIX wrapper ----------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_FTW_H
#define __CLANG_IRIX_FTW_H

#include_next <ftw.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
struct stat;
struct FTW;
#ifdef __cplusplus
extern "C" {
#endif
int nftw(const char *, int (*)(const char *, const struct stat *, int, struct FTW *),
         int, int);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_FTW_H */
