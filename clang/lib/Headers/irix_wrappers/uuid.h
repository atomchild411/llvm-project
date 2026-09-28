/*===---- uuid.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX has the DCE UUID calls BSD's <uuid.h> declares -- uuid_create,
 * uuid_to_string and the rest, exported by libc -- but declares them in
 * <sys/uuid.h> and has no <uuid.h>. Code that finds uuid_create in libc then
 * includes <uuid.h> (libSM's sm_genid.c). Provide it, as FreeBSD's does, by
 * including <sys/uuid.h>. The status argument is uint_t there, uint32_t on
 * the BSDs: the same type on IRIX.
 */

#ifndef __CLANG_IRIX_UUID_H
#define __CLANG_IRIX_UUID_H

#include <sys/types.h>
#include <sys/uuid.h>

#endif /* __CLANG_IRIX_UUID_H */
