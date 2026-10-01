/*===---- grp.h - IRIX wrapper ----------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_GRP_H
#define __CLANG_IRIX_GRP_H

#include <sys/types.h>
#include_next <grp.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
struct group;
#ifdef __cplusplus
extern "C" {
#endif
void endgrent(void);
struct group *getgrent(void);
void setgrent(void);
#ifdef __cplusplus
}
#endif
#endif

/* In IRIX's libc, and declared by no IRIX header a program would look in
 * for it: declared here, with IRIX's own prototypes. */
#ifdef __cplusplus
extern "C" {
#endif
int initgroups(const char *, gid_t);
int setgroups(int, const gid_t *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_GRP_H */
