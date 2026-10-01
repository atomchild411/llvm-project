/*===---- __irix_endian_swap.h - IRIX byte-order macros ---------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * Shared by the <endian.h> and <sys/endian.h> wrappers: the byte-order
 * names in every spelling (IRIX's sys/endian.h defines LITTLE_ENDIAN,
 * BIG_ENDIAN, PDP_ENDIAN and BYTE_ORDER only in SGI mode), glibc's
 * __BYTE_ORDER family, and the htobe16 ... le64toh conversions of glibc and
 * the BSDs.
 */
#ifndef __CLANG_IRIX_ENDIAN_SWAP_H
#define __CLANG_IRIX_ENDIAN_SWAP_H

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif
#ifndef PDP_ENDIAN
#define PDP_ENDIAN 3412
#endif
#ifndef BYTE_ORDER
#ifdef __MIPSEL__
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __BIG_ENDIAN BIG_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN
#define __BYTE_ORDER BYTE_ORDER
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define htobe16(x) ((unsigned short)(x))
#define htobe32(x) ((unsigned int)(x))
#define htobe64(x) ((unsigned long long)(x))
#define htole16(x) __builtin_bswap16(x)
#define htole32(x) __builtin_bswap32(x)
#define htole64(x) __builtin_bswap64(x)
#else
#define htobe16(x) __builtin_bswap16(x)
#define htobe32(x) __builtin_bswap32(x)
#define htobe64(x) __builtin_bswap64(x)
#define htole16(x) ((unsigned short)(x))
#define htole32(x) ((unsigned int)(x))
#define htole64(x) ((unsigned long long)(x))
#endif
#define be16toh(x) htobe16(x)
#define be32toh(x) htobe32(x)
#define be64toh(x) htobe64(x)
#define le16toh(x) htole16(x)
#define le32toh(x) htole32(x)
#define le64toh(x) htole64(x)
/* The BSDs' older names. */
#define betoh16(x) be16toh(x)
#define betoh32(x) be32toh(x)
#define betoh64(x) be64toh(x)
#define letoh16(x) le16toh(x)
#define letoh32(x) le32toh(x)
#define letoh64(x) le64toh(x)

#endif /* __CLANG_IRIX_ENDIAN_SWAP_H */
