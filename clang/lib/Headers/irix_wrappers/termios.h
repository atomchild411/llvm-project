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
int cfsetspeed(struct termios *, speed_t);
#ifdef __cplusplus
}
#endif
/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
pid_t tcgetsid(int);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_TERMIOS_H */
