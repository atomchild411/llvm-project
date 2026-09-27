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
 * under n64, and intptr_t is long under both.
 */

#ifndef __CLANG_IRIX_INTTYPES_H
#define __CLANG_IRIX_INTTYPES_H

#include_next <inttypes.h>

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

#endif /* __CLANG_IRIX_INTTYPES_H */
