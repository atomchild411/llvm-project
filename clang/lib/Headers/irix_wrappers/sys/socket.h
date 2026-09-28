/*===---- sys/socket.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * socklen_t: IRIX 6.5.22's <sys/socket.h> defines it, behind _SOCKLEN_T, to
 * the type its socket calls take. 6.5.7's has no socklen_t at all; its calls
 * take int * in IRIX's own API (_NO_XOPEN4, which XPG5 alone does not turn
 * off there) and size_t * under XPG4. When the system header has not
 * defined it -- only 6.5.7's -- define it to match 6.5.7's prototypes.
 */

#ifndef __CLANG_IRIX_SYS_SOCKET_H
#define __CLANG_IRIX_SYS_SOCKET_H

#include_next <sys/socket.h>

#ifndef _SOCKLEN_T
#define _SOCKLEN_T
#if _NO_XOPEN4
typedef int socklen_t;
#else
typedef size_t socklen_t;
#endif
#endif

#endif /* __CLANG_IRIX_SYS_SOCKET_H */
