/*===---- sys/bitypes.h - IRIX wrapper --------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * BIND's <sys/bitypes.h>, the BSD fixed-width types, which IRIX does not
 * ship: its <resolv.h> and <arpa/nameser.h> include it unless sgi is
 * defined, and strict C and C++ modes define only __sgi (libsoup builds as
 * C11). IRIX has the types in <sys/types.h>.
 */

#ifndef __CLANG_IRIX_SYS_BITYPES_H
#define __CLANG_IRIX_SYS_BITYPES_H

#include <sys/types.h>

#endif /* __CLANG_IRIX_SYS_BITYPES_H */
