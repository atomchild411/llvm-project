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
 * And POSIX 2008's fstatat and friends, with UTIME_NOW and UTIME_OMIT.
 */
#ifndef __CLANG_IRIX_SYS_STAT_H
#define __CLANG_IRIX_SYS_STAT_H
/* POSIX 2008 has <sys/stat.h> give struct timespec as <time.h> does; IRIX's
 * X/Open 5 mode names it only once <time.h> is in (utimensat below). */
#include <time.h>
#include_next <sys/stat.h>
#ifndef UTIME_NOW
#define UTIME_NOW ((1L << 30) - 1)
#define UTIME_OMIT ((1L << 30) - 2)
#endif
#ifdef __cplusplus
extern "C" {
#endif
int lchmod(const char *, mode_t);
/* POSIX 2008's (compiler-rt's irix/atfile.c; AT_ constants in <fcntl.h>).
 * No futimens: IRIX cannot set a descriptor's times. */
int fstatat(int, const char *, struct stat *, int);
int fchmodat(int, const char *, mode_t, int);
int mkdirat(int, const char *, mode_t);
int mknodat(int, const char *, mode_t, dev_t);
int mkfifoat(int, const char *, mode_t);
/* IRIX names the struct timespec only in POSIX.1b and later X/Open modes
 * (its <sys/timespec.h>), __timespec otherwise; a pointer (what the
 * array parameter is) needs the tag only, not the definition, which an
 * include cycle can leave for later. */
#if _POSIX93 || _ABIAPI || _XOPEN5
struct timespec;
int utimensat(int, const char *, const struct timespec *, int);
#else
struct __timespec;
int utimensat(int, const char *, const struct __timespec *, int);
#endif
#ifdef __cplusplus
}
#endif
/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
int fchmod(int, mode_t);
int lstat(const char *, struct stat *);
int mknod(const char *, mode_t, dev_t);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_SYS_STAT_H */
