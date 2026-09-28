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

#endif /* __CLANG_IRIX_FCNTL_H */
