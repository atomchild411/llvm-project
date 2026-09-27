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
 */

#ifndef __CLANG_IRIX_STDIO_H
#define __CLANG_IRIX_STDIO_H

#include_next <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IRIX's own, under names of ours: its declarations depend on feature
 * macros, and spell the size as ssize_t, which is int or long by ABI. */
extern int __irix_libc_vsnprintf(char *, long, const char *, char *)
    __asm__("vsnprintf");
extern void *__irix_libc_malloc(size_t) __asm__("malloc");
extern void __irix_libc_free(void *) __asm__("free");

static __inline__
    __attribute__((__format__(__printf__, 3, 0))) int
    __irix_vsnprintf(char *__s, size_t __n, const char *__fmt,
                     __builtin_va_list __ap) {
  __builtin_va_list __ap2;
  size_t __size;
  int __r;

  __builtin_va_copy(__ap2, __ap);
  __r = __irix_libc_vsnprintf(__s, (long)__n, __fmt, __ap2);
  __builtin_va_end(__ap2);
  if (__r < 0 || (__n > 0 && (size_t)__r < __n - 1))
    return __r; /* it fit, with room to spare */

  for (__size = __n > 64 ? 2 * __n : 128;; __size *= 2) {
    char *__buf = (char *)__irix_libc_malloc(__size);
    if (__buf == 0)
      return -1;
    __builtin_va_copy(__ap2, __ap);
    __r = __irix_libc_vsnprintf(__buf, (long)__size, __fmt, __ap2);
    __builtin_va_end(__ap2);
    __irix_libc_free(__buf);
    if (__r < 0 || (size_t)__r < __size - 1)
      return __r;
  }
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

/* Object-like, so that std::snprintf and using ::snprintf follow too. */
#define snprintf __irix_snprintf
#define vsnprintf __irix_vsnprintf

#endif /* __CLANG_IRIX_STDIO_H */
