//===-- irix/pthread_spin.c - POSIX spin locks for IRIX -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX's pthread_spin_* functions, which IRIX's pthreads lack (libiscsi,
// vifm). clang's IRIX <pthread.h> makes pthread_spinlock_t a mutex and
// declares these; they are the mutex operations, so a "spin" lock blocks
// instead of spinning, which POSIX allows. pshared is ignored, as IRIX's
// mutexes are process-private.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <pthread.h>

// pthread_spinlock_t is pthread_mutex_t (clang's <pthread.h>); spelled out
// here, as the builtins are built with the previous compiler's headers.

#pragma weak pthread_spin_init
#pragma weak pthread_spin_destroy
#pragma weak pthread_spin_lock
#pragma weak pthread_spin_trylock
#pragma weak pthread_spin_unlock

int pthread_spin_init(pthread_mutex_t *lock, int pshared) {
  (void)pshared;
  return pthread_mutex_init(lock, NULL);
}

int pthread_spin_destroy(pthread_mutex_t *lock) {
  return pthread_mutex_destroy(lock);
}

int pthread_spin_lock(pthread_mutex_t *lock) {
  return pthread_mutex_lock(lock);
}

int pthread_spin_trylock(pthread_mutex_t *lock) {
  return pthread_mutex_trylock(lock);
}

int pthread_spin_unlock(pthread_mutex_t *lock) {
  return pthread_mutex_unlock(lock);
}

#endif // defined(__sgi)
