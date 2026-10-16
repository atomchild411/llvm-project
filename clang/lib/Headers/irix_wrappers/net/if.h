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
 *
 * IRIX's header also uses the BSD types u_char, u_short, ... which
 * <sys/types.h> gives only outside POSIX and X/Open modes: bring them in
 * first (<sys/bsd_types.h> holds just those).
 */

#ifndef __CLANG_IRIX_NET_IF_H
#define __CLANG_IRIX_NET_IF_H

#include <sys/types.h>
#include <sys/bsd_types.h>
#include_next <net/if.h>

/* The functions unconditionally: IRIX declares them only in its INET6
 * (IPv6 kit) builds, and IF_NAMESIZE is no sign of them -- 6.5.22 defines
 * it without them, and c-ares defines its own before including this
 * header, which left if_indextoname undeclared there. The same prototype
 * again is harmless where IRIX does declare them. */
#ifndef IF_NAMESIZE
#define IF_NAMESIZE IFNAMSIZ
#endif
#ifdef __cplusplus
extern "C" {
#endif
unsigned int if_nametoindex(const char *);
char *if_indextoname(unsigned int, char *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_NET_IF_H */
