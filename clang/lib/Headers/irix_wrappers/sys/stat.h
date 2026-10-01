/*===---- sys/stat.h - IRIX wrapper ------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * lchmod (the BSDs and glibc have it; IRIX does not): compiler-rt's
 * irix/compat_bsd.c, which changes the mode of anything but a symbolic link
 * and fails with ENOTSUP for a link, as glibc does where the kernel cannot.
 */
#ifndef __CLANG_IRIX_SYS_STAT_H
#define __CLANG_IRIX_SYS_STAT_H
#include_next <sys/stat.h>
#ifdef __cplusplus
extern "C" {
#endif
int lchmod(const char *, mode_t);
#ifdef __cplusplus
}
#endif
#endif /* __CLANG_IRIX_SYS_STAT_H */
