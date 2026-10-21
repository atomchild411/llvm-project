/*===---- sys/capability.h - IRIX wrapper ----------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/capability.h> uses __uint64_t (cap_value_t, cap_flag_t),
 * size_t and ssize_t without including <sys/types.h>, which defines them
 * (through <sgidefs.h> for the first): a source that includes it first does
 * not compile (archivers/dar).
 */

#ifndef __CLANG_IRIX_SYS_CAPABILITY_H
#define __CLANG_IRIX_SYS_CAPABILITY_H

#include <sys/types.h>
#include_next <sys/capability.h>

#endif /* __CLANG_IRIX_SYS_CAPABILITY_H */
