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

/* The speeds past B115200 that Linux and the BSDs name: with IRIX's numeric
 * speeds (its new termios, the default), a speed is the rate itself, up to
 * __NEW_MAX_BAUD (500000), and these are within it. Whether a port runs at
 * one is its driver's to say. */
#if !defined(_OLD_TERMIOS) && !_ABIAPI && defined(__NEW_MAX_BAUD)
#ifndef B230400
#define B230400 230400
#endif
#ifndef B460800
#define B460800 460800
#endif
#endif

#endif /* __CLANG_IRIX_TERMIOS_H */
