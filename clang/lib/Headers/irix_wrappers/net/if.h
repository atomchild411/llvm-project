/*===---- net/if.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3493's if_nametoindex and if_indextoname, and IF_NAMESIZE, which IRIX
 * lacks. They exist for IPv6 scope ids, which IRIX has no use for: compiler-rt
 * (irix/netdb.c) defines them to find no interface.
 */

#ifndef __CLANG_IRIX_NET_IF_H
#define __CLANG_IRIX_NET_IF_H

#include_next <net/if.h>

#ifndef IF_NAMESIZE
#define IF_NAMESIZE IFNAMSIZ
#ifdef __cplusplus
extern "C" {
#endif
unsigned int if_nametoindex(const char *);
char *if_indextoname(unsigned int, char *);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_NET_IF_H */
