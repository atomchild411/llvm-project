/*===---- fcntl.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * O_CLOEXEC, which IRIX's open() lacks and modern code uses everywhere
 * (GLib, expat, ...). It is defined to a flag bit IRIX does not use, and
 * open() is routed to compiler-rt's IRIX builtins (irix/cloexec.c), which
 * open without the bit and then set FD_CLOEXEC with fcntl: the traditional
 * two steps, not atomic against a fork in another thread.
 *
 * O_NOFOLLOW, which IRIX lacks too, likewise: the builtins refuse a path
 * that is a symbolic link (ELOOP, as POSIX says), checking it with lstat
 * just before opening -- not atomic against the link changing in between.
 * And O_DIRECTORY: what was opened must be a directory, or the open fails
 * with ENOTDIR (checked with fstat on the descriptor, so exactly).
 *
 * And the AT_ constants and openat (below).
 */

#ifndef __CLANG_IRIX_FCNTL_H
#define __CLANG_IRIX_FCNTL_H

#include_next <fcntl.h>

#ifndef O_CLOEXEC
/* Above every O_ flag IRIX defines (its highest is O_LCFLUSH, 0x40000). */
#define O_CLOEXEC 0x10000000
#define __IRIX_O_CLOEXEC O_CLOEXEC
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0x20000000
#define __IRIX_O_NOFOLLOW O_NOFOLLOW
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 0x40000000
#define __IRIX_O_DIRECTORY O_DIRECTORY
#endif
#ifdef __cplusplus
extern "C" {
#endif
int open(const char *, int, ...) __asm__("__irix_open");
#ifdef __cplusplus
}
#endif
#endif

/* F_DUPFD_CLOEXEC, which IRIX's fcntl() lacks: Linux's command number, which
 * IRIX does not use, and fcntl() routed to compiler-rt's IRIX builtins
 * (irix/fcntl.c), which duplicate with F_DUPFD and then set FD_CLOEXEC (not
 * atomic against a fork in another thread) and pass every other command to
 * IRIX's fcntl(). */
#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC 1030
#define __IRIX_F_DUPFD_CLOEXEC F_DUPFD_CLOEXEC
#ifdef __cplusplus
extern "C" {
#endif
int fcntl(int, int, ...) __asm__("__irix_fcntl");
#ifdef __cplusplus
}
#endif
#endif

/* POSIX 2008's directory-relative calls, which no IRIX has: compiler-rt's
 * irix/atfile.c (which says how they work and what they cannot do). The
 * rest are declared by <sys/stat.h>, <unistd.h>, <stdio.h> and <dirent.h>.
 * The values are Linux's: AT_FDCWD is no descriptor, and AT_EACCESS and
 * AT_REMOVEDIR go to different calls. */
#ifndef AT_FDCWD
#define AT_FDCWD (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200
#define AT_EACCESS 0x200
#define AT_SYMLINK_FOLLOW 0x400
#endif
#ifdef __cplusplus
extern "C" {
#endif
int openat(int, const char *, int, ...);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_FCNTL_H */
