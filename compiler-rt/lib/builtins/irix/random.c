//===-- irix/random.c - getentropy, getrandom and arc4random for IRIX -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getentropy (<unistd.h>), getrandom (<sys/random.h>) and arc4random,
// arc4random_buf and arc4random_uniform (<stdlib.h>), as glibc and the BSDs
// have them; IRIX's
// libc has none, and portable code then compiles its own arc4random with a
// per-system entropy source it has none of for IRIX (LibreSSL, OpenNTPD:
// _getentropy_fail). clang's IRIX wrappers declare them; weak, so a
// program's own copies are kept.
//
// getentropy and getrandom read IRIX's /dev/urandom each time, and fail if
// it cannot be read. arc4random is OpenBSD's design: ChaCha20 keyed from
// /dev/urandom, rekeyed from its own output after every buffer, and stirred
// with fresh device bytes every 1.6 MB, after fork, and when the program
// starts -- so that a daemon that chroots (sshd's privilege separation,
// unbound, postfix) has its key before /dev is out of reach. When the
// device cannot be read later, it keeps going on the key it has (after
// fork mixing in the child's pid and the time, so a child never repeats
// its parent's bytes). arc4random cannot report failure: if it was never
// keyed and the device cannot be read, it aborts rather than return
// predictable bytes.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#pragma weak getentropy
#pragma weak getrandom
#pragma weak arc4random
#pragma weak arc4random_buf
#pragma weak arc4random_uniform

// Fill buf with n bytes from dev: 0, or -1 with errno set.
static int read_device(const char *dev, void *buf, size_t n) {
  unsigned char *p = (unsigned char *)buf;
  int fd, saved;
  do
    fd = open(dev, O_RDONLY);
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
      __irix_seterrno(saved);
      return -1;
    }
    p += got;
    n -= (size_t)got;
  }
  close(fd);
  return 0;
}

static int urandom(void *buf, size_t n) {
  return read_device("/dev/urandom", buf, n);
}

#define GRND_RANDOM 0x02

// glibc's getrandom: all n bytes or -1 (GRND_NONBLOCK and GRND_INSECURE
// are ignored).
ssize_t getrandom(void *buf, size_t n, unsigned int flags) {
  if (read_device(flags & GRND_RANDOM ? "/dev/random" : "/dev/urandom", buf,
                  n) != 0)
    return -1;
  return (ssize_t)n;
}

int getentropy(void *buf, size_t n) {
  if (n > 256) {
    __irix_seterrno(EIO);
    return -1;
  }
  return urandom(buf, n);
}

// The generator: a ChaCha20 key and nonce (the seed), and a buffer of
// keystream whose first RS_SEED bytes become the next seed.
#define RS_KEY 32
#define RS_SEED (RS_KEY + 8)
#define RS_BUF (64 * 16)
#define RS_RESEED 1600000

static pthread_mutex_t rs_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned int rs_input[16];
static unsigned char rs_buf[RS_BUF];
static size_t rs_have;  // unread bytes at the end of rs_buf
static size_t rs_count; // bytes left before the next stir
static pid_t rs_pid;
static int rs_keyed;

static unsigned int le32(const unsigned char *p) {
  return (unsigned int)p[0] | (unsigned int)p[1] << 8 |
         (unsigned int)p[2] << 16 | (unsigned int)p[3] << 24;
}

static void chacha_keysetup(const unsigned char *seed) {
  static const unsigned char sigma[16] = "expand 32-byte k";
  int i;
  for (i = 0; i < 4; i++)
    rs_input[i] = le32(sigma + 4 * i);
  for (i = 0; i < 8; i++)
    rs_input[4 + i] = le32(seed + 4 * i);
  rs_input[12] = rs_input[13] = 0;
  rs_input[14] = le32(seed + RS_KEY);
  rs_input[15] = le32(seed + RS_KEY + 4);
}

#define ROTL(v, n) (((v) << (n)) | ((v) >> (32 - (n))))
#define QR(a, b, c, d)                                                         \
  do {                                                                         \
    a += b, d ^= a, d = ROTL(d, 16);                                           \
    c += d, b ^= c, b = ROTL(b, 12);                                           \
    a += b, d ^= a, d = ROTL(d, 8);                                            \
    c += d, b ^= c, b = ROTL(b, 7);                                            \
  } while (0)

static void chacha_block(unsigned char *out) {
  unsigned int x[16];
  int i;
  memcpy(x, rs_input, sizeof x);
  for (i = 0; i < 10; i++) {
    QR(x[0], x[4], x[8], x[12]);
    QR(x[1], x[5], x[9], x[13]);
    QR(x[2], x[6], x[10], x[14]);
    QR(x[3], x[7], x[11], x[15]);
    QR(x[0], x[5], x[10], x[15]);
    QR(x[1], x[6], x[11], x[12]);
    QR(x[2], x[7], x[8], x[13]);
    QR(x[3], x[4], x[9], x[14]);
  }
  for (i = 0; i < 16; i++) {
    unsigned int v = x[i] + rs_input[i];
    out[4 * i] = (unsigned char)v;
    out[4 * i + 1] = (unsigned char)(v >> 8);
    out[4 * i + 2] = (unsigned char)(v >> 16);
    out[4 * i + 3] = (unsigned char)(v >> 24);
  }
  if (++rs_input[12] == 0)
    ++rs_input[13];
}

// Refill the buffer and take its first RS_SEED bytes, xor extra, as the
// next key, so that the bytes handed out cannot be recomputed from a later
// state.
static void rs_rekey(const unsigned char *extra, size_t n) {
  size_t i;
  for (i = 0; i < RS_BUF; i += 64)
    chacha_block(rs_buf + i);
  for (i = 0; i < n && i < RS_SEED; i++)
    rs_buf[i] ^= extra[i];
  chacha_keysetup(rs_buf);
  memset(rs_buf, 0, RS_SEED);
  rs_have = RS_BUF - RS_SEED;
}

// Mix in fresh device bytes: 0, or -1 if the device cannot be read.
static int rs_stir(void) {
  unsigned char seed[RS_SEED];
  int saved = errno;
  if (urandom(seed, sizeof seed) != 0) {
    __irix_seterrno(saved);
    return -1;
  }
  if (!rs_keyed)
    chacha_keysetup(seed);
  rs_rekey(seed, sizeof seed);
  memset(seed, 0, sizeof seed);
  rs_keyed = 1;
  rs_count = RS_RESEED;
  return 0;
}

// With rs_lock held: keyed, and stirred when it is time or after fork.
static void rs_check(size_t n) {
  pid_t pid = getpid();
  if (!rs_keyed) {
    if (rs_stir() != 0)
      abort();
  } else if (pid != rs_pid) {
    if (rs_stir() != 0) {
      struct {
        pid_t pid;
        struct timeval tv;
      } mix;
      memset(&mix, 0, sizeof mix);
      mix.pid = pid;
      gettimeofday(&mix.tv, 0);
      rs_rekey((const unsigned char *)&mix, sizeof mix);
    }
  } else if (rs_count <= n) {
    if (rs_stir() != 0)
      rs_count = RS_RESEED; // try again later
  }
  rs_pid = pid;
  rs_count = rs_count > n ? rs_count - n : 0;
}

// Keyed when the program starts (no lock: one thread so far).
__attribute__((constructor)) static void rs_init(void) {
  if (!rs_keyed && rs_stir() == 0)
    rs_pid = getpid();
}

void arc4random_buf(void *buf, size_t n) {
  unsigned char *p = (unsigned char *)buf;
  pthread_mutex_lock(&rs_lock);
  rs_check(n);
  while (n > 0) {
    unsigned char *k;
    size_t m;
    if (rs_have == 0)
      rs_rekey(0, 0);
    m = n < rs_have ? n : rs_have;
    k = rs_buf + RS_BUF - rs_have;
    memcpy(p, k, m);
    memset(k, 0, m);
    p += m;
    n -= m;
    rs_have -= m;
  }
  pthread_mutex_unlock(&rs_lock);
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
