/*===---- stdlib.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <stdlib.h> does not say that abort and exit never return, so code
 * ending in either reads as falling off the end of a function. Say so.
 *
 * Before 6.5.22 it also lacks C99's strtof: see below.
 */

#ifndef __CLANG_IRIX_STDLIB_H
#define __CLANG_IRIX_STDLIB_H

#include_next <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
void abort(void) __attribute__((__noreturn__));
void exit(int) __attribute__((__noreturn__));
/* BSD's, which IRIX's libc lacks: compiler-rt's irix/progname.c. */
const char *getprogname(void);
void setprogname(const char *);
#ifdef __cplusplus
}
#endif

/* C99's strtof: IRIX 6.5.7 has none, 6.5.22 declares one. Declare it as
 * compiler-rt's __irix_strtof (irix/strtof.c; asm label), which is right
 * whichever headers are present, where a definition of our own would clash
 * with 6.5.22's declaration. */
#if !defined(strtof)
#ifdef __cplusplus
extern "C" float strtof(const char *__restrict, char **__restrict)
    __asm__("__irix_strtof");
#else
extern float strtof(const char *__restrict, char **__restrict)
    __asm__("__irix_strtof");
#endif
#endif

#endif /* __CLANG_IRIX_STDLIB_H */
