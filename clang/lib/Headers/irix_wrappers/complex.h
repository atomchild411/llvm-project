/*===---- complex.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <complex.h> spells the imaginary unit as MIPSpro's builtin __I__,
 * which clang does not have. Spell I and _Complex_I as clang does; clang has
 * no imaginary types, so _Imaginary_I goes, as C11 says it must.
 */

#ifndef __CLANG_IRIX_COMPLEX_H
#define __CLANG_IRIX_COMPLEX_H

#include_next <complex.h>

#ifdef _Complex_I
#undef _Complex_I
#define _Complex_I (__extension__ 1.0iF)
#endif
#ifdef _Imaginary_I
#undef _Imaginary_I
#endif
#ifdef I
#undef I
#define I _Complex_I
#endif

#endif /* __CLANG_IRIX_COMPLEX_H */
