//===-- irix/spawn.c - posix_spawn for IRIX -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// posix_spawn and posix_spawnp, their file actions and attributes, which
// IRIX lacks; clang's IRIX <spawn.h> declares them.
//
// The child is made with fork. It blocks every signal around the fork, so
// no handler of the parent's runs in the child, and resets caught signals
// to their default before restoring the mask. It then applies the
// attributes and file actions in POSIX's order and execs. A failure there
// is written to a close-on-exec pipe: the parent reads it, reaps the child
// and returns the error, so a program that cannot be run is reported by
// posix_spawn itself, as it is on Linux and the BSDs.
//
// posix_spawnp searches PATH itself (IRIX has no execvpe), with IRIX's
// default path when PATH is unset, and runs a file that is not an
// executable format (ENOEXEC) with /bin/sh, as execvp does.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <spawn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Weak: a program that brings its own copy (a compat/ directory) keeps it,
// while still getting the rest of this file.
#pragma weak posix_spawn_file_actions_init
#pragma weak posix_spawn_file_actions_destroy
#pragma weak posix_spawn_file_actions_addopen
#pragma weak posix_spawn_file_actions_addclose
#pragma weak posix_spawn_file_actions_adddup2
#pragma weak posix_spawn_file_actions_addchdir
#pragma weak posix_spawn_file_actions_addfchdir
#pragma weak posix_spawnattr_init
#pragma weak posix_spawnattr_destroy
#pragma weak posix_spawnattr_getflags
#pragma weak posix_spawnattr_setflags
#pragma weak posix_spawnattr_getpgroup
#pragma weak posix_spawnattr_setpgroup
#pragma weak posix_spawnattr_getsigdefault
#pragma weak posix_spawnattr_setsigdefault
#pragma weak posix_spawnattr_getsigmask
#pragma weak posix_spawnattr_setsigmask
#pragma weak posix_spawnattr_getschedparam
#pragma weak posix_spawnattr_setschedparam
#pragma weak posix_spawnattr_getschedpolicy
#pragma weak posix_spawnattr_setschedpolicy
#pragma weak posix_spawn
#pragma weak posix_spawnp


extern char **environ;

enum { ACTION_OPEN, ACTION_CLOSE, ACTION_DUP2, ACTION_CHDIR, ACTION_FCHDIR };

struct __irix_spawn_action {
  int kind;
  int fd;
  int newfd;
  int oflag;
  mode_t mode;
  char *path;
};

// --- file actions ----------------------------------------------------------

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *fa) {
  fa->__used = 0;
  fa->__allocated = 0;
  fa->__actions = 0;
  return 0;
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *fa) {
  int i;
  for (i = 0; i < fa->__used; i++)
    free(fa->__actions[i].path);
  free(fa->__actions);
  fa->__used = fa->__allocated = 0;
  fa->__actions = 0;
  return 0;
}

static struct __irix_spawn_action *add_action(posix_spawn_file_actions_t *fa) {
  if (fa->__used == fa->__allocated) {
    int n = fa->__allocated ? 2 * fa->__allocated : 8;
    struct __irix_spawn_action *a = (struct __irix_spawn_action *)realloc(
        fa->__actions, n * sizeof(*a));
    if (!a)
      return 0;
    fa->__actions = a;
    fa->__allocated = n;
  }
  memset(&fa->__actions[fa->__used], 0, sizeof(fa->__actions[0]));
  return &fa->__actions[fa->__used++];
}

static int bad_fd(int fd) { return fd < 0 || fd >= sysconf(_SC_OPEN_MAX); }

int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *fa, int fd,
                                     const char *path, int oflag, mode_t mode) {
  struct __irix_spawn_action *a;
  char *p;
  if (bad_fd(fd))
    return EBADF;
  if (!(p = strdup(path)))
    return ENOMEM;
  if (!(a = add_action(fa))) {
    free(p);
    return ENOMEM;
  }
  a->kind = ACTION_OPEN;
  a->fd = fd;
  a->path = p;
  a->oflag = oflag;
  a->mode = mode;
  return 0;
}

int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *fa, int fd) {
  struct __irix_spawn_action *a;
  if (bad_fd(fd))
    return EBADF;
  if (!(a = add_action(fa)))
    return ENOMEM;
  a->kind = ACTION_CLOSE;
  a->fd = fd;
  return 0;
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *fa, int fd,
                                     int newfd) {
  struct __irix_spawn_action *a;
  if (bad_fd(fd) || bad_fd(newfd))
    return EBADF;
  if (!(a = add_action(fa)))
    return ENOMEM;
  a->kind = ACTION_DUP2;
  a->fd = fd;
  a->newfd = newfd;
  return 0;
}

int posix_spawn_file_actions_addchdir(posix_spawn_file_actions_t *fa,
                                      const char *path) {
  struct __irix_spawn_action *a;
  char *p;
  if (!(p = strdup(path)))
    return ENOMEM;
  if (!(a = add_action(fa))) {
    free(p);
    return ENOMEM;
  }
  a->kind = ACTION_CHDIR;
  a->path = p;
  return 0;
}

int posix_spawn_file_actions_addfchdir(posix_spawn_file_actions_t *fa,
                                       int fd) {
  struct __irix_spawn_action *a;
  if (bad_fd(fd))
    return EBADF;
  if (!(a = add_action(fa)))
    return ENOMEM;
  a->kind = ACTION_FCHDIR;
  a->fd = fd;
  return 0;
}

// --- attributes --------------------------------------------------------------

int posix_spawnattr_init(posix_spawnattr_t *at) {
  memset(at, 0, sizeof(*at));
  sigemptyset(&at->__sigdefault);
  sigemptyset(&at->__sigmask);
  return 0;
}

int posix_spawnattr_destroy(posix_spawnattr_t *at) {
  (void)at;
  return 0;
}

int posix_spawnattr_getflags(const posix_spawnattr_t *at, short *flags) {
  *flags = at->__flags;
  return 0;
}

int posix_spawnattr_setflags(posix_spawnattr_t *at, short flags) {
  if (flags & ~(POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP |
                POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK |
                POSIX_SPAWN_SETSCHEDPARAM | POSIX_SPAWN_SETSCHEDULER))
    return EINVAL;
  at->__flags = flags;
  return 0;
}

int posix_spawnattr_getpgroup(const posix_spawnattr_t *at, pid_t *pgroup) {
  *pgroup = at->__pgroup;
  return 0;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *at, pid_t pgroup) {
  at->__pgroup = pgroup;
  return 0;
}

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *at, sigset_t *s) {
  *s = at->__sigdefault;
  return 0;
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *at, const sigset_t *s) {
  at->__sigdefault = *s;
  return 0;
}

int posix_spawnattr_getsigmask(const posix_spawnattr_t *at, sigset_t *s) {
  *s = at->__sigmask;
  return 0;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *at, const sigset_t *s) {
  at->__sigmask = *s;
  return 0;
}

int posix_spawnattr_getschedparam(const posix_spawnattr_t *at,
                                  struct sched_param *p) {
  *p = at->__param;
  return 0;
}

int posix_spawnattr_setschedparam(posix_spawnattr_t *at,
                                  const struct sched_param *p) {
  at->__param = *p;
  return 0;
}

int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *at, int *policy) {
  *policy = at->__policy;
  return 0;
}

int posix_spawnattr_setschedpolicy(posix_spawnattr_t *at, int policy) {
  at->__policy = policy;
  return 0;
}

// --- the child ---------------------------------------------------------------

// Report errno on the pipe and leave. Only async-signal-safe calls from here.
static void child_fail(int errpipe) {
  int e = errno;
  while (write(errpipe, &e, sizeof(e)) < 0 && errno == EINTR)
    ;
  _exit(127);
}

// Keep the error pipe clear of a descriptor an action is about to use.
static int move_errpipe(int errpipe, int fd) {
  int n;
  if (fd != errpipe)
    return errpipe;
  n = fcntl(errpipe, F_DUPFD, 0);
  if (n < 0)
    child_fail(errpipe);
  fcntl(n, F_SETFD, FD_CLOEXEC);
  close(errpipe);
  return n;
}

static int run_actions(const posix_spawn_file_actions_t *fa, int errpipe) {
  int i, fd;
  for (i = 0; fa && i < fa->__used; i++) {
    const struct __irix_spawn_action *a = &fa->__actions[i];
    switch (a->kind) {
    case ACTION_OPEN:
      errpipe = move_errpipe(errpipe, a->fd);
      fd = open(a->path, a->oflag, a->mode);
      if (fd < 0)
        child_fail(errpipe);
      if (fd != a->fd) {
        if (dup2(fd, a->fd) < 0)
          child_fail(errpipe);
        close(fd);
      }
      break;
    case ACTION_CLOSE:
      // POSIX lets closing an unopened descriptor succeed.
      if (a->fd == errpipe)
        errpipe = move_errpipe(errpipe, a->fd);
      else
        close(a->fd);
      break;
    case ACTION_DUP2:
      errpipe = move_errpipe(errpipe, a->newfd);
      if (a->fd == a->newfd) {
        // The descriptor is inherited: clear its close-on-exec flag.
        int f = fcntl(a->fd, F_GETFD);
        if (f < 0 || fcntl(a->fd, F_SETFD, f & ~FD_CLOEXEC) < 0)
          child_fail(errpipe);
      } else if (dup2(a->fd, a->newfd) < 0) {
        child_fail(errpipe);
      }
      break;
    case ACTION_CHDIR:
      if (chdir(a->path) < 0)
        child_fail(errpipe);
      break;
    case ACTION_FCHDIR:
      if (fchdir(a->fd) < 0)
        child_fail(errpipe);
      break;
    }
  }
  return errpipe;
}

static void exec_sh(const char *path, char *const argv[], char *const envp[],
                    int errpipe) {
  int n = 0;
  (void)errpipe;
  while (argv[n])
    n++;
  // On the stack: after a fork, another thread of the parent may have held
  // malloc's lock.
  char *args[n + 2];
  args[0] = (char *)"sh";
  args[1] = (char *)path;
  memcpy(&args[2], &argv[1], n * sizeof(char *)); // argv[1..n-1] and the null
  if (n == 0)
    args[2] = 0;
  execve("/bin/sh", args, envp);
}

static void exec_path(const char *file, char *const argv[],
                      char *const envp[], int errpipe) {
  const char *path, *p, *end;
  char buf[1024];
  int saw_eacces = 0;
  size_t flen = strlen(file);

  path = getenv("PATH");
  if (!path)
    path = "/usr/sbin:/usr/bsd:/sbin:/usr/bin:/bin";
  for (p = path;; p = end + 1) {
    size_t dlen;
    end = strchr(p, ':');
    if (!end)
      end = p + strlen(p);
    dlen = (size_t)(end - p);
    if (dlen + 1 + flen + 1 <= sizeof(buf)) {
      if (dlen) {
        memcpy(buf, p, dlen);
        buf[dlen++] = '/';
      }
      memcpy(buf + dlen, file, flen + 1);
      execve(buf, argv, envp);
      if (errno == ENOEXEC)
        exec_sh(buf, argv, envp, errpipe);
      if (errno == EACCES)
        saw_eacces = 1;
      else if (errno != ENOENT && errno != ENOTDIR)
        child_fail(errpipe);
    }
    if (!*end)
      break;
  }
  __irix_seterrno(saw_eacces ? EACCES : ENOENT);
}

static int spawn(pid_t *pidp, const char *file,
                 const posix_spawn_file_actions_t *fa,
                 const posix_spawnattr_t *at, char *const argv[],
                 char *const envp[], int search) {
  int pipefd[2], err, status, n;
  short flags = at ? at->__flags : 0;
  sigset_t all, old;
  pid_t pid;

  if (!envp)
    envp = environ;
  if (pipe(pipefd) < 0)
    return errno;
  fcntl(pipefd[0], F_SETFD, FD_CLOEXEC);
  fcntl(pipefd[1], F_SETFD, FD_CLOEXEC);

  sigfillset(&all);
  sigprocmask(SIG_BLOCK, &all, &old);
  pid = fork();
  if (pid == 0) {
    int errpipe = pipefd[1], sig;
    struct sigaction sa;
    close(pipefd[0]);

    // No handler of the parent's may run here: reset caught signals (and,
    // for POSIX_SPAWN_SETSIGDEF, the ones asked for) to their default.
    for (sig = 1; sig < NSIG; sig++) {
      if (sigaction(sig, 0, &sa) < 0)
        continue;
      if ((flags & POSIX_SPAWN_SETSIGDEF) &&
          sigismember(&at->__sigdefault, sig) == 1) {
        sa.sa_handler = SIG_DFL;
      } else if (sa.sa_handler == SIG_DFL || sa.sa_handler == SIG_IGN) {
        continue;
      } else {
        sa.sa_handler = SIG_DFL;
      }
      sa.sa_flags = 0;
      sigemptyset(&sa.sa_mask);
      sigaction(sig, &sa, 0);
    }

    if ((flags & POSIX_SPAWN_SETPGROUP) && setpgid(0, at->__pgroup) < 0)
      child_fail(errpipe);
    if ((flags & POSIX_SPAWN_SETSCHEDULER) &&
        sched_setscheduler(0, at->__policy, &at->__param) < 0)
      child_fail(errpipe);
    else if (!(flags & POSIX_SPAWN_SETSCHEDULER) &&
             (flags & POSIX_SPAWN_SETSCHEDPARAM) &&
             sched_setparam(0, &at->__param) < 0)
      child_fail(errpipe);
    if (flags & POSIX_SPAWN_RESETIDS) {
      if (setgid(getgid()) < 0 || setuid(getuid()) < 0)
        child_fail(errpipe);
    }
    errpipe = run_actions(fa, errpipe);
    sigprocmask(SIG_SETMASK, (flags & POSIX_SPAWN_SETSIGMASK) ? &at->__sigmask
                                                              : &old,
                0);
    if (search && !strchr(file, '/')) {
      exec_path(file, argv, envp, errpipe);
    } else {
      execve(file, argv, envp);
      if (search && errno == ENOEXEC)
        exec_sh(file, argv, envp, errpipe);
    }
    child_fail(errpipe);
  }
  err = pid < 0 ? errno : 0;
  sigprocmask(SIG_SETMASK, &old, 0);
  close(pipefd[1]);
  if (pid < 0) {
    close(pipefd[0]);
    return err;
  }

  // The pipe closes on a successful exec; otherwise it carries the error.
  do
    n = (int)read(pipefd[0], &err, sizeof(err));
  while (n < 0 && errno == EINTR);
  close(pipefd[0]);
  if (n == (int)sizeof(err)) {
    while (waitpid(pid, &status, 0) < 0 && errno == EINTR)
      ;
    return err;
  }
  if (pidp)
    *pidp = pid;
  return 0;
}

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *fa,
                const posix_spawnattr_t *at, char *const argv[],
                char *const envp[]) {
  return spawn(pid, path, fa, at, argv, envp, 0);
}

int posix_spawnp(pid_t *pid, const char *file,
                 const posix_spawn_file_actions_t *fa,
                 const posix_spawnattr_t *at, char *const argv[],
                 char *const envp[]) {
  return spawn(pid, file, fa, at, argv, envp, 1);
}

#endif // defined(__sgi)
