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
 * Before 6.5.22 it also lacks C99's strtof: add it on strtod.
 */

#ifndef __CLANG_IRIX_STDLIB_H
#define __CLANG_IRIX_STDLIB_H

#include_next <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
void abort(void) __attribute__((__noreturn__));
void exit(int) __attribute__((__noreturn__));
#ifdef __cplusplus
}
#endif

#if __IRIX_VERSION__ < 60522
#include <errno.h>
static __inline__ float strtof(const char *__restrict __nptr,
                               char **__restrict __endptr) {
  double __d = strtod(__nptr, __endptr);
  float __f = (float)__d;
  /* Out of float's range, though not double's. */
  if (__builtin_isinf(__f) && !__builtin_isinf(__d))
    errno = ERANGE;
  else if (__f == 0 && __d != 0)
    errno = ERANGE;
  return __f;
}
#endif

#endif /* __CLANG_IRIX_STDLIB_H */
