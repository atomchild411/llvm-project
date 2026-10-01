/*===---- stdio.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's snprintf and vsnprintf predate C99: they return the number of
 * characters written, never the number the whole output needs, so code
 * that sizes a buffer with snprintf(NULL, 0, ...), or checks the result for
 * truncation, silently gets a short string. Replace both with versions that
 * return what C99 says, on top of IRIX's own: when the output may not have
 * fit, format it again into ever larger scratch buffers to measure it.
 * They are declared whatever the feature macros, as C99 requires.
 *
 * Nor does IRIX's printf family know C99's length modifiers z, t, j and hh:
 * it takes the letter for the conversion, the arguments fall out of step
 * with the format, and a later %s prints whatever the misread argument
 * points at. printf, fprintf, sprintf and their v- forms are sent (by asm
 * label, so that printf still names printf to the compiler, as in
 * __attribute__((format(printf, ...)))) to compiler-rt's __irix_c99_*, which
 * rewrite the format into one IRIX understands and call IRIX's own; snprintf
 * and vsnprintf below rewrite it the same way.
 *
 * The scanf family likewise (irix/scanf_c99.c), which also supplies C99's
 * vscanf, vfscanf and vsscanf for 6.5.7. Both drop or change L before a
 * floating conversion: clang's long double is a double on IRIX, MIPSpro's
 * a pair of doubles.
 */

#ifndef __CLANG_IRIX_STDIO_H
#define __CLANG_IRIX_STDIO_H

/* IRIX 6.5.22's <stdio.h> uses va_list (vsscanf, vfscanf, ...) without
 * including <stdarg.h>, and our _VA_LIST_ keeps it from declaring its own:
 * declare just va_list (POSIX allows it here), as clang's <stdarg.h> does. */
#define __need_va_list
#include <stdarg.h>

/* IRIX's <stdio.h> includes <getopt.h>: tell the <getopt.h> wrapper, so that
 * getopt_long and struct option are only declared for a direct include. */
#ifndef __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT_STDIO
#endif
#include_next <stdio.h>
#ifdef __IRIX_GETOPT_INDIRECT_STDIO
#undef __IRIX_GETOPT_INDIRECT
#undef __IRIX_GETOPT_INDIRECT_STDIO
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* IRIX's own, under names of ours: its declarations depend on feature
 * macros, and spell the size as ssize_t, which is int or long by ABI. */
extern int __irix_libc_vsnprintf(char *, long, const char *, char *)
    __asm__("vsnprintf");
extern void *__irix_libc_malloc(size_t) __asm__("malloc");
extern void __irix_libc_free(void *) __asm__("free");

/* C99 formats: in compiler-rt (irix/printf_c99.c). */
extern const char *__irix_c99_fmt(const char *, char *, size_t, char **);
extern int printf(const char *, ...) __asm__("__irix_c99_printf");
extern int fprintf(FILE *, const char *, ...) __asm__("__irix_c99_fprintf");
extern int sprintf(char *, const char *, ...) __asm__("__irix_c99_sprintf");
extern int vprintf(const char *, __builtin_va_list)
    __asm__("__irix_c99_vprintf");
extern int vfprintf(FILE *, const char *, __builtin_va_list)
    __asm__("__irix_c99_vfprintf");
extern int vsprintf(char *, const char *, __builtin_va_list)
    __asm__("__irix_c99_vsprintf");
extern int scanf(const char *, ...) __asm__("__irix_c99_scanf");
extern int fscanf(FILE *, const char *, ...) __asm__("__irix_c99_fscanf");
extern int sscanf(const char *, const char *, ...) __asm__("__irix_c99_sscanf");
extern int vscanf(const char *, __builtin_va_list) __asm__("__irix_c99_vscanf");
extern int vfscanf(FILE *, const char *, __builtin_va_list)
    __asm__("__irix_c99_vfscanf");
extern int vsscanf(const char *, const char *, __builtin_va_list)
    __asm__("__irix_c99_vsscanf");

static __inline__
    __attribute__((__format__(__printf__, 3, 0))) int
    __irix_vsnprintf(char *__s, size_t __n, const char *__fmt,
                     __builtin_va_list __ap) {
  __builtin_va_list __ap2;
  char __fbuf[256], *__fheap;
  size_t __size;
  int __r;

  __fmt = __irix_c99_fmt(__fmt, __fbuf, sizeof __fbuf, &__fheap);
  __builtin_va_copy(__ap2, __ap);
  __r = __irix_libc_vsnprintf(__s, (long)__n, __fmt, __ap2);
  __builtin_va_end(__ap2);
  if (!(__r >= 0 && __n > 0 && (size_t)__r < __n - 1)) {
    /* It may not have fit -- or IRIX said -1, as it does for a size of 0
     * (snprintf(NULL, 0, ...), C99's way to measure) and for some
     * truncations: measure it in ever larger buffers, up to 64 MB. */
    for (__size = __n > 64 ? 2 * __n : 128;; __size *= 2) {
      char *__buf;
      if (__size > (size_t)64 << 20) {
        __r = -1;
        break;
      }
      __buf = (char *)__irix_libc_malloc(__size);
      if (__buf == 0) {
        __r = -1;
        break;
      }
      __builtin_va_copy(__ap2, __ap);
      __r = __irix_libc_vsnprintf(__buf, (long)__size, __fmt, __ap2);
      __builtin_va_end(__ap2);
      __irix_libc_free(__buf);
      if (__r >= 0 && (size_t)__r < __size - 1)
        break;
    }
  }
  __irix_libc_free(__fheap);
  return __r;
}

static __inline__ __attribute__((__format__(__printf__, 3, 4))) int
__irix_snprintf(char *__s, size_t __n, const char *__fmt, ...) {
  __builtin_va_list __ap;
  int __r;
  __builtin_va_start(__ap, __fmt);
  __r = __irix_vsnprintf(__s, __n, __fmt, __ap);
  __builtin_va_end(__ap);
  return __r;
}

#ifdef __cplusplus
}
#endif

/* POSIX 2008's (and the BSDs' asprintf), which IRIX's libc lacks: from
 * compiler-rt's irix/posix2008.c. */
#ifdef __cplusplus
extern "C" {
#endif
ssize_t getdelim(char **__restrict, size_t *__restrict, int, FILE *__restrict);
ssize_t getline(char **__restrict, size_t *__restrict, FILE *__restrict);
int dprintf(int, const char *__restrict, ...)
    __attribute__((__format__(__printf__, 2, 3)));
int vdprintf(int, const char *__restrict, __builtin_va_list)
    __attribute__((__format__(__printf__, 2, 0)));
int asprintf(char **, const char *, ...)
    __attribute__((__format__(__printf__, 2, 3)));
int vasprintf(char **, const char *, __builtin_va_list)
    __attribute__((__format__(__printf__, 2, 0)));
int renameat(int, const char *, int, const char *); /* irix/atfile.c */
#ifdef __cplusplus
}
#endif

/* Object-like, so that std::snprintf and using ::snprintf follow too. */
#define snprintf __irix_snprintf
#define vsnprintf __irix_vsnprintf

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
char *ctermid(char *);
int fseeko(FILE *, off_t, int);
off_t ftello(FILE *);
char *tempnam(const char *, const char *);
#ifdef __cplusplus
}
#endif
#endif

/* BSD's, in IRIX's libc, which its header declares only in SGI mode:
 * declared here outside it, with IRIX's own prototypes (glibc gives them
 * with _DEFAULT_SOURCE; Python, among others, uses them). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
int setbuffer(FILE *, char *, int);
int setlinebuf(FILE *);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_STDIO_H */
