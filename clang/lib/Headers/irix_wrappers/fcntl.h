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
 */

#ifndef __CLANG_IRIX_FCNTL_H
#define __CLANG_IRIX_FCNTL_H

#include_next <fcntl.h>

#ifndef O_CLOEXEC
/* Above every O_ flag IRIX defines (its highest is O_LCFLUSH, 0x40000). */
#define O_CLOEXEC 0x10000000
#define __IRIX_O_CLOEXEC O_CLOEXEC
#ifdef __cplusplus
extern "C" {
#endif
int open(const char *, int, ...) __asm__("__irix_open");
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_FCNTL_H */
