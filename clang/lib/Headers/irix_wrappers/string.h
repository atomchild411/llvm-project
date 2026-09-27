/*===---- string.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX before 6.5.22 has no strnlen (POSIX 2008). Declare it; compiler-rt's
 * builtins, which every IRIX link takes, define it (irix/libc_compat.c).
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

#endif /* __CLANG_IRIX_STRING_H */
