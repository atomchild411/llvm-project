/*===---- sys/file.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/file.h> hides flock() from C++ unless _BSD_COMPAT is defined,
 * fearing a clash with struct flock; C++ allows a function and a struct of
 * the same name (glibc declares both). flock is in libc: declare it.
 */

#ifndef __CLANG_IRIX_SYS_FILE_H
#define __CLANG_IRIX_SYS_FILE_H

#include_next <sys/file.h>

#if defined(__cplusplus) && !defined(_BSD_COMPAT)
extern "C" int flock(int, int);
#endif

#endif /* __CLANG_IRIX_SYS_FILE_H */
