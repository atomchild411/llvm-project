/*===---- unistd.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <unistd.h> does not say that _exit never returns. Say so.
 *
 * sysconf: names that POSIX, the BSDs or glibc have and IRIX 6.5 does not.
 * Where IRIX has the value under another name, the name is an alias
 * (_SC_NPROCESSORS_ONLN is IRIX's _SC_NPROC_ONLN); the rest get numbers of
 * their own, above IRIX's, and sysconf is bound to compiler-rt's
 * irix/sysconf.c, which answers them and passes every other name to IRIX's
 * sysconf.
 */

#ifndef __CLANG_IRIX_UNISTD_H
#define __CLANG_IRIX_UNISTD_H

/* IRIX's <unistd.h> includes <getopt.h>: tell the <getopt.h> wrapper, so that
 * getopt_long and struct option are only declared for a direct include. */
#ifndef __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT_UNISTD
#endif
#define sysconf __irix_libc_sysconf
#include_next <unistd.h>
#undef sysconf
#ifdef __IRIX_GETOPT_INDIRECT_UNISTD
#undef __IRIX_GETOPT_INDIRECT
#undef __IRIX_GETOPT_INDIRECT_UNISTD
#endif

#ifdef __cplusplus
extern "C" {
#endif
void _exit(int) __attribute__((__noreturn__));
long sysconf(int) __asm__("__irix_sysconf");
#ifdef __cplusplus
}
#endif

#ifndef _SC_NPROCESSORS_CONF
#define _SC_NPROCESSORS_CONF _SC_NPROC_CONF
#endif
#ifndef _SC_NPROCESSORS_ONLN
#define _SC_NPROCESSORS_ONLN _SC_NPROC_ONLN
#endif
#ifndef _SC_LOGIN_NAME_MAX
#define _SC_LOGIN_NAME_MAX _SC_LOGNAME_MAX
#endif
/* Answered by compiler-rt irix/sysconf.c. */
#ifndef _SC_HOST_NAME_MAX
#define _SC_HOST_NAME_MAX 1001
#endif
#ifndef _SC_PHYS_PAGES
#define _SC_PHYS_PAGES 1002
#endif
#ifndef _SC_AVPHYS_PAGES
#define _SC_AVPHYS_PAGES 1003
#endif
#ifndef _SC_SYMLOOP_MAX
#define _SC_SYMLOOP_MAX 1004
#endif
#ifndef _SC_MONOTONIC_CLOCK
#define _SC_MONOTONIC_CLOCK 1005
#endif
#ifndef _SC_CLOCK_SELECTION
#define _SC_CLOCK_SELECTION 1006
#endif
#ifndef _SC_SPIN_LOCKS
#define _SC_SPIN_LOCKS 1007
#endif
#ifndef _SC_BARRIERS
#define _SC_BARRIERS 1008
#endif
#ifndef _SC_READER_WRITER_LOCKS
#define _SC_READER_WRITER_LOCKS 1009
#endif
#ifndef _SC_CPUTIME
#define _SC_CPUTIME 1010
#endif
#ifndef _SC_THREAD_CPUTIME
#define _SC_THREAD_CPUTIME 1011
#endif

#endif /* __CLANG_IRIX_UNISTD_H */
