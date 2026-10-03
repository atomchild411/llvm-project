/*===---- sys/kabi.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/kabi.h>, which <sys/proc.h> includes, declares the kernel's
 * psignal(proc_handl_t *, int) even outside the kernel. clang's IRIX
 * <signal.h> declares POSIX's psignal(int, const char *), which IRIX's libc
 * has, and a program that includes both (libstatgrab, for <sys/proc.h>)
 * stopped on conflicting types. Read the kernel's under another name.
 */

#ifndef __CLANG_IRIX_SYS_KABI_H
#define __CLANG_IRIX_SYS_KABI_H

#define psignal __irix_kernel_psignal
#include_next <sys/kabi.h>
#undef psignal

#endif /* __CLANG_IRIX_SYS_KABI_H */
