/*===---- ifaddrs.h - IRIX ---------------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * The BSDs' and Linux's getifaddrs and freeifaddrs, which IRIX lacks:
 * compiler-rt's IRIX builtins define them (irix/ifaddrs.c), for IPv4, from
 * SIOCGIFCONF and the per-interface ioctls. The BSD layout: ifa_dstaddr is
 * also the broadcast address.
 */

#ifndef __CLANG_IRIX_IFADDRS_H
#define __CLANG_IRIX_IFADDRS_H

#include <sys/socket.h>

struct ifaddrs {
  struct ifaddrs *ifa_next;
  char *ifa_name;
  unsigned int ifa_flags;
  struct sockaddr *ifa_addr;
  struct sockaddr *ifa_netmask;
  struct sockaddr *ifa_dstaddr;
  void *ifa_data;
};

#ifndef ifa_broadaddr
#define ifa_broadaddr ifa_dstaddr
#endif

#ifdef __cplusplus
extern "C" {
#endif
int getifaddrs(struct ifaddrs **);
void freeifaddrs(struct ifaddrs *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_IFADDRS_H */
