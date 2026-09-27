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

#include_next <stdint.h>

#ifdef INT64_C
#include <__irix_int_c.h>
#endif

#endif /* __CLANG_IRIX_STDINT_H */
