//===-- irix/process_bsd.c - BSD and C99 process calls IRIX lacks ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// daemon (<unistd.h>), _Exit (<stdlib.h>) and vfork (<unistd.h>), declared
// by clang's IRIX wrappers; packages in the IRIX pkgsrc bulk build failed to
// link without them. Weak, so a program that brings its own copy keeps it.
//
// daemon() is glibc's and the BSDs': fork, setsid, chdir("/") unless
// nochdir, stdin/stdout/stderr to /dev/null unless noclose, and no other
// descriptor touched. IRIX's _daemonize() closes every descriptor unless
// told not to, which would take sockets opened before the call with it.
// vfork() is IRIX's _vfork(), which IRIX's <unistd.h> names vfork only in
// XPG4-UX mode (IRIX's vfork is fork).
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#pragma weak daemon
#pragma weak _Exit
#pragma weak vfork

int daemon(int nochdir, int noclose) {
  switch (fork()) {
  case -1:
    return -1;
  case 0:
    break;
  default:
    _exit(0);
  }
  if (setsid() == -1)
    return -1;
  if (!nochdir)
    (void)chdir("/");
  if (!noclose) {
    int fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
      (void)dup2(fd, 0);
      (void)dup2(fd, 1);
      (void)dup2(fd, 2);
      if (fd > 2)
        (void)close(fd);
    }
  }
  return 0;
}

void _Exit(int status) { _exit(status); }

extern pid_t _vfork(void);
pid_t vfork(void) { return _vfork(); }

#endif // __sgi
