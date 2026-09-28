//===-- irix/strsignal.c - strsignal for IRIX -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// POSIX 2008's strsignal, which no IRIX libc has (the X server's fatal
// signal handler calls it). IRIX's libc keeps the descriptions psignal
// prints in _sys_siglist, _sys_nsig entries long; clang's IRIX <string.h>
// declares strsignal.
//
// It returns a constant string, never a buffer, so it is safe in a signal
// handler, which is where it tends to be called.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

extern char *_sys_siglist[];
extern int _sys_nsig;

char *strsignal(int sig) {
  if (sig > 0 && sig < _sys_nsig && _sys_siglist[sig])
    return _sys_siglist[sig];
  return (char *)"Unknown signal";
}

#endif // defined(__sgi)
