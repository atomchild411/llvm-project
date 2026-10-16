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
/* In SGI mode IRIX's header includes <string.h> inside its extern "C"
 * block, and is itself included from inside <sys/time.h>'s.  In C++ a
 * <string.h> with templates (libc++'s overloads, gnulib's replacement:
 * gnutls) cannot be read there: read it first, with C++ linkage (its C
 * declarations give themselves C linkage). */
#if defined(__cplusplus) && _SGIAPI
extern "C++" {
#include <string.h>
}
#endif
#include_next <sys/select.h>

/* select() itself, wherever IRIX declares it extern: in <sys/time.h> in the
 * X/Open UX modes, in <unistd.h> in SGI and BSD modes. POSIX (and glibc)
 * declare it in this header, and gnulib's replacement <sys/select.h> counts
 * on that: it includes this one, then defines select to rpl_select. IRIX's
 * own declaration comes after <sys/time.h>'s #include <sys/select.h>, so it
 * declared rpl_select instead, and gnulib's select.c, which calls the
 * system's select, found none (m4). Not where IRIX's <sys/time.h> has its
 * X/Open 4 static select instead. 6.5.22 (it has internal/ headers) extends
 * both to X/Open 5. */
#if __has_include(<internal/wchar_core.h>)
#define __CLANG_IRIX_XOPEN_SELECT (_XOPEN4UX || _XOPEN5)
#define __CLANG_IRIX_EXTERN_SELECT ((_NO_XOPEN4 && _NO_XOPEN5) || _ABIAPI)
#else
#define __CLANG_IRIX_XOPEN_SELECT _XOPEN4UX
#define __CLANG_IRIX_EXTERN_SELECT (_NO_XOPEN4 || _ABIAPI)
#endif
#if (__CLANG_IRIX_XOPEN_SELECT || _SGIAPI || defined(_BSD_TYPES) || \
     defined(_BSD_COMPAT)) &&                                          \
    !(__CLANG_IRIX_XOPEN_SELECT && !__CLANG_IRIX_EXTERN_SELECT)
struct timeval;
#ifdef __cplusplus
extern "C"
#endif
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#endif
#undef __CLANG_IRIX_XOPEN_SELECT
#undef __CLANG_IRIX_EXTERN_SELECT

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
