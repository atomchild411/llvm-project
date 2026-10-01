/*===---- sys/ioctl.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * A program that includes <sys/ioctl.h> wants the terminal ioctls, as on
 * every other system, whatever standard it asks for. IRIX's <termios.h>
 * declares struct winsize, TIOCGWINSZ and TIOCSWINSZ only outside POSIX and
 * X/Open modes, so _XOPEN_SOURCE 600 code (mpg123) found none: declare them
 * here then, the same layout and the same ioctl numbers.
 */

#ifndef __CLANG_IRIX_SYS_IOCTL_H
#define __CLANG_IRIX_SYS_IOCTL_H

#include_next <sys/ioctl.h>
#include <standards.h>

/* The mode test is IRIX's own (<standards.h>):
 * in SGI mode <termios.h> declares them itself, and may include this header
 * before it does. */
#if !(_NO_POSIX && _NO_XOPEN4) && !defined(TIOCGWINSZ) &&                     \
    !defined(_SYS_TTOLD_H) && !defined(_SYS_PTEM_H)
struct winsize {
  unsigned short ws_row;
  unsigned short ws_col;
  unsigned short ws_xpixel;
  unsigned short ws_ypixel;
};
#define TIOCGWINSZ _IOR('t', 104, struct winsize)
#define TIOCSWINSZ _IOW('t', 103, struct winsize)
#endif

#endif /* __CLANG_IRIX_SYS_IOCTL_H */
