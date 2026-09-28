/*===---- spawn.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * posix_spawn, which IRIX lacks: it has no <spawn.h>. compiler-rt's IRIX
 * builtins (irix/spawn.c) implement it with fork and execve, reporting a
 * failed exec to the caller as posix_spawn's result, as POSIX allows.
 */

#ifndef __CLANG_IRIX_SPAWN_H
#define __CLANG_IRIX_SPAWN_H

#include <sched.h>
#include <signal.h>
#include <sys/types.h>

#define POSIX_SPAWN_RESETIDS 0x01
#define POSIX_SPAWN_SETPGROUP 0x02
#define POSIX_SPAWN_SETSIGDEF 0x04
#define POSIX_SPAWN_SETSIGMASK 0x08
#define POSIX_SPAWN_SETSCHEDPARAM 0x10
#define POSIX_SPAWN_SETSCHEDULER 0x20

typedef struct {
  short __flags;
  pid_t __pgroup;
  sigset_t __sigdefault;
  sigset_t __sigmask;
  int __policy;
  struct sched_param __param;
} posix_spawnattr_t;

struct __irix_spawn_action;
typedef struct {
  int __used;
  int __allocated;
  struct __irix_spawn_action *__actions;
} posix_spawn_file_actions_t;

#ifdef __cplusplus
extern "C" {
#endif

int posix_spawn(pid_t *__restrict, const char *__restrict,
                const posix_spawn_file_actions_t *,
                const posix_spawnattr_t *__restrict, char *const[],
                char *const[]);
int posix_spawnp(pid_t *__restrict, const char *__restrict,
                 const posix_spawn_file_actions_t *,
                 const posix_spawnattr_t *__restrict, char *const[],
                 char *const[]);

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *__restrict,
                                     int, const char *__restrict, int, mode_t);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *, int);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *, int, int);
int posix_spawn_file_actions_addchdir(posix_spawn_file_actions_t *__restrict,
                                      const char *__restrict);
int posix_spawn_file_actions_addfchdir(posix_spawn_file_actions_t *, int);

int posix_spawnattr_init(posix_spawnattr_t *);
int posix_spawnattr_destroy(posix_spawnattr_t *);
int posix_spawnattr_getflags(const posix_spawnattr_t *__restrict,
                             short *__restrict);
int posix_spawnattr_setflags(posix_spawnattr_t *, short);
int posix_spawnattr_getpgroup(const posix_spawnattr_t *__restrict,
                              pid_t *__restrict);
int posix_spawnattr_setpgroup(posix_spawnattr_t *, pid_t);
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *__restrict,
                                  sigset_t *__restrict);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *__restrict,
                                  const sigset_t *__restrict);
int posix_spawnattr_getsigmask(const posix_spawnattr_t *__restrict,
                               sigset_t *__restrict);
int posix_spawnattr_setsigmask(posix_spawnattr_t *__restrict,
                               const sigset_t *__restrict);
int posix_spawnattr_getschedparam(const posix_spawnattr_t *__restrict,
                                  struct sched_param *__restrict);
int posix_spawnattr_setschedparam(posix_spawnattr_t *__restrict,
                                  const struct sched_param *__restrict);
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *__restrict,
                                   int *__restrict);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *, int);

#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_SPAWN_H */
