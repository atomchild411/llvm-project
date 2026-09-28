/*===---- err.h - IRIX wrapper ----------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * The BSDs' (and glibc's) <err.h>, which IRIX lacks: formatted messages on
 * stderr, prefixed with the program's name, with or without strerror(errno),
 * optionally exiting. compiler-rt's IRIX builtins implement them
 * (irix/err.c).
 */

#ifndef __CLANG_IRIX_ERR_H
#define __CLANG_IRIX_ERR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void err(int, const char *, ...)
    __attribute__((__noreturn__, __format__(__printf__, 2, 3)));
void verr(int, const char *, va_list)
    __attribute__((__noreturn__, __format__(__printf__, 2, 0)));
void errx(int, const char *, ...)
    __attribute__((__noreturn__, __format__(__printf__, 2, 3)));
void verrx(int, const char *, va_list)
    __attribute__((__noreturn__, __format__(__printf__, 2, 0)));
void warn(const char *, ...) __attribute__((__format__(__printf__, 1, 2)));
void vwarn(const char *, va_list) __attribute__((__format__(__printf__, 1, 0)));
void warnx(const char *, ...) __attribute__((__format__(__printf__, 1, 2)));
void vwarnx(const char *, va_list)
    __attribute__((__format__(__printf__, 1, 0)));

#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_ERR_H */
