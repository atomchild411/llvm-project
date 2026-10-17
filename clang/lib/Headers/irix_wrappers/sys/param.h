/*===---- sys/param.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * glibc's <sys/param.h> includes <limits.h>, and code written for it uses
 * INT_MAX and the rest with nothing else (mlmmj, libdnsres,
 * rocksndiamonds); IRIX's does not include it. Included here after IRIX's.
 */

#ifndef __CLANG_IRIX_SYS_PARAM_H
#define __CLANG_IRIX_SYS_PARAM_H

#include_next <sys/param.h>
#include <limits.h>

#endif /* __CLANG_IRIX_SYS_PARAM_H */
