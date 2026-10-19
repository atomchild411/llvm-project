/*===---- sys/cdefs.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/cdefs.h> has __P, __CONCAT, __STRING and __BEGIN_DECLS.
 * BSD-derived code also expects the ID-string and attribute macros of the
 * BSDs' one; those that cannot collide with a program's identifiers are
 * defined here when missing. (__unused, __dead and __packed are left out:
 * code written for glibc, which has none of them, uses those names.)
 */

#ifndef __CLANG_IRIX_SYS_CDEFS_H
#define __CLANG_IRIX_SYS_CDEFS_H

#include_next <sys/cdefs.h>

#ifndef __IDSTRING
#define __IDSTRING(name, string) /* nothing */
#endif
#ifndef __RCSID
#define __RCSID(s) /* nothing */
#endif
#ifndef __SCCSID
#define __SCCSID(s) /* nothing */
#endif
#ifndef __COPYRIGHT
#define __COPYRIGHT(s) /* nothing */
#endif
#ifndef __KERNEL_RCSID
#define __KERNEL_RCSID(n, s) /* nothing */
#endif
#ifndef __FBSDID
#define __FBSDID(s) /* nothing */
#endif
#ifndef __printflike
#define __printflike(fmt, arg) __attribute__((__format__(__printf__, fmt, arg)))
#endif
#ifndef __scanflike
#define __scanflike(fmt, arg) __attribute__((__format__(__scanf__, fmt, arg)))
#endif
#ifndef __predict_true
#define __predict_true(exp) __builtin_expect((exp) != 0, 1)
#endif
#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif
#ifndef __UNCONST
#define __UNCONST(a) ((void *)(unsigned long)(const void *)(a))
#endif
#ifndef __UNVOLATILE
#define __UNVOLATILE(a) ((void *)(unsigned long)(volatile void *)(a))
#endif
#ifndef __arraycount
#define __arraycount(a) (sizeof(a) / sizeof((a)[0]))
#endif

#endif /* __CLANG_IRIX_SYS_CDEFS_H */
