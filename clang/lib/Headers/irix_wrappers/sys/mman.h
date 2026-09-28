/*===---- sys/mman.h - IRIX wrapper -----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * MAP_ANON and MAP_ANONYMOUS, which IRIX lacks: its anonymous memory is a
 * mapping of /dev/zero (SVR4's way). Give MAP_ANON a flag bit IRIX does not
 * use and route mmap and mmap64 to compiler-rt's builtins (irix/mman.c),
 * which map /dev/zero for it, with MAP_AUTORESRV on private mappings so
 * that swap is reserved as pages are touched, not for the whole range up
 * front (large reservations then behave as on other systems).
 */

#ifndef __CLANG_IRIX_SYS_MMAN_H
#define __CLANG_IRIX_SYS_MMAN_H

#include_next <sys/mman.h>

/* Code that defines MAP_ANONYMOUS as MAP_ANON (or the reverse) before
 * including this header gets the same bit. */
#ifndef __IRIX_MAP_ANON
#define __IRIX_MAP_ANON 0x40000000
#ifndef MAP_ANON
#define MAP_ANON __IRIX_MAP_ANON
#endif
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS __IRIX_MAP_ANON
#endif
#ifdef __cplusplus
extern "C" {
#endif
void *mmap(void *, size_t, int, int, int, off_t) __asm__("__irix_mmap");
void *mmap64(void *, size_t, int, int, int, long long) __asm__("__irix_mmap64");
#ifdef __cplusplus
}
#endif
#endif

/* The BSDs' (and Linux's) no-op flag for file mappings. */
#ifndef MAP_FILE
#define MAP_FILE 0
#endif

#endif /* __CLANG_IRIX_SYS_MMAN_H */
