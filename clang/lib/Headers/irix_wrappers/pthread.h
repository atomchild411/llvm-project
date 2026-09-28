/*===---- pthread.h - IRIX wrapper ------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's pthread_cond_timedwait waits until an absolute CLOCK_REALTIME time,
 * and it has no pthread_condattr_setclock to make that CLOCK_MONOTONIC. Code
 * that needs timeouts that do not depend on the date (GLib's GCond) can use
 * the relative form macOS offers instead, pthread_cond_timedwait_relative_np,
 * which compiler-rt's IRIX builtins define (irix/pthread_np.c).
 */

#ifndef __CLANG_IRIX_PTHREAD_H
#define __CLANG_IRIX_PTHREAD_H

#include_next <pthread.h>

struct timespec;
#ifdef __cplusplus
extern "C" {
#endif
int pthread_cond_timedwait_relative_np(pthread_cond_t *, pthread_mutex_t *,
                                       const struct timespec *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_PTHREAD_H */
