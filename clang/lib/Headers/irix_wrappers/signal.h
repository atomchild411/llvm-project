/*===---- signal.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_SIGNAL_H
#define __CLANG_IRIX_SIGNAL_H

#include <sys/types.h>
#include_next <signal.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
int killpg(pid_t, int);
int sighold(int);
int sigignore(int);
int siginterrupt(int, int);
int sigpause(int);
int sigrelse(int);
void (*sigset(int, void (*)(int)))(int);
#ifdef __cplusplus
}
#endif
#endif

/* In IRIX's libc, and declared by no IRIX header a program would look in
 * for it: declared here, with IRIX's own prototypes. */
#ifdef __cplusplus
extern "C" {
#endif
#if _POSIX93 || _XOPEN4UX || _XOPEN5 /* where IRIX defines siginfo_t */
void psiginfo(siginfo_t *, const char *);
#endif
void psignal(int, const char *);
int pthread_sigmask(int, const sigset_t *, sigset_t *);
#ifdef __cplusplus
}
#endif

/* BSD's, in IRIX's libc, which its header declares only in SGI mode:
 * declared here outside it, with IRIX's own prototypes (glibc gives them
 * with _DEFAULT_SOURCE; Python, among others, uses them). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
void (*bsd_signal(int, void (*)(int)))(int);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_SIGNAL_H */
