/*===---- net/if.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3493's if_nametoindex, if_indextoname, if_nameindex and
 * if_freenameindex, and IF_NAMESIZE, which IRIX lacks: compiler-rt
 * (irix/ifnameindex.c) defines them over SIOCGIFCONF.
 *
 * IRIX's header also uses the BSD types u_char, u_short, ... which
 * <sys/types.h> gives only outside POSIX and X/Open modes: bring them in
 * first (<sys/bsd_types.h> holds just those).
 *
 * It also brings in <sys/sema.h>, whose kernel "struct mutex" is a global
 * name in C++: with "using namespace std", a plain "mutex" then means
 * either it or std::mutex (kea).  C++ sees it under another tag; its
 * mutex_t typedef is unchanged.
 */

#ifndef __CLANG_IRIX_NET_IF_H
#define __CLANG_IRIX_NET_IF_H

#include <sys/types.h>
#include <sys/bsd_types.h>
#ifdef __cplusplus
#pragma push_macro("mutex")
#undef mutex
#define mutex __irix_sema_mutex
#endif
#include_next <net/if.h>
#ifdef __cplusplus
#pragma pop_macro("mutex")
#endif

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
/* IRIX's struct for it is also INET6's only. */
#ifndef INET6
struct if_nameindex {
  unsigned int if_index;
  char *if_name;
};
#endif
struct if_nameindex *if_nameindex(void);
void if_freenameindex(struct if_nameindex *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_NET_IF_H */
