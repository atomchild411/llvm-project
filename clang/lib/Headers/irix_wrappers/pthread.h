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
 *
 * IRIX's pthreads also lack pthread_mutex_timedlock and the spin locks;
 * compiler-rt defines them (irix/pthread_timedlock.c, irix/pthread_spin.c),
 * a spin lock being a mutex.
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
int pthread_mutex_timedlock(pthread_mutex_t *, const struct timespec *);
typedef pthread_mutex_t pthread_spinlock_t;
int pthread_spin_init(pthread_spinlock_t *, int);
int pthread_spin_destroy(pthread_spinlock_t *);
int pthread_spin_lock(pthread_spinlock_t *);
int pthread_spin_trylock(pthread_spinlock_t *);
int pthread_spin_unlock(pthread_spinlock_t *);
#ifdef __cplusplus
}
#endif

/* Thread names, in Linux's (and glibc's) form, which IRIX's pthreads do not
 * have: compiler-rt's irix/pthread_np.c checks the length (15 characters and
 * the NUL, ERANGE beyond) and keeps no name, so pthread_getname_np gives an
 * empty one. */
#ifdef __cplusplus
extern "C" {
#endif
int pthread_setname_np(pthread_t, const char *);
int pthread_getname_np(pthread_t, char *, size_t);
#ifdef __cplusplus
}
#endif

/* pthread_exit does not return (POSIX), which IRIX's declaration does not
 * say: a thread function that ends with it then "does not return a value"
 * (libssh, where that is an error). */
#ifdef __cplusplus
extern "C"
#endif
void pthread_exit(void *) __attribute__((__noreturn__));

/* In IRIX's libc, and declared by no IRIX header a program would look in
 * for it: declared here, with IRIX's own prototypes. */
#ifdef __cplusplus
extern "C" {
#endif
int pthread_atfork(void (*)(void), void (*)(void), void (*)(void));
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_PTHREAD_H */
