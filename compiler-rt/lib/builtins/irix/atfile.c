//===-- irix/atfile.c - POSIX 2008's *at() functions on IRIX --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// openat, fstatat, unlinkat and the rest of POSIX 2008's directory-relative
// calls, and fdopendir, none of which any IRIX 6.5 libc has; clang's IRIX
// wrappers declare them, with the AT_ constants (<fcntl.h>).
//
// A path relative to a directory descriptor is made absolute and given to
// IRIX's own call, so they are not atomic as the real ones are. The
// directory's path comes from a table of descriptor number to path, filled
// when a directory is opened here or when its path is first worked out. An
// entry is trusted only while fstat(fd) and stat(path) name the same device
// and inode, so a descriptor that was closed and reused, or a directory that
// was renamed, is noticed and the entry dropped. With a valid entry nothing
// changes the process's working directory. (The path can still go stale
// between that check and the call, if the directory is renamed in that
// instant; a real openat cannot.)
//
// A descriptor with no valid entry (a directory opened by something else:
// opendir, a descriptor inherited or received, or one renamed since) has its
// path read the one way IRIX allows: fchdir to it, getcwd, fchdir back. That
// is the remaining window. IRIX's pthreads share the working directory and
// getcwd walks ".." with open and getdents, so for that long the whole
// process is in the other directory, and a thread outside this file that
// resolves a relative path then sees it. The calls in this file are
// serialized with each other by a lock, so two threads in them cannot see
// each other's directory; a thread outside this file is not stopped. The
// result is cached, so the window opens once per descriptor, not per call.
// An absolute path, or AT_FDCWD, goes straight to IRIX's call.
//
// What IRIX cannot do is refused: AT_SYMLINK_NOFOLLOW for fchmodat and
// utimensat on a symbolic link (no lchmod, no lutimes) fails with
// EOPNOTSUPP, as POSIX allows. There is no futimens: IRIX has no way to set
// a descriptor's times.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <dirent.h>
#include <errno.h>
#include "irix_errno.h"
#include <fcntl.h>
#include <limits.h>
#include <sched.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifndef AT_FDCWD
#error "clang's IRIX <fcntl.h> wrapper defines the AT_ constants"
#endif

// IRIX's own open, under its own name: open() here is the wrapper's, which
// adds O_CLOEXEC and the like (irix/cloexec.c) and is what openat wants.
extern int __irix_libc_open(const char *, int, ...) __asm__("open");

// Held while the working directory is changed, so that two of these calls
// never run their fchdir/getcwd/fchdir at once: each would read the other's
// directory (or the restored one) and hand back the wrong path.
static volatile int at_lock_word;

static void at_lock(void) {
  while (__sync_lock_test_and_set(&at_lock_word, 1))
    sched_yield();
}

static void at_unlock(void) { __sync_lock_release(&at_lock_word); }

// What is known of directory descriptors: the absolute path each one was
// opened as, with the device and inode it must still name. Indexed by
// descriptor number (those beyond the table take the slow way), guarded by
// at_lock. A descriptor closed behind our back leaves a stale entry; the
// identity check on use is what makes that harmless.
#define AT_TABLE 4096
static struct at_entry {
  char *path;
  dev_t dev;
  ino_t ino;
} at_table[AT_TABLE];

// Remember that FD is the directory named by the absolute path P, if that
// path really names what FD is.
static void at_remember(int fd, const char *p) {
  struct stat a, b;
  char *copy, *old;
  if (fd < 0 || fd >= AT_TABLE || p[0] != '/' || strlen(p) >= PATH_MAX)
    return;
  if (fstat(fd, &a) < 0 || !S_ISDIR(a.st_mode) || stat(p, &b) < 0 ||
      a.st_dev != b.st_dev || a.st_ino != b.st_ino)
    return;
  copy = strdup(p);
  if (copy == 0)
    return;
  at_lock();
  old = at_table[fd].path;
  at_table[fd].path = copy;
  at_table[fd].dev = a.st_dev;
  at_table[fd].ino = a.st_ino;
  at_unlock();
  free(old);
}

// Copy the path known for FD into BUF if it is still right: two stats and no
// change of directory. An entry that fails the check is dropped.
static int at_lookup(int fd, char *buf) {
  struct stat a, b;
  char *drop = 0;
  int ok = 0;
  if (fd < 0 || fd >= AT_TABLE || fstat(fd, &a) < 0)
    return 0;
  at_lock();
  if (at_table[fd].path) {
    if (at_table[fd].dev == a.st_dev && at_table[fd].ino == a.st_ino) {
      strcpy(buf, at_table[fd].path); // stored only when it fits PATH_MAX
      ok = 1;
    } else { // the number now names another directory
      drop = at_table[fd].path;
      at_table[fd].path = 0;
    }
  }
  at_unlock();
  free(drop);
  if (!ok)
    return 0;
  if (stat(buf, &b) == 0 && b.st_dev == a.st_dev && b.st_ino == a.st_ino)
    return 1;
  // renamed or removed since; forget it unless it was replaced meanwhile
  at_lock();
  if (at_table[fd].path && strcmp(at_table[fd].path, buf) == 0) {
    drop = at_table[fd].path;
    at_table[fd].path = 0;
  }
  at_unlock();
  free(drop);
  return 0;
}

// FD's directory path into BUF the slow way: with the lock held, fchdir to
// it, getcwd, fchdir back. For the length of the getcwd every thread of the
// process is in that directory.
static int at_getcwd_path(int fd, char *buf) {
  int cwd, e;
  at_lock();
  cwd = __irix_libc_open(".", O_RDONLY);
  if (cwd < 0) {
    e = errno;
    at_unlock();
    __irix_seterrno(e);
    return 0;
  }
  if (fchdir(fd) < 0) {
    e = errno;
    close(cwd);
    at_unlock();
    __irix_seterrno(e);
    return 0;
  }
  if (getcwd(buf, PATH_MAX) == 0) {
    e = errno;
    fchdir(cwd);
    close(cwd);
    at_unlock();
    __irix_seterrno(e);
    return 0;
  }
  fchdir(cwd);
  close(cwd);
  at_unlock();
  return 1;
}

// PATH resolved against the directory FD into BUF (PATH_MAX bytes), or PATH
// itself when it is absolute or FD is AT_FDCWD. Returns 0 with errno set if
// the directory cannot be entered or the result does not fit.
static const char *at_path(int fd, const char *path, char *buf) {
  size_t n;
  if (path == 0) {
    __irix_seterrno(EFAULT);
    return 0;
  }
  if (fd == AT_FDCWD || path[0] == '/')
    return path;
  if (path[0] == '\0') {
    __irix_seterrno(ENOENT);
    return 0;
  }
  if (!at_lookup(fd, buf)) {
    if (!at_getcwd_path(fd, buf))
      return 0;
    at_remember(fd, buf);
  }
  n = strlen(buf);
  if (n + 1 + strlen(path) + 1 > PATH_MAX) {
    __irix_seterrno(ENAMETOOLONG);
    return 0;
  }
  if (n == 0 || buf[n - 1] != '/')
    buf[n++] = '/';
  strcpy(buf + n, path);
  return buf;
}

// FD was just opened as P (what openat was given, or made of it). If it is a
// directory, remember its path for the calls that will be relative to it. A
// path relative to the working directory is made absolute first; at_remember
// checks the result against the descriptor, so a chdir racing with this only
// costs the entry.
static void at_opened(int fd, int flags, const char *p) {
  char buf[PATH_MAX];
  size_t n;
  struct stat st;
  if (fd < 0 || fd >= AT_TABLE || (flags & O_ACCMODE) != O_RDONLY)
    return;
  if (fstat(fd, &st) < 0 || !S_ISDIR(st.st_mode))
    return;
  if (p[0] == '/') {
    at_remember(fd, p);
    return;
  }
  if (getcwd(buf, PATH_MAX) == 0)
    return;
  n = strlen(buf);
  if (n + 1 + strlen(p) + 1 > PATH_MAX)
    return;
  if (n == 0 || buf[n - 1] != '/')
    buf[n++] = '/';
  strcpy(buf + n, p);
  at_remember(fd, buf);
}

#define AT(fd, path)                                                           \
  char at_buf_[PATH_MAX];                                                      \
  const char *at_p_ = at_path(fd, path, at_buf_);                              \
  if (at_p_ == 0)                                                              \
    return -1

#pragma weak openat
int openat(int fd, const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
  int r;
  AT(fd, path);
  r = open(at_p_, flags, mode);
  if (r >= 0)
    at_opened(r, flags, at_p_);
  return r;
}

#pragma weak fstatat
int fstatat(int fd, const char *path, struct stat *st, int flag) {
  AT(fd, path);
  return flag & AT_SYMLINK_NOFOLLOW ? lstat(at_p_, st) : stat(at_p_, st);
}

// The large-file forms, glibc's, on IRIX's open64 and stat64.
#pragma weak openat64
int openat64(int fd, const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
  int r;
  AT(fd, path);
  r = open64(at_p_, flags, mode);
  if (r >= 0)
    at_opened(r, flags, at_p_);
  return r;
}

#pragma weak fstatat64
int fstatat64(int fd, const char *path, struct stat64 *st, int flag) {
  AT(fd, path);
  return flag & AT_SYMLINK_NOFOLLOW ? lstat64(at_p_, st) : stat64(at_p_, st);
}

#pragma weak fchmodat
int fchmodat(int fd, const char *path, mode_t mode, int flag) {
  AT(fd, path);
  if (flag & AT_SYMLINK_NOFOLLOW) {
    struct stat st;
    if (lstat(at_p_, &st) < 0)
      return -1;
    if (S_ISLNK(st.st_mode)) {
      __irix_seterrno(EOPNOTSUPP);
      return -1;
    }
  }
  return chmod(at_p_, mode);
}

#pragma weak fchownat
int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flag) {
  AT(fd, path);
  return flag & AT_SYMLINK_NOFOLLOW ? lchown(at_p_, uid, gid)
                                    : chown(at_p_, uid, gid);
}

#pragma weak mkdirat
int mkdirat(int fd, const char *path, mode_t mode) {
  AT(fd, path);
  return mkdir(at_p_, mode);
}

#pragma weak mknodat
int mknodat(int fd, const char *path, mode_t mode, dev_t dev) {
  AT(fd, path);
  return mknod(at_p_, mode, dev);
}

#pragma weak mkfifoat
int mkfifoat(int fd, const char *path, mode_t mode) {
  AT(fd, path);
  return mkfifo(at_p_, mode);
}

#pragma weak unlinkat
int unlinkat(int fd, const char *path, int flag) {
  AT(fd, path);
  return flag & AT_REMOVEDIR ? rmdir(at_p_) : unlink(at_p_);
}

#pragma weak readlinkat
ssize_t readlinkat(int fd, const char *path, char *buf, size_t size) {
  AT(fd, path);
  return readlink(at_p_, buf, size);
}

#pragma weak symlinkat
int symlinkat(const char *target, int fd, const char *path) {
  AT(fd, path);
  return symlink(target, at_p_);
}

#pragma weak renameat
int renameat(int fd1, const char *old, int fd2, const char *new_) {
  char buf2[PATH_MAX];
  const char *p2;
  AT(fd1, old);
  p2 = at_path(fd2, new_, buf2);
  if (p2 == 0)
    return -1;
  return rename(at_p_, p2);
}

// IRIX's link() makes the new name for the file the old one names, whatever
// it is, so AT_SYMLINK_FOLLOW is not looked at.
#pragma weak linkat
int linkat(int fd1, const char *old, int fd2, const char *new_, int flag) {
  char buf2[PATH_MAX];
  const char *p2;
  (void)flag;
  AT(fd1, old);
  p2 = at_path(fd2, new_, buf2);
  if (p2 == 0)
    return -1;
  return link(at_p_, p2);
}

// With AT_EACCESS the check is against the effective ids, by the file's
// mode bits (IRIX has no eaccess); otherwise access(), which uses the real
// ones.
#pragma weak faccessat
int faccessat(int fd, const char *path, int mode, int flag) {
  struct stat st;
  uid_t uid;
  AT(fd, path);
  if (!(flag & AT_EACCESS) ||
      (geteuid() == getuid() && getegid() == getgid()))
    return access(at_p_, mode);
  if (stat(at_p_, &st) < 0)
    return -1;
  if (mode == F_OK)
    return 0;
  uid = geteuid();
  if (uid == 0) {
    // root may read and write anything, and execute what anyone may.
    if (!(mode & X_OK) || S_ISDIR(st.st_mode) ||
        (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
      return 0;
  } else {
    int shift = 0, i, n;
    gid_t groups[NGROUPS_MAX];
    if (st.st_uid == uid)
      shift = 6;
    else if (st.st_gid == getegid())
      shift = 3;
    else {
      n = getgroups(NGROUPS_MAX, groups);
      for (i = 0; i < n; ++i)
        if (groups[i] == st.st_gid)
          shift = 3;
    }
    if ((((st.st_mode >> shift) & 7) & mode) == mode)
      return 0;
  }
  __irix_seterrno(EACCES);
  return -1;
}

// UTIME_NOW and UTIME_OMIT need the current times; utimes takes
// microseconds.
#pragma weak utimensat
int utimensat(int fd, const char *path, const struct timespec ts[2],
              int flag) {
  struct timeval tv[2];
  struct stat st;
  int i;
  AT(fd, path);
  if (flag & AT_SYMLINK_NOFOLLOW) {
    if (lstat(at_p_, &st) < 0)
      return -1;
    if (S_ISLNK(st.st_mode)) {
      __irix_seterrno(EOPNOTSUPP);
      return -1;
    }
  }
  if (ts == 0)
    return utimes(at_p_, 0);
  if (ts[0].tv_nsec == UTIME_OMIT && ts[1].tv_nsec == UTIME_OMIT)
    return 0;
  if (ts[0].tv_nsec == UTIME_OMIT || ts[1].tv_nsec == UTIME_OMIT)
    if (stat(at_p_, &st) < 0)
      return -1;
  for (i = 0; i != 2; ++i) {
    if (ts[i].tv_nsec == UTIME_NOW) {
      gettimeofday(&tv[i], 0);
    } else if (ts[i].tv_nsec == UTIME_OMIT) {
      tv[i].tv_sec = i == 0 ? st.st_atime : st.st_mtime;
      tv[i].tv_usec = 0;
    } else if (ts[i].tv_nsec < 0 || ts[i].tv_nsec >= 1000000000L) {
      __irix_seterrno(EINVAL);
      return -1;
    } else {
      tv[i].tv_sec = ts[i].tv_sec;
      tv[i].tv_usec = ts[i].tv_nsec / 1000;
    }
  }
  return utimes(at_p_, tv);
}

// A DIR for the directory FD, which then belongs to it (closedir closes
// it). IRIX's opendir is given a directory that always exists to make the
// DIR and its buffer, and the DIR is pointed at FD instead: no path, no
// change of directory, and FD keeps its number. (An earlier version opened
// FD's directory again by path and dup2'd the new descriptor onto FD. IRIX's
// dup2 is close followed by fcntl(F_DUPFD) -- not atomic -- and the threads
// of a process share one descriptor table, so another thread's open could
// take FD's number in between; the DIR then closed that thread's file.)
#pragma weak fdopendir
DIR *fdopendir(int fd) {
  struct stat st;
  DIR *d;
  if (fstat(fd, &st) < 0)
    return 0;
  if (!S_ISDIR(st.st_mode)) {
    __irix_seterrno(ENOTDIR);
    return 0;
  }
  d = opendir("/");
  if (d == 0)
    return 0;
  // the spare descriptor opendir made is ours alone until it is closed
  close(d->dd_fd);
  d->dd_fd = fd;
  d->dd_loc = 0;
  d->dd_size = 0;
  d->__dd_flags &= ~_DIR_FLAGS_SEEN_EOF;
  return d;
}

#endif // __sgi
