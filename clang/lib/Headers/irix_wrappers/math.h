/*===---- math.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <math.h> predates C99's floating-point classification in 6.5.7:
 * no FP_NAN and friends, no fpclassify, isnan only as a function. Add the
 * constants when missing (the values only need to be consistent;
 * __builtin_fpclassify takes them) and the classification and comparison
 * macros on clang's builtins. Likewise C99's
 * INFINITY, NAN, HUGE_VALF and HUGE_VALL, and float_t and double_t.
 *
 * Its libm has most of C99's functions (roundf, lrint, log2, fma, ...) but
 * its header declares only some: declare the rest, for double and float.
 * The few that only 6.5.22's libc has -- nan, nearbyint, fmin, fmax and some
 * float forms -- are also in compiler-rt's builtins (irix/math_c99.c), so
 * that programs built against 6.5.7 link and run on both.
 *
 * clang's long double is a double on IRIX. IRIX's own *l functions take and
 * return MIPSpro's long double, a pair of doubles, so every long double
 * function is declared (by asm label) as its double counterpart, which has
 * the same calling convention now. C++'s <cmath> overloads follow.
 */

#ifndef __CLANG_IRIX_MATH_H
#define __CLANG_IRIX_MATH_H

/* IRIX's HUGE_VAL reads a union in libc (__huge_val.d), which is no
 * constant expression: static initializers with it (graphviz) did not
 * compile. Define it first (as <limits.h> does) as the constant, which IRIX's headers keep. */
#ifndef HUGE_VAL
#define HUGE_VAL __builtin_huge_val()
#endif

#include_next <math.h>

/* C99's float_t and double_t, which IRIX's header lacks: MIPS evaluates
 * float and double in their own precision (FLT_EVAL_METHOD 0). */
#ifdef __c99
typedef float float_t;
typedef double double_t;
#endif

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
/* C23, from irix/math_c99.c. */
double roundeven(double);
float roundevenf(float);
/* C99's, in IRIX's libm, which its <math.h> declares only in some modes
 * (POSIX 2008 mode hides acosh .. trunc; X/Open 7 hides copysign, trunc
 * and float forms): declared here whatever the mode. */
double acosh(double);
double asinh(double);
double atanh(double);
double cbrt(double);
double copysign(double, double);
double erf(double);
double erfc(double);
double expm1(double);
float expm1f(float);
double hypot(double, double);
float hypotf(float, float);
int ilogb(double);
double lgamma(double);
double log1p(double);
float log1pf(float);
double logb(double);
double nextafter(double, double);
double remainder(double, double);
double rint(double);
double trunc(double);
float truncf(float);
/* No IRIX has it: irix/math_c99.c. */
float nexttowardf(float, long double);
#ifdef __cplusplus
}
#endif

/* long double: the double functions, by asm label (see above). */
#define __IRIX_LDBL1(f) long double f##l(long double) __asm__(#f);
#define __IRIX_LDBL2(f) long double f##l(long double, long double) __asm__(#f);
#ifdef __cplusplus
extern "C" {
#endif
__IRIX_LDBL1(fabs) __IRIX_LDBL1(acos) __IRIX_LDBL1(asin) __IRIX_LDBL1(atan)
__IRIX_LDBL1(ceil) __IRIX_LDBL1(cos) __IRIX_LDBL1(cosh) __IRIX_LDBL1(erf)
__IRIX_LDBL1(erfc) __IRIX_LDBL1(exp) __IRIX_LDBL1(floor) __IRIX_LDBL1(log)
__IRIX_LDBL1(log1p) __IRIX_LDBL1(log10) __IRIX_LDBL1(logb) __IRIX_LDBL1(rint)
__IRIX_LDBL1(sin) __IRIX_LDBL1(sinh) __IRIX_LDBL1(sqrt) __IRIX_LDBL1(tan)
__IRIX_LDBL1(tanh) __IRIX_LDBL1(trunc) __IRIX_LDBL1(j0) __IRIX_LDBL1(j1)
__IRIX_LDBL1(y0) __IRIX_LDBL1(y1) __IRIX_LDBL1(gamma) __IRIX_LDBL1(lgamma)
__IRIX_LDBL1(round) __IRIX_LDBL1(nearbyint) __IRIX_LDBL1(cbrt)
__IRIX_LDBL1(exp2) __IRIX_LDBL1(log2) __IRIX_LDBL1(expm1) __IRIX_LDBL1(tgamma)
__IRIX_LDBL1(acosh) __IRIX_LDBL1(asinh) __IRIX_LDBL1(atanh)
__IRIX_LDBL1(roundeven)
__IRIX_LDBL2(atan2) __IRIX_LDBL2(copysign) __IRIX_LDBL2(fmod)
__IRIX_LDBL2(hypot) __IRIX_LDBL2(pow) __IRIX_LDBL2(nextafter)
__IRIX_LDBL2(scalb) __IRIX_LDBL2(fmin) __IRIX_LDBL2(fmax)
__IRIX_LDBL2(remainder) __IRIX_LDBL2(fdim)
long double jnl(int, long double) __asm__("jn");
long double ynl(int, long double) __asm__("yn");
int finitel(long double) __asm__("finite");
int isnanl(long double) __asm__("isnan");
long double frexpl(long double, int *) __asm__("frexp");
long double ldexpl(long double, int) __asm__("ldexp");
long double modfl(long double, long double *) __asm__("modf");
long double fmal(long double, long double, long double) __asm__("fma");
long double remquol(long double, long double, int *) __asm__("remquo");
int ilogbl(long double) __asm__("ilogb");
long double scalbnl(long double, int) __asm__("scalbn");
long double scalblnl(long double, long) __asm__("scalbln");
long double nanl(const char *) __asm__("nan");
long lrintl(long double) __asm__("lrint");
long long llrintl(long double) __asm__("llrint");
long lroundl(long double) __asm__("lround");
long long llroundl(long double) __asm__("llround");
long double nexttowardl(long double, long double) __asm__("nextafter");
double nexttoward(double, long double) __asm__("nextafter");
#if _SGIAPI
/* Two long doubles, so two doubles: cabs's struct. */
long double cabsl(struct __cabsl_s) __asm__("cabs");
#endif
#ifdef __cplusplus
}
#endif
#undef __IRIX_LDBL1
#undef __IRIX_LDBL2

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
/* IRIX has isnan as a function (of a double); C99 makes it a macro. */
#ifndef isnan
#define isnan(x) __builtin_isnan(x)
#endif
#ifndef isgreater
#define isgreater(x, y) __builtin_isgreater((x), (y))
#define isgreaterequal(x, y) __builtin_isgreaterequal((x), (y))
#define isless(x, y) __builtin_isless((x), (y))
#define islessequal(x, y) __builtin_islessequal((x), (y))
#define islessgreater(x, y) __builtin_islessgreater((x), (y))
#define isunordered(x, y) __builtin_isunordered((x), (y))
#endif
#endif

#endif /* __CLANG_IRIX_MATH_H */
