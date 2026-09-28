/*===---- math.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <math.h> predates C99's floating-point classification in 6.5.7:
 * no FP_NAN and friends, no fpclassify. Add the constants when missing
 * (the values only need to be consistent; __builtin_fpclassify takes them)
 * and the classification macros on clang's builtins. Likewise C99's
 * INFINITY, NAN, HUGE_VALF and HUGE_VALL.
 *
 * Its libm has most of C99's functions (roundf, lrint, log2, fma, ...) but
 * its header declares only some: declare the rest, for double and float.
 * (Not long double: IRIX's is a pair of doubles, clang's IEEE quad.) The
 * few that only 6.5.22's libc has -- nan, nearbyint, fmin, fmax and some
 * float forms -- are also in compiler-rt's builtins (irix/math_c99.c), so
 * that programs built against 6.5.7 link and run on both.
 */

#ifndef __CLANG_IRIX_MATH_H
#define __CLANG_IRIX_MATH_H

#include_next <math.h>

#ifndef FP_NAN
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4
#endif

#ifndef INFINITY
#define INFINITY __builtin_inff()
#endif
#ifndef NAN
#define NAN __builtin_nanf("")
#endif
#ifndef HUGE_VALF
#define HUGE_VALF __builtin_huge_valf()
#endif
#ifndef HUGE_VALL
#define HUGE_VALL __builtin_huge_vall()
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* In IRIX 6.5.7's libm and later, not declared by its <math.h>. */
float acoshf(float);
float asinhf(float);
float atanhf(float);
double exp2(double);
float exp2f(float);
double log2(double);
float log2f(float);
double scalbn(double, int);
float scalbnf(float, int);
double scalbln(double, long);
float scalblnf(float, long);
float cbrtf(float);
float erff(float);
float erfcf(float);
float lgammaf(float);
double tgamma(double);
float tgammaf(float);
float rintf(float);
long lrint(double);
long lrintf(float);
long long llrint(double);
long long llrintf(float);
double round(double);
float roundf(float);
long lround(double);
long lroundf(float);
long long llround(double);
long long llroundf(float);
float remainderf(float, float);
double remquo(double, double, int *);
float remquof(float, float, int *);
float copysignf(float, float);
double fdim(double, double);
float fdimf(float, float);
double fma(double, double, double);
float fmaf(float, float, float);
/* In 6.5.22's libc; for 6.5.7 in irix/math_c99.c. */
double nan(const char *);
float nanf(const char *);
double nearbyint(double);
float nearbyintf(float);
double fmax(double, double);
float fmaxf(float, float);
double fmin(double, double);
float fminf(float, float);
float frexpf(float, int *);
float ldexpf(float, int);
float fabsf(float);
int ilogbf(float);
float logbf(float);
float nextafterf(float, float);
#ifdef __cplusplus
}
#endif

/* C++ gets these as functions from the C++ library's <cmath>. */
#ifndef __cplusplus
#ifndef fpclassify
#define fpclassify(x)                                                          \
  __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, (x))
#endif
#ifndef isfinite
#define isfinite(x) __builtin_isfinite(x)
#endif
#ifndef isinf
#define isinf(x) __builtin_isinf(x)
#endif
#ifndef isnormal
#define isnormal(x) __builtin_isnormal(x)
#endif
#ifndef signbit
#define signbit(x) __builtin_signbit(x)
#endif
#endif

#endif /* __CLANG_IRIX_MATH_H */
