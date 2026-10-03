//===-- irix/irix_errno.h - errno for the IRIX stand-ins ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX's libc keeps errno twice in a program that uses threads: the global
// int, which is errno in code built without _SGI_MP_SOURCE, and the
// thread's own, *__oserror(), which <pthread.h> makes errno. libc sets
// both, and so must the stand-ins, or a threaded program reads a stale
// errno after one fails. In a program without threads the two are one int.
// The global is named by its symbol: in a file that includes <pthread.h>,
// errno is the thread's.
//
//===----------------------------------------------------------------------===//

#ifndef IRIX_ERRNO_H
#define IRIX_ERRNO_H

#include <errno.h>

extern int *__oserror(void);
extern int __irix_global_errno __asm__("errno");

static inline void __irix_seterrno(int e) {
  __irix_global_errno = e;
  *__oserror() = e;
}

#endif // IRIX_ERRNO_H
