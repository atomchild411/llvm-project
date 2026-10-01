//===-- irix/dladdr.c - dladdr for IRIX -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// dladdr(), which IRIX's libc lacks (clang's IRIX <dlfcn.h> declares it and
// Dl_info). IRIX's runtime linker answers the same question through
// _rld_new_interface(_RLD_DLADDR): nonzero and Dl_info filled in when the
// address is in a loaded object, zero otherwise, as dladdr() returns.
// Checked on IRIX 6.5.22: a libc function's address names libc.so.1 and the
// function; an address in no object returns 0.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <dlfcn.h>
#include <rld_interface.h>

int dladdr(const void *addr, Dl_info *info) {
  return _rld_new_interface(_RLD_DLADDR, addr, info) != 0;
}

#endif
