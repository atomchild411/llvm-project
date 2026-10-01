/*===---- netinet/tcp.h - IRIX wrapper --------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's header uses the BSD types u_char, u_short, ... which <sys/types.h>
 * gives only outside POSIX and X/Open modes (libuv's <uv/unix.h>, in
 * cmake): bring them in first, as <net/if.h> does.
 */

#ifndef __CLANG_IRIX_NETINET_TCP_H
#define __CLANG_IRIX_NETINET_TCP_H

#include <sys/types.h>
#include <sys/bsd_types.h>
#include_next <netinet/tcp.h>

#endif /* __CLANG_IRIX_NETINET_TCP_H */
