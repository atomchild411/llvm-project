/*===---- byteswap.h - IRIX wrapper ------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * glibc's <byteswap.h>, which IRIX does not have: bswap_16, bswap_32 and
 * bswap_64, here as the compiler's byte-swap builtins.
 */
#ifndef __CLANG_IRIX_BYTESWAP_H
#define __CLANG_IRIX_BYTESWAP_H

#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)

#endif /* __CLANG_IRIX_BYTESWAP_H */
