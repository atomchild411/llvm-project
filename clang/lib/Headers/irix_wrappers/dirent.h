/*===---- dirent.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * dirfd (POSIX 2008), which IRIX lacks: compiler-rt's irix/posix2008.c
 * returns the DIR's descriptor.
 */

#ifndef __CLANG_IRIX_DIRENT_H
#define __CLANG_IRIX_DIRENT_H

#include_next <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif
int dirfd(DIR *);
DIR *fdopendir(int); /* irix/atfile.c */
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_DIRENT_H */
