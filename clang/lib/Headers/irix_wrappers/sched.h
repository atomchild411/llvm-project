/*===---- sched.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sched.h> includes <sys/sched.h>, which in SGI mode includes
 * <sys/param.h>, so every program that includes <pthread.h> -- in C++ all
 * that include libc++'s thread-aware headers (<list>, <queue>, <mutex>, ...)
 * -- gets its MIN, MAX and TICK macros, which glibc's <sched.h> does not
 * give. C++ code that names a function or member MIN, MAX or TICK then fails
 * (freeimage, clustalw, espeak-ng). In C++, when <sys/param.h> came in only
 * this way, take back those of the three it brought, but not one defined
 * before (glib's MIN and MAX: gegl); the <sys/param.h> wrapper defines them
 * again on any later #include of it. C is left as it was.
 */

#ifndef __CLANG_IRIX_SCHED_H
#define __CLANG_IRIX_SCHED_H

#if defined(__cplusplus) && !defined(_SYS_PARAM_H)
#define __CLANG_IRIX_SCHED_PARAM
#ifdef MIN
#define __CLANG_IRIX_SCHED_HAD_MIN
#endif
#ifdef MAX
#define __CLANG_IRIX_SCHED_HAD_MAX
#endif
#ifdef TICK
#define __CLANG_IRIX_SCHED_HAD_TICK
#endif
#endif

#include_next <sched.h>

#ifdef __CLANG_IRIX_SCHED_PARAM
#undef __CLANG_IRIX_SCHED_PARAM
#ifndef __CLANG_IRIX_SCHED_HAD_MIN
#undef MIN
#endif
#ifndef __CLANG_IRIX_SCHED_HAD_MAX
#undef MAX
#endif
#ifndef __CLANG_IRIX_SCHED_HAD_TICK
#undef TICK
#endif
#undef __CLANG_IRIX_SCHED_HAD_MIN
#undef __CLANG_IRIX_SCHED_HAD_MAX
#undef __CLANG_IRIX_SCHED_HAD_TICK
#endif

#endif /* __CLANG_IRIX_SCHED_H */
