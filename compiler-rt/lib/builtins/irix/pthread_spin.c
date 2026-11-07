//===-- irix/pthread_spin.c - POSIX spin locks for IRIX -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX's pthread_spin_* functions, for IRIX without spin locks (6.5.22;
// 6.5.30's libpthread has them). There, clang's IRIX <pthread.h> makes
// pthread_spinlock_t a mutex and maps pthread_spin_* to these names, so a
// program built against 6.5.30's headers never links them in place of
// libpthread's (whose pthread_spinlock_t is half a mutex's size). They are
// the mutex operations, so a "spin" lock blocks instead of spinning, which
// POSIX allows. pshared is ignored, as IRIX's mutexes are process-private.
//
// Weak, so a program's own definitions win (pthread_spin_* that a program
// defines itself get these names too, from <pthread.h>). Its own file, so a
// program is only given it when it asks for it. Programs built before the
// names changed carry their own copies (static builtins).
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <pthread.h>

// pthread_spinlock_t is pthread_mutex_t (clang's <pthread.h>); spelled out
// here, as the builtins are built with the previous compiler's headers.

#pragma weak __irix_pthread_spin_init
#pragma weak __irix_pthread_spin_destroy
#pragma weak __irix_pthread_spin_lock
#pragma weak __irix_pthread_spin_trylock
#pragma weak __irix_pthread_spin_unlock

int __irix_pthread_spin_init(pthread_mutex_t *lock, int pshared) {
  (void)pshared;
  return pthread_mutex_init(lock, NULL);
}

int __irix_pthread_spin_destroy(pthread_mutex_t *lock) {
  return pthread_mutex_destroy(lock);
}

int __irix_pthread_spin_lock(pthread_mutex_t *lock) {
  return pthread_mutex_lock(lock);
}

int __irix_pthread_spin_trylock(pthread_mutex_t *lock) {
  return pthread_mutex_trylock(lock);
}

int __irix_pthread_spin_unlock(pthread_mutex_t *lock) {
  return pthread_mutex_unlock(lock);
}

#endif // defined(__sgi)
