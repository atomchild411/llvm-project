/*===---- sys/random.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * glibc's <sys/random.h>, which IRIX does not have: getrandom and
 * getentropy, from compiler-rt's irix/random.c (IRIX's /dev/urandom). Code
 * that finds getentropy by linking (ImageMagick) then includes this header
 * for its declaration.
 */

#ifndef __CLANG_IRIX_SYS_RANDOM_H
#define __CLANG_IRIX_SYS_RANDOM_H

#include <sys/types.h>

/* getrandom's flags, as glibc's. Every call reads IRIX's /dev/urandom,
 * or /dev/random for GRND_RANDOM; GRND_NONBLOCK and GRND_INSECURE are
 * ignored. */
#define GRND_NONBLOCK 0x01
#define GRND_RANDOM 0x02
#define GRND_INSECURE 0x04

#ifdef __cplusplus
extern "C" {
#endif
ssize_t getrandom(void *, size_t, unsigned int);
int getentropy(void *, size_t);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_SYS_RANDOM_H */
