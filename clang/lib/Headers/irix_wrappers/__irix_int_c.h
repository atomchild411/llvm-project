/*===---- __irix_int_c.h - C99 integer constant macros for IRIX -----------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <stdint.h> and <inttypes.h> define INT64_C and the rest as casts,
 * and <stdint.h>'s without a suffix, so UINT64_C(10000000000000000000)
 * overflows before the cast, and none can be used in #if. C99 wants integer
 * constants of the right type: clang's own macros make those. Included after
 * either header, each time, since each redefines them. No include guard.
 */

#undef INT8_C
#undef INT16_C
#undef INT32_C
#undef INT64_C
#undef UINT8_C
#undef UINT16_C
#undef UINT32_C
#undef UINT64_C
#undef INTMAX_C
#undef UINTMAX_C
#define INT8_C(c) __INT8_C(c)
#define INT16_C(c) __INT16_C(c)
#define INT32_C(c) __INT32_C(c)
#define INT64_C(c) __INT64_C(c)
#define UINT8_C(c) __UINT8_C(c)
#define UINT16_C(c) __UINT16_C(c)
#define UINT32_C(c) __UINT32_C(c)
#define UINT64_C(c) __UINT64_C(c)
#define INTMAX_C(c) __INTMAX_C(c)
#define UINTMAX_C(c) __UINTMAX_C(c)
