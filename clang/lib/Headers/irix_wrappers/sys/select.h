/*===---- sys/select.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX has <sys/select.h> define struct timeval, which select() takes.
 * IRIX's (6.5.7 and 6.5.22 alike) does not: struct timeval is in
 * <sys/time.h>, so code that includes only <sys/select.h> (libX11's
 * xcb_io.c) finds the type incomplete. Include <sys/time.h> too.
 */

#ifndef __CLANG_IRIX_SYS_SELECT_H
#define __CLANG_IRIX_SYS_SELECT_H

/* 6.5.22's uses SGI's namespace macros without including their definitions
 * (<sys/types.h> brings them), which fails when it comes first. */
#include <sys/types.h>
#include_next <sys/select.h>
#include <sys/time.h>

/* IRIX gives struct timeval and select() only in X/Open, SGI and BSD modes.
 * POSIX.1-2001 has both in <sys/select.h> without X/Open, so a program that
 * asks for _POSIX_C_SOURCE 200112L alone (mpg123) gets neither: declare them
 * here, the same layout and the same function. */
/* IRIX declares select in <sys/time.h> in X/Open UX mode, and 6.5.22 (it
 * has internal/ headers) in every X/Open mode, as a static function there. */
#if __has_include(<internal/wchar_core.h>)
#define __CLANG_IRIX_HAS_SELECT (_XOPEN4UX || _XOPEN5)
#else
#define __CLANG_IRIX_HAS_SELECT _XOPEN4UX
#endif
#if !(__CLANG_IRIX_HAS_SELECT || defined(_BSD_TYPES) || defined(_BSD_COMPAT)) && \
    defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE + 0 >= 200112L
#ifndef _TIMEVAL_T
#define _TIMEVAL_T
struct timeval {
#if _MIPS_SZLONG == 64
  int : 32;
#endif
  time_t tv_sec;
  long tv_usec;
};
#endif
#ifdef __cplusplus
extern "C"
#endif
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif
#undef __CLANG_IRIX_HAS_SELECT

/* pselect(), which IRIX lacks (6.5.7 and 6.5.22): compiler-rt's IRIX
 * builtins provide it over sigprocmask and select.  POSIX has this header
 * define sigset_t; IRIX's <signal.h> cannot be included from here (it needs
 * types that come later), so sigset_t is declared under IRIX's own guard,
 * with its layout. */
#ifndef _SIGSET_T
#define _SIGSET_T
typedef struct {
  __uint32_t __sigbits[4];
} sigset_t;
#endif
/* The timespec tag IRIX uses in this mode, as in <sys/stat.h>: IRIX's
 * <sys/types.h> includes this header, so <sys/timespec.h> cannot be. */
#ifdef __cplusplus
extern "C" {
#endif
#if _POSIX93 || _ABIAPI || _XOPEN5
struct timespec;
int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *,
            const sigset_t *);
#else
struct __timespec;
int pselect(int, fd_set *, fd_set *, fd_set *, const struct __timespec *,
            const sigset_t *);
#endif
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_SYS_SELECT_H */
