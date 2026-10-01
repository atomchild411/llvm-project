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
/* POSIX 2008's (compiler-rt's irix/atfile.c; AT_ constants in <fcntl.h>). */
int faccessat(int, const char *, int, int);
int fchownat(int, const char *, uid_t, gid_t, int);
int linkat(int, const char *, int, const char *, int);
ssize_t readlinkat(int, const char *__restrict, char *__restrict, size_t);
int symlinkat(const char *, int, const char *);
int unlinkat(int, const char *, int);
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
/* The rest of POSIX.1-2008's. */
#ifndef _SC_2_PBS
#define _SC_2_PBS 1012
#endif
#ifndef _SC_2_PBS_ACCOUNTING
#define _SC_2_PBS_ACCOUNTING 1013
#endif
#ifndef _SC_2_PBS_CHECKPOINT
#define _SC_2_PBS_CHECKPOINT 1014
#endif
#ifndef _SC_2_PBS_LOCATE
#define _SC_2_PBS_LOCATE 1015
#endif
#ifndef _SC_2_PBS_MESSAGE
#define _SC_2_PBS_MESSAGE 1016
#endif
#ifndef _SC_2_PBS_TRACK
#define _SC_2_PBS_TRACK 1017
#endif
#ifndef _SC_ADVISORY_INFO
#define _SC_ADVISORY_INFO 1018
#endif
#ifndef _SC_IPV6
#define _SC_IPV6 1019
#endif
#ifndef _SC_RAW_SOCKETS
#define _SC_RAW_SOCKETS 1020
#endif
#ifndef _SC_REGEXP
#define _SC_REGEXP 1021
#endif
#ifndef _SC_SHELL
#define _SC_SHELL 1022
#endif
#ifndef _SC_SPAWN
#define _SC_SPAWN 1023
#endif
#ifndef _SC_SPORADIC_SERVER
#define _SC_SPORADIC_SERVER 1024
#endif
#ifndef _SC_SS_REPL_MAX
#define _SC_SS_REPL_MAX 1025
#endif
#ifndef _SC_THREAD_ROBUST_PRIO_INHERIT
#define _SC_THREAD_ROBUST_PRIO_INHERIT 1026
#endif
#ifndef _SC_THREAD_ROBUST_PRIO_PROTECT
#define _SC_THREAD_ROBUST_PRIO_PROTECT 1027
#endif
#ifndef _SC_THREAD_SPORADIC_SERVER
#define _SC_THREAD_SPORADIC_SERVER 1028
#endif
#ifndef _SC_TIMEOUTS
#define _SC_TIMEOUTS 1029
#endif
#ifndef _SC_TRACE
#define _SC_TRACE 1030
#endif
#ifndef _SC_TRACE_EVENT_FILTER
#define _SC_TRACE_EVENT_FILTER 1031
#endif
#ifndef _SC_TRACE_EVENT_NAME_MAX
#define _SC_TRACE_EVENT_NAME_MAX 1032
#endif
#ifndef _SC_TRACE_INHERIT
#define _SC_TRACE_INHERIT 1033
#endif
#ifndef _SC_TRACE_LOG
#define _SC_TRACE_LOG 1034
#endif
#ifndef _SC_TRACE_NAME_MAX
#define _SC_TRACE_NAME_MAX 1035
#endif
#ifndef _SC_TRACE_SYS_MAX
#define _SC_TRACE_SYS_MAX 1036
#endif
#ifndef _SC_TRACE_USER_EVENT_MAX
#define _SC_TRACE_USER_EVENT_MAX 1037
#endif
#ifndef _SC_TYPED_MEMORY_OBJECTS
#define _SC_TYPED_MEMORY_OBJECTS 1038
#endif
#ifndef _SC_V6_ILP32_OFF32
#define _SC_V6_ILP32_OFF32 1039
#endif
#ifndef _SC_V6_ILP32_OFFBIG
#define _SC_V6_ILP32_OFFBIG 1040
#endif
#ifndef _SC_V6_LP64_OFF64
#define _SC_V6_LP64_OFF64 1041
#endif
#ifndef _SC_V6_LPBIG_OFFBIG
#define _SC_V6_LPBIG_OFFBIG 1042
#endif
#ifndef _SC_V7_ILP32_OFF32
#define _SC_V7_ILP32_OFF32 1043
#endif
#ifndef _SC_V7_ILP32_OFFBIG
#define _SC_V7_ILP32_OFFBIG 1044
#endif
#ifndef _SC_V7_LP64_OFF64
#define _SC_V7_LP64_OFF64 1045
#endif
#ifndef _SC_V7_LPBIG_OFFBIG
#define _SC_V7_LPBIG_OFFBIG 1046
#endif
#ifndef _SC_XOPEN_REALTIME_THREADS
#define _SC_XOPEN_REALTIME_THREADS 1047
#endif
#ifndef _SC_XOPEN_STREAMS
#define _SC_XOPEN_STREAMS 1048
#endif
#ifndef _SC_XOPEN_UUCP
#define _SC_XOPEN_UUCP 1049
#endif

#endif /* __CLANG_IRIX_UNISTD_H */
