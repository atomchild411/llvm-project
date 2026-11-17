//===-- irix/bsearch.c - bsearch of an empty array -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// IRIX's bsearch() takes base + (nel - 1) * size as the last element and
// gives up only if that is below base. For an empty array at NULL (nel 0,
// base NULL, as GTK 3 passes) it wraps to the top of the address space, and
// bsearch calls the comparator on an element near 0x7ffffff0: SIGSEGV.
// clang's IRIX <stdlib.h> sends bsearch here (by asm label): nothing to
// search finds nothing; every other call goes to IRIX's.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stddef.h>

extern void *__irix_libc_bsearch(const void *, const void *, size_t, size_t,
                                 int (*)(const void *, const void *))
    __asm__("bsearch");

void *__irix_bsearch(const void *key, const void *base, size_t nel,
                     size_t size, int (*compar)(const void *, const void *)) {
  if (nel == 0 || size == 0)
    return NULL;
  return __irix_libc_bsearch(key, base, nel, size, compar);
}

#endif // defined(__sgi)
