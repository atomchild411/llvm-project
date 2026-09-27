/*===---- ctype.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX 6.5.7's <ctype.h> has C99's isblank only as __isblank; 6.5.22's
 * declares isblank too. Declare isblank as libc's __isblank (asm label),
 * which every 6.5 release exports: right whichever headers are present --
 * a 6.5.22 root builds for the 6.5.7 floor as well -- where a definition of
 * our own would clash with 6.5.22's declaration.
 */

#ifndef __CLANG_IRIX_CTYPE_H
#define __CLANG_IRIX_CTYPE_H

#include_next <ctype.h>

#if !defined(isblank)
#ifdef __cplusplus
extern "C" int isblank(int) __asm__("__isblank");
#else
extern int isblank(int) __asm__("__isblank");
#endif
#endif

#endif /* __CLANG_IRIX_CTYPE_H */
