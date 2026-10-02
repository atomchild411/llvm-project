//===-- irix/ssp.c - stack protector runtime for IRIX ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// __stack_chk_guard and __stack_chk_fail, which -fstack-protector code calls
// and IRIX's libc lacks (packages that turn the protector on themselves
// failed to link). The guard is seeded at startup from the time, the pid
// and an address; a failed check reports and aborts, as glibc's does.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#pragma weak __stack_chk_guard
#pragma weak __stack_chk_fail

uintptr_t __stack_chk_guard = 0xff0a0000;

__attribute__((constructor)) static void seed_guard(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uintptr_t g = (uintptr_t)tv.tv_sec ^ ((uintptr_t)tv.tv_usec << 12) ^
                ((uintptr_t)getpid() << 20) ^ (uintptr_t)&tv;
  // A zero byte first in memory, as glibc does, so a string overflow cannot
  // reproduce the guard: the most significant byte on big-endian IRIX.
  __stack_chk_guard = g & ~((uintptr_t)0xff << (8 * (sizeof g - 1)));
}

void __stack_chk_fail(void) {
  static const char msg[] = "*** stack smashing detected ***: terminated\n";
  write(2, msg, sizeof msg - 1);
  abort();
}

#endif // __sgi
