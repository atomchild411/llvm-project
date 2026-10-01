//===-- irix/sysconf.c - sysconf names IRIX lacks ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// sysconf for the names clang's IRIX <unistd.h> adds (numbered from 1001;
// see that wrapper): answered from IRIX's own facilities. Every other name
// goes to IRIX's sysconf.
//
//   _SC_HOST_NAME_MAX     MAXHOSTNAMELEN - 1 (POSIX leaves out the NUL)
//   _SC_PHYS_PAGES        sysmp(MP_SAGET, MPSA_RMINFO): physmem
//   _SC_AVPHYS_PAGES      ... freemem
//   _SC_SYMLOOP_MAX       MAXSYMLINKS
//   _SC_MONOTONIC_CLOCK,  supported (200112): clang's <time.h> wrapper
//   _SC_CLOCK_SELECTION   provides CLOCK_MONOTONIC
//   _SC_SPIN_LOCKS, _SC_BARRIERS, _SC_READER_WRITER_LOCKS, _SC_CPUTIME,
//   _SC_THREAD_CPUTIME    -1: not supported (6.5.22's libpthread has no
//                         spin locks or barriers; no CPU-time clocks)
//   _SC_RAW_SOCKETS       supported (200112): IRIX has SOCK_RAW
//   _SC_REGEXP, _SC_SHELL supported (1): regcomp, /bin/sh
//   _SC_SPAWN             supported (200112): compiler-rt's posix_spawn
//   _SC_XOPEN_STREAMS     supported (1): IRIX has STREAMS
//   _SC_V6_*, _SC_V7_*    IRIX's answer for the matching _SC_XBS5_*
//                         compilation environment
//   the PBS options, _SC_ADVISORY_INFO, _SC_IPV6, the sporadic server,
//   robust mutexes, _SC_TIMEOUTS, tracing, typed memory objects,
//   _SC_XOPEN_REALTIME_THREADS, _SC_XOPEN_UUCP
//                         -1: not supported (or, for a limit, none known)
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <sys/param.h>
#include <sys/sysmp.h>
#include <sys/types.h>
#include <unistd.h>

extern long __irix_libc_sysconf(int) __asm__("sysconf");

static long rminfo_pages(int which) {
  struct rminfo ri;
  if (sysmp(MP_SAGET, MPSA_RMINFO, &ri, sizeof(ri)) == -1)
    return -1;
  return which ? (long)ri.freemem : (long)ri.physmem;
}

long __irix_sysconf(int name) {
  switch (name) {
  case 1001:
    return MAXHOSTNAMELEN - 1;
  case 1002:
    return rminfo_pages(0);
  case 1003:
    return rminfo_pages(1);
  case 1004:
    return MAXSYMLINKS;
  case 1005:
  case 1006:
    return 200112L;
  case 1007:
  case 1008:
  case 1009:
  case 1010:
  case 1011:
    return -1;
  case 1020: // _SC_RAW_SOCKETS
  case 1023: // _SC_SPAWN
    return 200112L;
  case 1021: // _SC_REGEXP
  case 1022: // _SC_SHELL
  case 1048: // _SC_XOPEN_STREAMS
    return 1;
  case 1039: // _SC_V6_ILP32_OFF32 .. _SC_V6_LPBIG_OFFBIG
  case 1040:
  case 1041:
  case 1042:
    return __irix_libc_sysconf(_SC_XBS5_ILP32_OFF32 + (name - 1039));
  case 1043: // _SC_V7_ILP32_OFF32 .. _SC_V7_LPBIG_OFFBIG
  case 1044:
  case 1045:
  case 1046:
    return __irix_libc_sysconf(_SC_XBS5_ILP32_OFF32 + (name - 1043));
  }
  if (name >= 1012 && name <= 1049)
    return -1;
  return __irix_libc_sysconf(name);
}

#endif // defined(__sgi)
