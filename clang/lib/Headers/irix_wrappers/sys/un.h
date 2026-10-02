/*===---- sys/un.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * SUN_LEN, which the BSDs, Linux and POSIX's rationale have and IRIX's
 * <sys/un.h> does not: the length of a sockaddr_un up to the end of the path
 * in it.
 */

#ifndef __CLANG_IRIX_SYS_UN_H
#define __CLANG_IRIX_SYS_UN_H

#include_next <sys/un.h>

#ifndef SUN_LEN
#include <string.h>
#define SUN_LEN(su)                                                            \
  (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif

#endif /* __CLANG_IRIX_SYS_UN_H */
