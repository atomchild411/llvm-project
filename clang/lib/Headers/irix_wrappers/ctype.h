/*===---- ctype.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX 6.5.7's <ctype.h> has C99's isblank only as __isblank. Add isblank on
 * it for releases before 6.5.22 (see __IRIX_VERSION__).
 */

#ifndef __CLANG_IRIX_CTYPE_H
#define __CLANG_IRIX_CTYPE_H

#include_next <ctype.h>

#if __IRIX_VERSION__ < 60522 && !defined(isblank)
static __inline__ int isblank(int __c) { return __isblank(__c); }
#endif

#endif /* __CLANG_IRIX_CTYPE_H */
