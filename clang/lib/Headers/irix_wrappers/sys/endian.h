/*===---- sys/endian.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/endian.h> stops unless <standards.h> came first, and has no
 * htobe16 ... le64toh (the BSDs' <sys/endian.h> does): include
 * <standards.h>, then IRIX's header, then the rest.
 */
#ifndef __CLANG_IRIX_SYS_ENDIAN_H
#define __CLANG_IRIX_SYS_ENDIAN_H
#include <standards.h>
#include_next <sys/endian.h>
#include <__irix_endian_swap.h>
#endif /* __CLANG_IRIX_SYS_ENDIAN_H */
