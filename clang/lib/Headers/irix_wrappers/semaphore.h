/*===---- semaphore.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * POSIX's sem_timedwait, which IRIX's libc lacks: compiler-rt's IRIX
 * builtins (irix/semaphore.c) have it.
 */

#ifndef __CLANG_IRIX_SEMAPHORE_H
#define __CLANG_IRIX_SEMAPHORE_H

#include_next <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif
struct timespec;
int sem_timedwait(sem_t *__restrict, const struct timespec *__restrict);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_SEMAPHORE_H */
