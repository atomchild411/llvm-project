/*===---- endian.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX has no <endian.h> (glibc, the BSDs, Solaris 11 do): IRIX's own
 * <sys/endian.h> (after <standards.h>, which it requires) and the byte-order
 * names and conversions it lacks.
 */
#ifndef __CLANG_IRIX_ENDIAN_H
#define __CLANG_IRIX_ENDIAN_H
#include <sys/endian.h>
#endif /* __CLANG_IRIX_ENDIAN_H */
