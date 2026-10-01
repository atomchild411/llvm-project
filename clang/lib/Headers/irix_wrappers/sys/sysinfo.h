/*===---- sys/sysinfo.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * get_nprocs and get_nprocs_conf, which glibc declares here (IRIX's
 * <sys/sysinfo.h> is its kernel statistics): compiler-rt's
 * irix/compat_bsd.c.
 */
#ifndef __CLANG_IRIX_SYS_SYSINFO_H
#define __CLANG_IRIX_SYS_SYSINFO_H
/* IRIX's header (its kernel statistics) uses SGI types such as uint, which
 * a strict POSIX/X/Open compilation does not have: include it only in SGI
 * mode. Code that wants glibc's get_nprocs gets the declarations either way. */
#include <standards.h>
#if _SGIAPI
#include_next <sys/sysinfo.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
int get_nprocs(void);
int get_nprocs_conf(void);
#ifdef __cplusplus
}
#endif
#endif /* __CLANG_IRIX_SYS_SYSINFO_H */
