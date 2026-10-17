/*===---- sys/stream.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/stream.h> uses uint and hostid_t, which <sys/types.h> and
 * <sys/bsd_types.h> declare only in SGI mode: in a POSIX or X/Open mode
 * (vte's -D_XOPEN_SOURCE=700) it stops compiling. Give the header its own
 * names for them while it is read, without declaring uint or hostid_t for
 * the program.
 */

#ifndef __CLANG_IRIX_SYS_STREAM_H
#define __CLANG_IRIX_SYS_STREAM_H

#include <sys/types.h>

typedef unsigned int __irix_stream_uint;
typedef long __irix_stream_hostid_t;
#define uint __irix_stream_uint
#define hostid_t __irix_stream_hostid_t
#include_next <sys/stream.h>
#undef uint
#undef hostid_t

#endif /* __CLANG_IRIX_SYS_STREAM_H */
