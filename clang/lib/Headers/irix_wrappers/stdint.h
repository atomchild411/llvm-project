/*===---- stdint.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <stdint.h> defines its integer constant macros (INT64_C and the
 * rest) as casts: see __irix_int_c.h.
 */

#ifndef __CLANG_IRIX_STDINT_H
#define __CLANG_IRIX_STDINT_H

#if __has_include_next(<stdint.h>) && defined(__c99)
/* IRIX's C99 <stdint.h> comes with MIPSpro 7.3/7.4's headers, and stops
 * with #error unless the compilation is C99 (__c99); C89 and gnu89 code
 * (fribidi) takes the branch below. */
#include_next <stdint.h>
#ifdef INT64_C
#include <__irix_int_c.h>
#endif
#else
/* A root without MIPSpro 7.3+'s headers (a stock 6.5.22 has none), or a
 * compilation that is not C99: IRIX's
 * pre-C99 <inttypes.h> (through our wrapper, which fixes its INTn_C) has the
 * exact-width types, intmax_t and intptr_t with their limits. Add the rest
 * of C99's <stdint.h> from clang's predefined macros. (clang's own
 * <stdint.h> cannot be used here: it comes first in the search and hands
 * over to this file.) */
#include <inttypes.h>

typedef __INT_LEAST8_TYPE__ int_least8_t;
typedef __UINT_LEAST8_TYPE__ uint_least8_t;
typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __INT_LEAST16_TYPE__ int_least16_t;
typedef __UINT_LEAST16_TYPE__ uint_least16_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __INT_LEAST32_TYPE__ int_least32_t;
typedef __UINT_LEAST32_TYPE__ uint_least32_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __INT_LEAST64_TYPE__ int_least64_t;
typedef __UINT_LEAST64_TYPE__ uint_least64_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

#define INT_LEAST8_MIN (-__INT_LEAST8_MAX__ - 1)
#define INT_LEAST8_MAX __INT_LEAST8_MAX__
#define UINT_LEAST8_MAX __UINT_LEAST8_MAX__
#define INT_FAST8_MIN (-__INT_FAST8_MAX__ - 1)
#define INT_FAST8_MAX __INT_FAST8_MAX__
#define UINT_FAST8_MAX __UINT_FAST8_MAX__
#define INT_LEAST16_MIN (-__INT_LEAST16_MAX__ - 1)
#define INT_LEAST16_MAX __INT_LEAST16_MAX__
#define UINT_LEAST16_MAX __UINT_LEAST16_MAX__
#define INT_FAST16_MIN (-__INT_FAST16_MAX__ - 1)
#define INT_FAST16_MAX __INT_FAST16_MAX__
#define UINT_FAST16_MAX __UINT_FAST16_MAX__
#define INT_LEAST32_MIN (-__INT_LEAST32_MAX__ - 1)
#define INT_LEAST32_MAX __INT_LEAST32_MAX__
#define UINT_LEAST32_MAX __UINT_LEAST32_MAX__
#define INT_FAST32_MIN (-__INT_FAST32_MAX__ - 1)
#define INT_FAST32_MAX __INT_FAST32_MAX__
#define UINT_FAST32_MAX __UINT_FAST32_MAX__
#define INT_LEAST64_MIN (-__INT_LEAST64_MAX__ - 1)
#define INT_LEAST64_MAX __INT_LEAST64_MAX__
#define UINT_LEAST64_MAX __UINT_LEAST64_MAX__
#define INT_FAST64_MIN (-__INT_FAST64_MAX__ - 1)
#define INT_FAST64_MAX __INT_FAST64_MAX__
#define UINT_FAST64_MAX __UINT_FAST64_MAX__

#ifndef PTRDIFF_MIN
#define PTRDIFF_MIN (-__PTRDIFF_MAX__ - 1)
#define PTRDIFF_MAX __PTRDIFF_MAX__
#endif
#ifndef SIZE_MAX
#define SIZE_MAX __SIZE_MAX__
#endif
#ifndef SIG_ATOMIC_MIN
#define SIG_ATOMIC_MIN (-__SIG_ATOMIC_MAX__ - 1)
#define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#endif
#ifndef WINT_MIN
#define WINT_MIN (-__WINT_MAX__ - 1)
#define WINT_MAX __WINT_MAX__
#endif
#ifndef WCHAR_MIN
#define WCHAR_MIN (-__WCHAR_MAX__ - 1)
#define WCHAR_MAX __WCHAR_MAX__
#endif
#endif

#endif /* __CLANG_IRIX_STDINT_H */
