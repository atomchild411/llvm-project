/*===---- sys/termios.h - IRIX wrapper --------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX declares struct winsize, TIOCGWINSZ and TIOCSWINSZ here, but only
 * outside POSIX and X/Open modes (6.5.22 also hides them from X/Open 5), so
 * _XOPEN_SOURCE 600 code that includes <sys/ioctl.h> (mpg123) found none;
 * elsewhere <sys/ioctl.h> has them in every mode, and POSIX 2024 puts them
 * in <termios.h>. Both reach this header: once IRIX's is done, declare them
 * if it did not, the same layout and the same ioctl numbers.
 */

#ifndef __CLANG_IRIX_SYS_TERMIOS_H
#define __CLANG_IRIX_SYS_TERMIOS_H

#include_next <sys/termios.h>

#if !defined(TIOCGWINSZ) && !defined(_SYS_TTOLD_H) && !defined(_SYS_PTEM_H)
#include <sys/ioccom.h>
struct winsize {
  unsigned short ws_row;
  unsigned short ws_col;
  unsigned short ws_xpixel;
  unsigned short ws_ypixel;
};
#define TIOCGWINSZ _IOR('t', 104, struct winsize)
#define TIOCSWINSZ _IOW('t', 103, struct winsize)
#endif

/* CRTSCTS: IRIX spells hardware flow control CNEW_RTSCTS (its RiscOS
 * compatibility name). */
#if !defined(CRTSCTS) && defined(CNEW_RTSCTS)
#define CRTSCTS CNEW_RTSCTS
#endif

#endif /* __CLANG_IRIX_SYS_TERMIOS_H */
