/*===---- sys/wait.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_SYS_WAIT_H
#define __CLANG_IRIX_SYS_WAIT_H

#include <sys/types.h>
#include_next <sys/wait.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
struct rusage;
#ifdef __cplusplus
extern "C" {
#endif
pid_t wait3(int *, int, struct rusage *);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_SYS_WAIT_H */
