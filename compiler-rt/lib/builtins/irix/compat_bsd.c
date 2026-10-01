//===-- irix/compat_bsd.c - BSD and glibc calls IRIX lacks ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// cfmakeraw (<termios.h>), lchmod (<sys/stat.h>), get_nprocs and
// get_nprocs_conf (<sys/sysinfo.h>), declared by clang's IRIX wrappers.
// Weak, so a program that brings its own copy keeps it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#pragma weak cfmakeraw
#pragma weak lchmod
#pragma weak get_nprocs
#pragma weak get_nprocs_conf

// As the BSDs and glibc define it (and Solaris' cfmakeraw recipe).
void cfmakeraw(struct termios *t) {
  t->c_iflag &= ~(IMAXBEL | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                  ICRNL | IXON);
  t->c_oflag &= ~OPOST;
  t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  t->c_cflag &= ~(CSIZE | PARENB);
  t->c_cflag |= CS8;
  t->c_cc[VMIN] = 1;
  t->c_cc[VTIME] = 0;
}

// IRIX cannot change a symbolic link's mode; glibc's lchmod fails with
// ENOTSUP in that case too, and does the chmod otherwise.
int lchmod(const char *path, mode_t mode) {
  struct stat st;
  if (lstat(path, &st) == -1)
    return -1;
  if (S_ISLNK(st.st_mode)) {
    errno = ENOTSUP;
    return -1;
  }
  return chmod(path, mode);
}

int get_nprocs(void) { return (int)sysconf(_SC_NPROC_ONLN); }
int get_nprocs_conf(void) { return (int)sysconf(_SC_NPROC_CONF); }

#endif // defined(__sgi)
