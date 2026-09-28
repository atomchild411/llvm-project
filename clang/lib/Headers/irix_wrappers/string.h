/*===---- string.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX before 6.5.22 has no strnlen (POSIX 2008), and no IRIX has
 * strsignal. Declare them; compiler-rt's builtins, which every IRIX link
 * takes, define them (irix/libc_compat.c, irix/strsignal.c).
 */

#ifndef __CLANG_IRIX_STRING_H
#define __CLANG_IRIX_STRING_H

#include_next <string.h>

#if __IRIX_VERSION__ < 60522
#ifdef __cplusplus
extern "C" {
#endif
size_t strnlen(const char *, size_t);
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
char *strsignal(int);
/* POSIX 2024's (and every other libc's), from irix/misc.c. */
void *memmem(const void *, size_t, const void *, size_t);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_STRING_H */
