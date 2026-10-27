//===-- irix/memstream.c - fmemopen and open_memstream for IRIX -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// fmemopen and open_memstream (POSIX 2008), which IRIX lacks. IRIX's stdio
// has no funopen or fopencookie, so each stream is a tmpfile(): clang's
// IRIX <stdio.h> sends fclose and fflush here (as __irix_fclose and
// __irix_fflush), and these bring the caller's buffer up to date, as POSIX
// has them do:
//
//   open_memstream: *bufp gets the contents (allocated here, realloc'd at
//     each update, NUL-terminated; the caller frees it), *sizep the smaller
//     of the current position and the length.
//   fmemopen: "r" streams read a copy of buf[0..size). Others ("r+" too)
//     write back at most size bytes of the contents into buf, and a NUL
//     after them where there is room. "w" starts empty, "a" at buf's first
//     NUL. A NULL buf is a buffer of size bytes of our own, freed at fclose.
//
// Every other stream goes straight to IRIX's fclose and fflush. A stream
// closed by code compiled without clang's headers (none of ours do) keeps
// its last update.
//
// <stdio.h> is not included: its fclose and fflush would be these.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct __irix_memstream_file FILE;

extern int __irix_libc_fclose(FILE *) __asm__("fclose");
extern int __irix_libc_fflush(FILE *) __asm__("fflush");
extern FILE *__irix_libc_tmpfile(void) __asm__("tmpfile");
extern long __irix_libc_ftell(FILE *) __asm__("ftell");
extern int __irix_libc_fseek(FILE *, long, int) __asm__("fseek");
extern size_t __irix_libc_fread(void *, size_t, size_t, FILE *)
    __asm__("fread");
extern size_t __irix_libc_fwrite(const void *, size_t, size_t, FILE *)
    __asm__("fwrite");

#define MS_SEEK_SET 0
#define MS_SEEK_END 2

struct memstream {
  FILE *f;
  int dynamic;    // open_memstream
  int writeback;  // fmemopen, not "r"
  char **bufp;    // open_memstream
  size_t *sizep;
  char *buf;      // fmemopen
  size_t size;
  int ownbuf;
};

static struct memstream *streams;
static int nstreams, maxstreams;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static int add(const struct memstream *m) {
  if (nstreams == maxstreams) {
    int n = maxstreams ? 2 * maxstreams : 8;
    struct memstream *s = realloc(streams, n * sizeof *s);
    if (s == NULL)
      return -1;
    streams = s;
    maxstreams = n;
  }
  streams[nstreams++] = *m;
  return 0;
}

static struct memstream *find(FILE *f) {
  for (int i = 0; i < nstreams; ++i)
    if (streams[i].f == f)
      return &streams[i];
  return NULL;
}

// The contents into the caller's buffer; the stream's position kept.
static int update(struct memstream *m) {
  long pos, len;
  if (__irix_libc_fflush(m->f) != 0)
    return -1;
  if (!m->dynamic && !m->writeback)
    return 0;
  if ((pos = __irix_libc_ftell(m->f)) < 0 ||
      __irix_libc_fseek(m->f, 0, MS_SEEK_END) != 0 ||
      (len = __irix_libc_ftell(m->f)) < 0 ||
      __irix_libc_fseek(m->f, 0, MS_SEEK_SET) != 0)
    return -1;
  if (m->dynamic) {
    char *b = realloc(*m->bufp, (size_t)len + 1);
    if (b == NULL)
      return -1;
    *m->bufp = b;
    b[__irix_libc_fread(b, 1, (size_t)len, m->f)] = '\0';
    *m->sizep = (size_t)(pos < len ? pos : len);
  } else {
    size_t n = (size_t)len < m->size ? (size_t)len : m->size;
    n = __irix_libc_fread(m->buf, 1, n, m->f);
    if (n < m->size)
      m->buf[n] = '\0';
  }
  return __irix_libc_fseek(m->f, pos, MS_SEEK_SET);
}

FILE *open_memstream(char **bufp, size_t *sizep) {
  struct memstream m = {0};
  if (bufp == NULL || sizep == NULL) {
    __irix_seterrno(EINVAL);
    return NULL;
  }
  if ((m.f = __irix_libc_tmpfile()) == NULL)
    return NULL;
  m.dynamic = 1;
  m.bufp = bufp;
  m.sizep = sizep;
  *bufp = NULL;
  *sizep = 0;
  pthread_mutex_lock(&lock);
  int r = add(&m) == 0 ? update(&streams[nstreams - 1]) : -1;
  pthread_mutex_unlock(&lock);
  if (r != 0) {
    __irix_libc_fclose(m.f);
    return NULL;
  }
  return m.f;
}

FILE *fmemopen(void *buf, size_t size, const char *mode) {
  struct memstream m = {0};
  if (size == 0 || mode == NULL || strchr("rwa", mode[0]) == NULL) {
    __irix_seterrno(EINVAL);
    return NULL;
  }
  m.buf = buf;
  if (m.buf == NULL) {
    if ((m.buf = calloc(1, size)) == NULL)
      return NULL;
    m.ownbuf = 1;
  }
  m.size = size;
  m.writeback = mode[0] != 'r' || strchr(mode, '+') != NULL;
  if ((m.f = __irix_libc_tmpfile()) == NULL)
    goto fail;
  if (mode[0] == 'r' || mode[0] == 'a') {
    // "r": all size bytes; "a": up to the first NUL, the place to append.
    size_t n = size;
    if (mode[0] == 'a')
      n = strnlen(m.buf, size);
    if (__irix_libc_fwrite(m.buf, 1, n, m.f) != n)
      goto fail;
    if (mode[0] == 'r')
      __irix_libc_fseek(m.f, 0, MS_SEEK_SET);
  } else {
    m.buf[0] = '\0';
  }
  pthread_mutex_lock(&lock);
  int r = add(&m);
  pthread_mutex_unlock(&lock);
  if (r != 0)
    goto fail;
  return m.f;
fail:
  if (m.f != NULL)
    __irix_libc_fclose(m.f);
  if (m.ownbuf)
    free(m.buf);
  return NULL;
}

int __irix_fflush(FILE *f) {
  int r = 0;
  if (nstreams == 0)
    return __irix_libc_fflush(f);
  pthread_mutex_lock(&lock);
  if (f == NULL) {
    for (int i = 0; i < nstreams; ++i)
      if (update(&streams[i]) != 0)
        r = -1;
    pthread_mutex_unlock(&lock);
    return __irix_libc_fflush(NULL) != 0 ? -1 : r;
  }
  struct memstream *m = find(f);
  r = m != NULL ? update(m) : __irix_libc_fflush(f);
  pthread_mutex_unlock(&lock);
  return r;
}

int __irix_fclose(FILE *f) {
  int r = 0;
  if (nstreams == 0)
    return __irix_libc_fclose(f);
  pthread_mutex_lock(&lock);
  struct memstream *m = find(f);
  if (m != NULL) {
    r = update(m);
    if (m->ownbuf)
      free(m->buf);
    *m = streams[--nstreams];
  }
  pthread_mutex_unlock(&lock);
  return __irix_libc_fclose(f) != 0 ? -1 : r;
}

#endif // defined(__sgi)
