/*===---- unistd.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <unistd.h> does not say that _exit never returns. Say so.
 */

#ifndef __CLANG_IRIX_UNISTD_H
#define __CLANG_IRIX_UNISTD_H

/* IRIX's <unistd.h> includes <getopt.h>: tell the <getopt.h> wrapper, so that
 * getopt_long and struct option are only declared for a direct include. */
#ifndef __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT_UNISTD
#endif
#include_next <unistd.h>
#ifdef __IRIX_GETOPT_INDIRECT_UNISTD
#undef __IRIX_GETOPT_INDIRECT
#undef __IRIX_GETOPT_INDIRECT_UNISTD
#endif

#ifdef __cplusplus
extern "C" {
#endif
void _exit(int) __attribute__((__noreturn__));
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_UNISTD_H */
