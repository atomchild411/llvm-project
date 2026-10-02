/*===---- string.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * No IRIX has strnlen (POSIX 2008) or strsignal. Declare them;
 * compiler-rt's builtins, which every IRIX link takes, define them
 * (irix/libc_compat.c, irix/strsignal.c). Likewise POSIX 2008's strndup,
 * stpcpy, stpncpy and strerror_r (the int one), and strsep, strcasestr and
 * explicit_bzero (irix/posix2008.c).
 */

#ifndef __CLANG_IRIX_STRING_H
#define __CLANG_IRIX_STRING_H

#include_next <string.h>

#ifdef __cplusplus
extern "C" {
#endif
size_t strnlen(const char *, size_t);
char *strsignal(int);
/* POSIX 2024's (and every other libc's), from irix/misc.c. */
void *memmem(const void *, size_t, const void *, size_t);
/* From irix/posix2008.c. */
char *strndup(const char *, size_t);
char *stpcpy(char *__restrict, const char *__restrict);
char *stpncpy(char *__restrict, const char *__restrict, size_t);
char *strsep(char **, const char *);
char *strcasestr(const char *, const char *);
void explicit_bzero(void *, size_t);
int strerror_r(int, char *, size_t);
#ifdef __cplusplus
}
#endif

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
char *strdup(const char *);
#ifdef __cplusplus
}
#endif
#endif

/* bzero, bcopy and the rest of <strings.h>: glibc's <string.h> includes it
 * unless asked for strict ISO C, and code relies on that; IRIX's does not. */
#if !defined(__STRICT_ANSI__) || defined(_GNU_SOURCE) || \
    defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE)
#include <strings.h>
#endif

#endif /* __CLANG_IRIX_STRING_H */
