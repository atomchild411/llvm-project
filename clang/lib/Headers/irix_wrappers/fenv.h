/*===---- fenv.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <fenv.h> declares libm's functions without extern "C", so C++
 * callers would look for mangled names libm does not have. Give them C
 * linkage.
 */

#ifndef __CLANG_IRIX_FENV_H
#define __CLANG_IRIX_FENV_H

#ifdef __cplusplus
extern "C" {
#endif
#include_next <fenv.h>
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_FENV_H */
