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
