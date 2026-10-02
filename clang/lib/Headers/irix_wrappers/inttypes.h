/*===---- inttypes.h - IRIX wrapper for C99 format macros -----------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <inttypes.h> declares the fixed-width types but, before 6.5.22,
 * none of C99's PRI and SCN format macros. Add those it lacks, for the types
 * it declares: IRIX's int64_t and intmax_t are long long under n32 and long
 * under n64, and intptr_t is long under both. Its integer constant macros
 * (INT64_C and the rest) are casts: see __irix_int_c.h.
 *
 * C99 also has <inttypes.h> include <stdint.h>; IRIX's does not, so code
 * that includes only <inttypes.h> (nghttp2) went without SIZE_MAX and the
 * other limits. Include it in the modes where IRIX has C99 (__c99: C99 and
 * later, and C++); a C89 compilation keeps IRIX's header as it is.
 *
 * strtoimax, strtoumax and imaxabs: IRIX declares the first two only in its
 * own API mode, and no IRIX library defines any of them; compiler-rt's
 * builtins do (irix/libc_compat.c). Declare them in every C99 mode. IRIX's
 * declarations have no extern "C", so in C++ they would give the functions
 * C++ linkage: they are renamed out of the way while its header is read.
 */

#ifndef __CLANG_IRIX_INTTYPES_H
#define __CLANG_IRIX_INTTYPES_H

#define strtoimax __irix_unused_strtoimax
#define strtoumax __irix_unused_strtoumax
#include_next <inttypes.h>
#undef strtoimax
#undef strtoumax

#ifdef __c99
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
intmax_t strtoimax(const char *__restrict, char **__restrict, int);
uintmax_t strtoumax(const char *__restrict, char **__restrict, int);
intmax_t imaxabs(intmax_t);
#ifdef __cplusplus
}
#endif
#endif

#include <__irix_int_c.h>

#if _MIPS_SZLONG == 64
#define __IRIX_PRI64 "l"
#else
#define __IRIX_PRI64 "ll"
#endif
#define __IRIX_PRIPTR "l"

/* printf */
#ifndef PRId8
#define PRId8 "hhd"
#define PRIi8 "hhi"
#define PRIo8 "hho"
#define PRIu8 "hhu"
#define PRIx8 "hhx"
#define PRIX8 "hhX"
#define PRId16 "hd"
#define PRIi16 "hi"
#define PRIo16 "ho"
#define PRIu16 "hu"
#define PRIx16 "hx"
#define PRIX16 "hX"
#define PRId32 "d"
#define PRIi32 "i"
#define PRIo32 "o"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
#define PRId64 __IRIX_PRI64 "d"
#define PRIi64 __IRIX_PRI64 "i"
#define PRIo64 __IRIX_PRI64 "o"
#define PRIu64 __IRIX_PRI64 "u"
#define PRIx64 __IRIX_PRI64 "x"
#define PRIX64 __IRIX_PRI64 "X"
#endif

#ifndef PRIdMAX
#define PRIdMAX __IRIX_PRI64 "d"
#define PRIiMAX __IRIX_PRI64 "i"
#define PRIoMAX __IRIX_PRI64 "o"
#define PRIuMAX __IRIX_PRI64 "u"
#define PRIxMAX __IRIX_PRI64 "x"
#define PRIXMAX __IRIX_PRI64 "X"
#endif

#ifndef PRIdPTR
#define PRIdPTR __IRIX_PRIPTR "d"
#define PRIiPTR __IRIX_PRIPTR "i"
#define PRIoPTR __IRIX_PRIPTR "o"
#define PRIuPTR __IRIX_PRIPTR "u"
#define PRIxPTR __IRIX_PRIPTR "x"
#define PRIXPTR __IRIX_PRIPTR "X"
#endif

/* scanf */
#ifndef SCNd8
#define SCNd8 "hhd"
#define SCNi8 "hhi"
#define SCNo8 "hho"
#define SCNu8 "hhu"
#define SCNx8 "hhx"
#define SCNd16 "hd"
#define SCNi16 "hi"
#define SCNo16 "ho"
#define SCNu16 "hu"
#define SCNx16 "hx"
#define SCNd32 "d"
#define SCNi32 "i"
#define SCNo32 "o"
#define SCNu32 "u"
#define SCNx32 "x"
#define SCNd64 __IRIX_PRI64 "d"
#define SCNi64 __IRIX_PRI64 "i"
#define SCNo64 __IRIX_PRI64 "o"
#define SCNu64 __IRIX_PRI64 "u"
#define SCNx64 __IRIX_PRI64 "x"
#endif

#ifndef SCNdMAX
#define SCNdMAX __IRIX_PRI64 "d"
#define SCNiMAX __IRIX_PRI64 "i"
#define SCNoMAX __IRIX_PRI64 "o"
#define SCNuMAX __IRIX_PRI64 "u"
#define SCNxMAX __IRIX_PRI64 "x"
#endif

#ifndef SCNdPTR
#define SCNdPTR __IRIX_PRIPTR "d"
#define SCNiPTR __IRIX_PRIPTR "i"
#define SCNoPTR __IRIX_PRIPTR "o"
#define SCNuPTR __IRIX_PRIPTR "u"
#define SCNxPTR __IRIX_PRIPTR "x"
#endif

/* The least and fast types, which IRIX's header has no formats for:
 * clang knows their printf conversions, and scanf takes the same. */
#ifndef PRIdFAST32
#define PRIdLEAST8 __INT_LEAST8_FMTd__
#define PRIiLEAST8 __INT_LEAST8_FMTi__
#define PRIoLEAST8 __UINT_LEAST8_FMTo__
#define PRIuLEAST8 __UINT_LEAST8_FMTu__
#define PRIxLEAST8 __UINT_LEAST8_FMTx__
#define PRIXLEAST8 __UINT_LEAST8_FMTX__
#define PRIdLEAST16 __INT_LEAST16_FMTd__
#define PRIiLEAST16 __INT_LEAST16_FMTi__
#define PRIoLEAST16 __UINT_LEAST16_FMTo__
#define PRIuLEAST16 __UINT_LEAST16_FMTu__
#define PRIxLEAST16 __UINT_LEAST16_FMTx__
#define PRIXLEAST16 __UINT_LEAST16_FMTX__
#define PRIdLEAST32 __INT_LEAST32_FMTd__
#define PRIiLEAST32 __INT_LEAST32_FMTi__
#define PRIoLEAST32 __UINT_LEAST32_FMTo__
#define PRIuLEAST32 __UINT_LEAST32_FMTu__
#define PRIxLEAST32 __UINT_LEAST32_FMTx__
#define PRIXLEAST32 __UINT_LEAST32_FMTX__
#define PRIdLEAST64 __INT_LEAST64_FMTd__
#define PRIiLEAST64 __INT_LEAST64_FMTi__
#define PRIoLEAST64 __UINT_LEAST64_FMTo__
#define PRIuLEAST64 __UINT_LEAST64_FMTu__
#define PRIxLEAST64 __UINT_LEAST64_FMTx__
#define PRIXLEAST64 __UINT_LEAST64_FMTX__
#define PRIdFAST8 __INT_FAST8_FMTd__
#define PRIiFAST8 __INT_FAST8_FMTi__
#define PRIoFAST8 __UINT_FAST8_FMTo__
#define PRIuFAST8 __UINT_FAST8_FMTu__
#define PRIxFAST8 __UINT_FAST8_FMTx__
#define PRIXFAST8 __UINT_FAST8_FMTX__
#define PRIdFAST16 __INT_FAST16_FMTd__
#define PRIiFAST16 __INT_FAST16_FMTi__
#define PRIoFAST16 __UINT_FAST16_FMTo__
#define PRIuFAST16 __UINT_FAST16_FMTu__
#define PRIxFAST16 __UINT_FAST16_FMTx__
#define PRIXFAST16 __UINT_FAST16_FMTX__
#define PRIdFAST32 __INT_FAST32_FMTd__
#define PRIiFAST32 __INT_FAST32_FMTi__
#define PRIoFAST32 __UINT_FAST32_FMTo__
#define PRIuFAST32 __UINT_FAST32_FMTu__
#define PRIxFAST32 __UINT_FAST32_FMTx__
#define PRIXFAST32 __UINT_FAST32_FMTX__
#define PRIdFAST64 __INT_FAST64_FMTd__
#define PRIiFAST64 __INT_FAST64_FMTi__
#define PRIoFAST64 __UINT_FAST64_FMTo__
#define PRIuFAST64 __UINT_FAST64_FMTu__
#define PRIxFAST64 __UINT_FAST64_FMTx__
#define PRIXFAST64 __UINT_FAST64_FMTX__
#endif
#ifndef SCNdFAST32
#define SCNdLEAST8 __INT_LEAST8_FMTd__
#define SCNiLEAST8 __INT_LEAST8_FMTi__
#define SCNoLEAST8 __UINT_LEAST8_FMTo__
#define SCNuLEAST8 __UINT_LEAST8_FMTu__
#define SCNxLEAST8 __UINT_LEAST8_FMTx__
#define SCNdLEAST16 __INT_LEAST16_FMTd__
#define SCNiLEAST16 __INT_LEAST16_FMTi__
#define SCNoLEAST16 __UINT_LEAST16_FMTo__
#define SCNuLEAST16 __UINT_LEAST16_FMTu__
#define SCNxLEAST16 __UINT_LEAST16_FMTx__
#define SCNdLEAST32 __INT_LEAST32_FMTd__
#define SCNiLEAST32 __INT_LEAST32_FMTi__
#define SCNoLEAST32 __UINT_LEAST32_FMTo__
#define SCNuLEAST32 __UINT_LEAST32_FMTu__
#define SCNxLEAST32 __UINT_LEAST32_FMTx__
#define SCNdLEAST64 __INT_LEAST64_FMTd__
#define SCNiLEAST64 __INT_LEAST64_FMTi__
#define SCNoLEAST64 __UINT_LEAST64_FMTo__
#define SCNuLEAST64 __UINT_LEAST64_FMTu__
#define SCNxLEAST64 __UINT_LEAST64_FMTx__
#define SCNdFAST8 __INT_FAST8_FMTd__
#define SCNiFAST8 __INT_FAST8_FMTi__
#define SCNoFAST8 __UINT_FAST8_FMTo__
#define SCNuFAST8 __UINT_FAST8_FMTu__
#define SCNxFAST8 __UINT_FAST8_FMTx__
#define SCNdFAST16 __INT_FAST16_FMTd__
#define SCNiFAST16 __INT_FAST16_FMTi__
#define SCNoFAST16 __UINT_FAST16_FMTo__
#define SCNuFAST16 __UINT_FAST16_FMTu__
#define SCNxFAST16 __UINT_FAST16_FMTx__
#define SCNdFAST32 __INT_FAST32_FMTd__
#define SCNiFAST32 __INT_FAST32_FMTi__
#define SCNoFAST32 __UINT_FAST32_FMTo__
#define SCNuFAST32 __UINT_FAST32_FMTu__
#define SCNxFAST32 __UINT_FAST32_FMTx__
#define SCNdFAST64 __INT_FAST64_FMTd__
#define SCNiFAST64 __INT_FAST64_FMTi__
#define SCNoFAST64 __UINT_FAST64_FMTo__
#define SCNuFAST64 __UINT_FAST64_FMTu__
#define SCNxFAST64 __UINT_FAST64_FMTx__
#endif

#endif /* __CLANG_IRIX_INTTYPES_H */
