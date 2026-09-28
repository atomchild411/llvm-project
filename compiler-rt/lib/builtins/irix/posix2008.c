//===-- irix/posix2008.c - POSIX 2008 functions IRIX's libc lacks ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Functions of POSIX 2008 (and a few of C11 and the BSDs) that modern code
// takes for granted and no IRIX 6.5 libc has (checked against 6.5.22's
// exports). clang's IRIX wrappers declare them:
//
//   <string.h>  strndup stpcpy stpncpy strsep strcasestr explicit_bzero
//               strerror_r (the POSIX one, returning int)
//   <stdio.h>   getline getdelim dprintf vdprintf asprintf vasprintf
//   <stdlib.h>  posix_memalign aligned_alloc reallocarray mkostemp
//   <time.h>    timegm
//   <dirent.h>  dirfd
//
// posix_memalign and aligned_alloc use IRIX's memalign, whose blocks free()
// takes. The printf family goes through the <stdio.h> wrapper, so C99
// formats and return values hold here too.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern void *memalign(size_t, size_t);

// --- <string.h> -------------------------------------------------------------

char *strndup(const char *s, size_t n) {
  size_t len = strnlen(s, n);
  char *p = (char *)malloc(len + 1);
  if (!p)
    return 0;
  memcpy(p, s, len);
  p[len] = '\0';
  return p;
}

char *stpcpy(char *__restrict d, const char *__restrict s) {
  size_t len = strlen(s);
  memcpy(d, s, len + 1);
  return d + len;
}

char *stpncpy(char *__restrict d, const char *__restrict s, size_t n) {
  size_t len = strnlen(s, n);
  memcpy(d, s, len);
  memset(d + len, 0, n - len);
  return d + len;
}

char *strsep(char **sp, const char *delim) {
  char *s = *sp, *end;
  if (!s)
    return 0;
  end = s + strcspn(s, delim);
  if (*end) {
    *end = '\0';
    *sp = end + 1;
  } else {
    *sp = 0;
  }
  return s;
}

char *strcasestr(const char *h, const char *n) {
  size_t len = strlen(n), i;
  if (!len)
    return (char *)h;
  for (; *h; h++) {
    for (i = 0; i < len; i++)
      if (tolower((unsigned char)h[i]) != tolower((unsigned char)n[i]))
        break;
    if (i == len)
      return (char *)h;
  }
  return 0;
}

void explicit_bzero(void *p, size_t n) {
  memset(p, 0, n);
  // The stores must happen: let the compiler assume the memory is read.
  __asm__ __volatile__("" : : "r"(p) : "memory");
}

int strerror_r(int e, char *buf, size_t len) {
  const char *s;
  size_t slen;
  extern int sys_nerr;
  if (e < 0 || e >= sys_nerr) {
    // Still a message, as other systems give.
    if (len)
      snprintf(buf, len, "Unknown error %d", e);
    return EINVAL;
  }
  s = strerror(e);
  slen = strlen(s);
  if (slen >= len) {
    if (len) {
      memcpy(buf, s, len - 1);
      buf[len - 1] = '\0';
    }
    return ERANGE;
  }
  memcpy(buf, s, slen + 1);
  return 0;
}

// --- <stdio.h> --------------------------------------------------------------

ssize_t getdelim(char **__restrict lineptr, size_t *__restrict n, int delim,
                 FILE *__restrict f) {
  size_t len = 0;
  int c;

  if (!lineptr || !n || !f) {
    errno = EINVAL;
    return -1;
  }
  if (!*lineptr || !*n) {
    char *p = (char *)realloc(*lineptr, 128);
    if (!p) {
      errno = ENOMEM;
      return -1;
    }
    *lineptr = p;
    *n = 128;
  }
  for (;;) {
    c = getc(f);
    if (c == EOF)
      break;
    if (len + 2 > *n) {
      size_t size = *n * 2;
      char *p;
      if (size > ((size_t)-1 >> 1)) { // beyond ssize_t
        errno = EOVERFLOW;
        return -1;
      }
      p = (char *)realloc(*lineptr, size);
      if (!p) {
        errno = ENOMEM;
        return -1;
      }
      *lineptr = p;
      *n = size;
    }
    (*lineptr)[len++] = (char)c;
    if (c == delim)
      break;
  }
  (*lineptr)[len] = '\0';
  return len ? (ssize_t)len : -1;
}

ssize_t getline(char **__restrict lineptr, size_t *__restrict n,
                FILE *__restrict f) {
  return getdelim(lineptr, n, '\n', f);
}

int vasprintf(char **sp, const char *fmt, va_list ap) {
  va_list ap2;
  int len;
  char *s;

  va_copy(ap2, ap);
  len = vsnprintf(0, 0, fmt, ap2);
  va_end(ap2);
  if (len < 0 || !(s = (char *)malloc((size_t)len + 1))) {
    *sp = 0;
    return -1;
  }
  vsnprintf(s, (size_t)len + 1, fmt, ap);
  *sp = s;
  return len;
}

int asprintf(char **sp, const char *fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = vasprintf(sp, fmt, ap);
  va_end(ap);
  return r;
}

int vdprintf(int fd, const char *__restrict fmt, va_list ap) {
  char *s;
  int len = vasprintf(&s, fmt, ap), done = 0;
  if (len < 0)
    return -1;
  while (done < len) {
    ssize_t w = write(fd, s + done, (size_t)(len - done));
    if (w < 0) {
      if (errno == EINTR)
        continue;
      free(s);
      return -1;
    }
    done += (int)w;
  }
  free(s);
  return len;
}

int dprintf(int fd, const char *__restrict fmt, ...) {
  va_list ap;
  int r;
  va_start(ap, fmt);
  r = vdprintf(fd, fmt, ap);
  va_end(ap);
  return r;
}

// --- <stdlib.h> -------------------------------------------------------------

int posix_memalign(void **p, size_t align, size_t size) {
  void *m;
  if (align < sizeof(void *) || (align & (align - 1)))
    return EINVAL;
  m = memalign(align, size ? size : 1);
  if (!m)
    return ENOMEM;
  *p = m;
  return 0;
}

void *aligned_alloc(size_t align, size_t size) {
  if (!align || (align & (align - 1))) {
    errno = EINVAL;
    return 0;
  }
  if (align < sizeof(void *))
    align = sizeof(void *);
  return memalign(align, size ? size : 1);
}

// mkstemp, then the flags: O_CLOEXEC (the <fcntl.h> wrapper's) as FD_CLOEXEC,
// the rest (O_APPEND, O_SYNC, ...) through F_SETFL.
int mkostemp(char *tmpl, int flags) {
  int fd = mkstemp(tmpl);
  if (fd < 0)
    return -1;
#ifdef __IRIX_O_CLOEXEC
  if (flags & __IRIX_O_CLOEXEC) {
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    flags &= ~__IRIX_O_CLOEXEC;
  }
#endif
  if (flags && fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | flags) < 0) {
    int e = errno;
    close(fd);
    unlink(tmpl);
    errno = e;
    return -1;
  }
  return fd;
}

void *reallocarray(void *p, size_t n, size_t size) {
  if (size && n > SIZE_MAX / size) {
    errno = ENOMEM;
    return 0;
  }
  return realloc(p, n * size);
}

// --- <time.h> ---------------------------------------------------------------

// Days from 1970-01-01 to y-m-d in the proleptic Gregorian calendar.
static long long days_from_civil(long long y, int m, int d) {
  long long era, yoe, doy, doe;
  y -= m <= 2;
  era = (y >= 0 ? y : y - 399) / 400;
  yoe = y - era * 400;
  doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}

time_t timegm(struct tm *tm) {
  long long year = tm->tm_year + 1900LL, mon = tm->tm_mon, secs;
  time_t t;

  // Out-of-range months carry into the year, as mktime normalizes.
  year += mon / 12;
  mon %= 12;
  if (mon < 0) {
    mon += 12;
    year--;
  }
  secs = days_from_civil(year, (int)mon + 1, 1) + (tm->tm_mday - 1);
  secs = secs * 86400 + tm->tm_hour * 3600LL + tm->tm_min * 60LL + tm->tm_sec;
  t = (time_t)secs;
  if ((long long)t != secs) {
    errno = EOVERFLOW;
    return (time_t)-1;
  }
  gmtime_r(&t, tm);
  return t;
}

// --- <dirent.h> -------------------------------------------------------------

int dirfd(DIR *d) { return d->__dd_fd; }

#endif // defined(__sgi)
