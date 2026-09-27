/*===---- internal/sgimacros.h - IRIX wrapper ------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX 6.5.22's headers (6.5.7's have no sgimacros.h) spell restrict as
 * __restrict, which this file defines as C99's `restrict` wherever __c99 is
 * set. clang sets __c99 in C++ too, where IRIX's C99 declarations are
 * wanted (libc++'s <cstdint>), but C++ has no `restrict` keyword: every
 * prototype would then name a parameter `restrict`. In C++, drop the macro
 * so __restrict is clang's own keyword, which means the same.
 */

#include_next <internal/sgimacros.h>

#if defined(__cplusplus) && defined(__restrict)
#undef __restrict
#endif
