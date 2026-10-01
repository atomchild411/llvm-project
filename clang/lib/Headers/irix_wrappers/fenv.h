/*===---- fenv.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <fenv.h> declares libm's functions without extern "C", so C++
 * callers would look for mangled names libm does not have. Give them C
 * linkage.
 */

#ifndef __CLANG_IRIX_FENV_H
#define __CLANG_IRIX_FENV_H

#if __has_include_next(<fenv.h>) && defined(__c99)
/* IRIX's C99 <fenv.h> comes with MIPSpro 7.3/7.4's headers, and stops with
 * #error unless the compilation is C99 (__c99); gnu89 code (libsoxr) takes
 * the branch below. */
#ifdef __cplusplus
extern "C" {
#endif
#include_next <fenv.h>
#ifdef __cplusplus
}
#endif

#else
/* A root without MIPSpro 7.3+'s headers (a stock 6.5.22 has none, and its
 * libc and libm lack the C99 functions), or a compilation that is not C99:
 * compiler-rt's (irix/fenv.c), on the
 * FPU's control and status register. The values are the register's fields
 * as the MIPS architecture defines them: the flag bits and the rounding
 * mode. */
typedef unsigned int fenv_t;    /* the whole control and status register */
typedef unsigned int fexcept_t; /* its flag bits */

#define FE_INEXACT 0x04
#define FE_UNDERFLOW 0x08
#define FE_OVERFLOW 0x10
#define FE_DIVBYZERO 0x20
#define FE_INVALID 0x40
#define FE_ALL_EXCEPT 0x7c

#define FE_TONEAREST 0
#define FE_TOWARDZERO 1
#define FE_UPWARD 2
#define FE_DOWNWARD 3

#ifdef __cplusplus
extern "C" {
#endif
extern const fenv_t __irix_fe_dfl_env;
#define FE_DFL_ENV (&__irix_fe_dfl_env)
int feclearexcept(int) __asm__("__irix_feclearexcept");
int fegetexceptflag(fexcept_t *, int) __asm__("__irix_fegetexceptflag");
int feraiseexcept(int) __asm__("__irix_feraiseexcept");
int fesetexceptflag(const fexcept_t *, int) __asm__("__irix_fesetexceptflag");
int fetestexcept(int) __asm__("__irix_fetestexcept");
int fegetround(void) __asm__("__irix_fegetround");
int fesetround(int) __asm__("__irix_fesetround");
int fegetenv(fenv_t *) __asm__("__irix_fegetenv");
int feholdexcept(fenv_t *) __asm__("__irix_feholdexcept");
int fesetenv(const fenv_t *) __asm__("__irix_fesetenv");
int feupdateenv(const fenv_t *) __asm__("__irix_feupdateenv");
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_FENV_H */
