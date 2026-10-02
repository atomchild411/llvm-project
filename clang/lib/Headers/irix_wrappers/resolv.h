/*===---- resolv.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <resolv.h> includes <sys/bitypes.h>, which IRIX does not have,
 * unless sgi is defined; a strict C or C++ mode defines only __sgi (libsoup
 * builds as C11). Define sgi while it is read.
 */

#ifndef __CLANG_IRIX_RESOLV_H
#define __CLANG_IRIX_RESOLV_H

#ifndef sgi
#define sgi 1
#define __CLANG_IRIX_RESOLV_SGI
#endif
#include_next <resolv.h>
#ifdef __CLANG_IRIX_RESOLV_SGI
#undef sgi
#undef __CLANG_IRIX_RESOLV_SGI
#endif

#endif /* __CLANG_IRIX_RESOLV_H */
