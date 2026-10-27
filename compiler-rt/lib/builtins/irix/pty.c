//===-- irix/pty.c - openpty, forkpty and login_tty for IRIX --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The BSDs' and glibc's pseudo-terminal functions (libutil there), which
// IRIX lacks; clang's IRIX <pty.h> and <util.h> declare them, and the driver
// drops -lutil from links. IRIX's own way to a pty is _getpty(), which
// opens a master and returns its slave's name (and, given nofork 0, has
// mkpts give the slave to the caller).
//
// posix_openpt (POSIX), which IRIX lacks too, is _getpty() for the master
// alone: grantpt, unlockpt and ptsname, which IRIX's libc has, then work on
// it as on any SVR4 system.
//
// login_tty: IRIX has no TIOCSCTTY. As on any SVR4 system, a session leader
// gets its controlling terminal by opening one without O_NOCTTY, so
// login_tty opens the terminal by name after setsid().
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

extern char *_getpty(int *, int, mode_t, int);

int openpty(int *amaster, int *aslave, char *name,
            const struct termios *termp, const struct winsize *winp) {
  int master, slave, e;
  char *slavename = _getpty(&master, O_RDWR | O_NOCTTY, 0600, 0);
  if (slavename == NULL)
    return -1;
  slave = open(slavename, O_RDWR | O_NOCTTY);
  if (slave < 0) {
    e = errno;
    close(master);
    __irix_seterrno(e);
    return -1;
  }
  if (name != NULL)
    strcpy(name, slavename);
  if (termp != NULL)
    tcsetattr(slave, TCSAFLUSH, termp);
  if (winp != NULL)
    ioctl(slave, TIOCSWINSZ, winp);
  *amaster = master;
  *aslave = slave;
  return 0;
}

#pragma weak posix_openpt

int posix_openpt(int flags) {
  int master;
  if (_getpty(&master, flags & (O_RDWR | O_NOCTTY), 0600, 0) == NULL)
    return -1;
  return master;
}

int login_tty(int fd) {
  char *name;
  int t;
  setsid();
  name = ttyname(fd);
  if (name != NULL && (t = open(name, O_RDWR)) >= 0)
    close(t);
  if (dup2(fd, 0) < 0 || dup2(fd, 1) < 0 || dup2(fd, 2) < 0)
    return -1;
  if (fd > 2)
    close(fd);
  return 0;
}

pid_t forkpty(int *amaster, char *name, const struct termios *termp,
              const struct winsize *winp) {
  int master, slave, e;
  pid_t pid;
  if (openpty(&master, &slave, name, termp, winp) == -1)
    return -1;
  pid = fork();
  if (pid == -1) {
    e = errno;
    close(master);
    close(slave);
    __irix_seterrno(e);
    return -1;
  }
  if (pid == 0) {
    close(master);
    if (login_tty(slave) == -1)
      _exit(1);
    return 0;
  }
  close(slave);
  *amaster = master;
  return pid;
}

#endif // defined(__sgi)
