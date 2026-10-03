//===-- irix/random.c - getentropy and arc4random for IRIX ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getentropy (<unistd.h>) and arc4random, arc4random_buf and
// arc4random_uniform (<stdlib.h>), as glibc and the BSDs have them; IRIX's
// libc has none, and portable code then compiles its own arc4random with a
// per-system entropy source it has none of for IRIX (LibreSSL, OpenNTPD:
// _getentropy_fail). Every byte comes from IRIX's /dev/urandom, read anew
// each time, so there is no state to reseed after fork. clang's IRIX
// wrappers declare them; weak, so a program's own copies are kept.
//
// arc4random cannot report failure: if the device cannot be read, it
// aborts rather than return predictable bytes.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#pragma weak getentropy
#pragma weak arc4random
#pragma weak arc4random_buf
#pragma weak arc4random_uniform

// Fill buf with n bytes from /dev/urandom: 0, or -1 with errno set.
static int urandom(void *buf, size_t n) {
  unsigned char *p = (unsigned char *)buf;
  int fd, saved;
  do
    fd = open("/dev/urandom", O_RDONLY);
  while (fd < 0 && errno == EINTR);
  if (fd < 0)
    return -1;
  while (n > 0) {
    ssize_t got = read(fd, p, n);
    if (got < 0 && errno == EINTR)
      continue;
    if (got <= 0) {
      saved = got < 0 ? errno : EIO;
      close(fd);
      errno = saved;
      return -1;
    }
    p += got;
    n -= (size_t)got;
  }
  close(fd);
  return 0;
}

int getentropy(void *buf, size_t n) {
  if (n > 256) {
    errno = EIO;
    return -1;
  }
  return urandom(buf, n);
}

void arc4random_buf(void *buf, size_t n) {
  if (urandom(buf, n) != 0)
    abort();
}

unsigned int arc4random(void) {
  unsigned int v;
  arc4random_buf(&v, sizeof v);
  return v;
}

// Uniform in [0, upper): reject the values below 2^32 mod upper, so every
// remainder is equally likely.
unsigned int arc4random_uniform(unsigned int upper) {
  unsigned int min, r;
  if (upper < 2)
    return 0;
  min = (0u - upper) % upper;
  do
    r = arc4random();
  while (r < min);
  return r % upper;
}

#endif // defined(__sgi)
