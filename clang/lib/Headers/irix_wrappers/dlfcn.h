/*===---- dlfcn.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_DLFCN_H
#define __CLANG_IRIX_DLFCN_H

#include_next <dlfcn.h>

/* dladdr(), which IRIX's libc lacks: compiler-rt's IRIX builtins provide it
 * over the runtime linker's own dladdr service, which fills in this
 * structure.  <rld_interface.h> declares the same one under the same guard,
 * so a program may include both. */
#ifndef _RLD_INTERFACE_DLFCN_H_DLADDR
#define _RLD_INTERFACE_DLFCN_H_DLADDR
typedef struct Dl_info {
  const char *dli_fname; /* the object containing the address */
  void *dli_fbase;       /* where that object is loaded */
  const char *dli_sname; /* the nearest symbol at or below the address */
  void *dli_saddr;       /* that symbol's address */
  int dli_version;
  int dli_reserved1;
  long dli_reserved[4];
} Dl_info;
#endif

/* For code that otherwise writes its own dladdr over rld, as IRIX's
 * dladdr(3C) tells it to (OpenSSL). */
#define __CLANG_IRIX_DLADDR 1
#ifdef __cplusplus
extern "C" {
#endif
int dladdr(const void *, Dl_info *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_DLFCN_H */
