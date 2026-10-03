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
// A path relative to a directory descriptor is made absolute -- fchdir to
// the directory, getcwd, fchdir back -- and given to IRIX's own call. So
// they are not atomic as the real ones are: the directory's path is read
// at the call, and the process's working directory changes for a moment,
// which another thread resolving a relative path at that moment would see.
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

// PATH resolved against the directory FD into BUF (PATH_MAX bytes), or PATH
// itself when it is absolute or FD is AT_FDCWD. Returns 0 with errno set if
// the directory cannot be entered or the result does not fit.
static const char *at_path(int fd, const char *path, char *buf) {
  int cwd, e;
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
  cwd = __irix_libc_open(".", O_RDONLY);
  if (cwd < 0)
    return 0;
  if (fchdir(fd) < 0) {
    e = errno;
    close(cwd);
    __irix_seterrno(e);
    return 0;
  }
  if (getcwd(buf, PATH_MAX) == 0) {
    e = errno;
    fchdir(cwd);
    close(cwd);
    __irix_seterrno(e);
    return 0;
  }
  e = errno;
  fchdir(cwd);
  close(cwd);
  __irix_seterrno(e);
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

#define AT(fd, path)                                                           \
  char at_buf_[PATH_MAX];                                                      \
  const char *at_p_ = at_path(fd, path, at_buf_);                              \
  if (at_p_ == 0)                                                              \
    return -1

int openat(int fd, const char *path, int flags, ...) {
  mode_t mode = 0;
  if (flags & O_CREAT) {
    va_list ap;
    va_start(ap, flags);
    mode = (mode_t)va_arg(ap, int);
    va_end(ap);
  }
  AT(fd, path);
  return open(at_p_, flags, mode);
}

int fstatat(int fd, const char *path, struct stat *st, int flag) {
  AT(fd, path);
  return flag & AT_SYMLINK_NOFOLLOW ? lstat(at_p_, st) : stat(at_p_, st);
}

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

int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flag) {
  AT(fd, path);
  return flag & AT_SYMLINK_NOFOLLOW ? lchown(at_p_, uid, gid)
                                    : chown(at_p_, uid, gid);
}

int mkdirat(int fd, const char *path, mode_t mode) {
  AT(fd, path);
  return mkdir(at_p_, mode);
}

int mknodat(int fd, const char *path, mode_t mode, dev_t dev) {
  AT(fd, path);
  return mknod(at_p_, mode, dev);
}

int mkfifoat(int fd, const char *path, mode_t mode) {
  AT(fd, path);
  return mkfifo(at_p_, mode);
}

int unlinkat(int fd, const char *path, int flag) {
  AT(fd, path);
  return flag & AT_REMOVEDIR ? rmdir(at_p_) : unlink(at_p_);
}

ssize_t readlinkat(int fd, const char *path, char *buf, size_t size) {
  AT(fd, path);
  return readlink(at_p_, buf, size);
}

int symlinkat(const char *target, int fd, const char *path) {
  AT(fd, path);
  return symlink(target, at_p_);
}

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
// it): the directory is opened again by path and its descriptor moved onto
// FD's number.
DIR *fdopendir(int fd) {
  char buf[PATH_MAX];
  struct stat st;
  const char *p;
  DIR *d;
  int dfd, e, fdflags;
  if (fstat(fd, &st) < 0)
    return 0;
  fdflags = fcntl(fd, F_GETFD);
  if (!S_ISDIR(st.st_mode)) {
    __irix_seterrno(ENOTDIR);
    return 0;
  }
  p = at_path(fd, ".", buf);
  if (p == 0)
    return 0;
  d = opendir(p);
  if (d == 0)
    return 0;
  dfd = dirfd(d);
  if (dfd != fd) {
    if (dup2(dfd, fd) < 0) {
      e = errno;
      closedir(d);
      __irix_seterrno(e);
      return 0;
    }
    close(dfd);
    d->dd_fd = fd;
    if (fdflags >= 0)
      fcntl(fd, F_SETFD, fdflags); // dup2 cleared close-on-exec
  }
  return d;
}

#endif // __sgi
