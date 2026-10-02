//===-- irix/dlsym.c - dlsym with RTLD_DEFAULT for IRIX -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// dlsym() with RTLD_DEFAULT, which IRIX's lacks; clang's IRIX <dlfcn.h>
// defines RTLD_DEFAULT as a null handle and routes dlsym() here, as
// __irix_dlsym. A name looked up with RTLD_DEFAULT is found among the global
// symbols of the program and every library loaded, by rld's
// _RLD_NAME_TO_ADDR (which finds the program's own symbols only if they are
// exported, as with glibc); dlerror() says nothing when it is not found.
// Every other handle goes to IRIX's dlsym() unchanged.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <dlfcn.h>
#include <rld_interface.h>

#ifndef RTLD_DEFAULT
#error "clang's IRIX <dlfcn.h> wrapper defines RTLD_DEFAULT"
#endif

// IRIX's own dlsym, under its own name (the wrapper renames the one programs
// see).
extern void *__irix_libc_dlsym(void *, const char *) __asm__("dlsym");

void *__irix_dlsym(void *handle, const char *name) {
  if (handle == RTLD_DEFAULT)
    return _rld_new_interface(_RLD_NAME_TO_ADDR, name);
  return __irix_libc_dlsym(handle, name);
}

#endif // defined(__sgi)
