/*===---- termios.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * cfmakeraw (the BSDs and glibc have it; IRIX does not): compiler-rt's
 * irix/compat_bsd.c.
 */
#ifndef __CLANG_IRIX_TERMIOS_H
#define __CLANG_IRIX_TERMIOS_H
#include_next <termios.h>
#ifdef __cplusplus
extern "C" {
#endif
void cfmakeraw(struct termios *);
#ifdef __cplusplus
}
#endif
#endif /* __CLANG_IRIX_TERMIOS_H */
