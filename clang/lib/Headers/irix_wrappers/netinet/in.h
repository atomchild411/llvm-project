/*===---- netinet/in.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3493's IPv6 types and names, for code that handles IPv6 addresses
 * whether or not it ever opens an IPv6 socket (GLib's GIO). IRIX declares an
 * early draft's versions, and only under INET6: a sockaddr_in6 with a length
 * byte and no sin6_scope_id. Without INET6, these standard ones are declared
 * instead: in6_addr, sockaddr_in6 (with sin6_scope_id, and IRIX's 16-bit
 * address family first, as in its sockaddr), in6addr_any and
 * in6addr_loopback (defined by compiler-rt's IRIX builtins, irix/netdb.c),
 * the IN6_IS_ADDR_* tests and IN6_ARE_ADDR_EQUAL, ipv6_mreq and the IPV6_*
 * option names. They make code compile and let it parse and print IPv6
 * addresses; they do not give IRIX IPv6 networking. INET_ADDRSTRLEN, which
 * IRIX also keeps under INET6, is defined either way.
 */

#ifndef __CLANG_IRIX_NETINET_IN_H
#define __CLANG_IRIX_NETINET_IN_H

#include_next <netinet/in.h>

/* POSIX's IPv4 constant, which IRIX defines only under INET6. */
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#if !defined(INET6) && !defined(IN6ADDR_ANY_INIT)
#include <sys/socket.h>

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

struct in6_addr {
  union {
    unsigned char __u6_addr8[16];
    unsigned short __u6_addr16[8];
    unsigned int __u6_addr32[4];
  } __u6_addr;
};
#define s6_addr __u6_addr.__u6_addr8

struct sockaddr_in6 {
  sa_family_t sin6_family;
  in_port_t sin6_port;
  unsigned int sin6_flowinfo;
  struct in6_addr sin6_addr;
  unsigned int sin6_scope_id;
};

#define IN6ADDR_ANY_INIT                                                       \
  {                                                                            \
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }                     \
  }
#define IN6ADDR_LOOPBACK_INIT                                                  \
  {                                                                            \
    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } }                     \
  }
#ifdef __cplusplus
extern "C" {
#endif
extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;
#ifdef __cplusplus
}
#endif

#define IN6_IS_ADDR_UNSPECIFIED(a)                                             \
  ((a)->__u6_addr.__u6_addr32[0] == 0 && (a)->__u6_addr.__u6_addr32[1] == 0 && \
   (a)->__u6_addr.__u6_addr32[2] == 0 && (a)->__u6_addr.__u6_addr32[3] == 0)
#define IN6_IS_ADDR_LOOPBACK(a)                                                \
  ((a)->__u6_addr.__u6_addr32[0] == 0 && (a)->__u6_addr.__u6_addr32[1] == 0 && \
   (a)->__u6_addr.__u6_addr32[2] == 0 && (a)->s6_addr[12] == 0 &&             \
   (a)->s6_addr[13] == 0 && (a)->s6_addr[14] == 0 && (a)->s6_addr[15] == 1)
#define IN6_IS_ADDR_V4COMPAT(a)                                                \
  ((a)->__u6_addr.__u6_addr32[0] == 0 && (a)->__u6_addr.__u6_addr32[1] == 0 && \
   (a)->__u6_addr.__u6_addr32[2] == 0 && !IN6_IS_ADDR_UNSPECIFIED(a) &&       \
   !IN6_IS_ADDR_LOOPBACK(a))
#define IN6_IS_ADDR_V4MAPPED(a)                                                \
  ((a)->__u6_addr.__u6_addr32[0] == 0 && (a)->__u6_addr.__u6_addr32[1] == 0 && \
   (a)->s6_addr[8] == 0 && (a)->s6_addr[9] == 0 && (a)->s6_addr[10] == 0xff && \
   (a)->s6_addr[11] == 0xff)
#define IN6_IS_ADDR_LINKLOCAL(a)                                               \
  ((a)->s6_addr[0] == 0xfe && ((a)->s6_addr[1] & 0xc0) == 0x80)
#define IN6_IS_ADDR_SITELOCAL(a)                                               \
  ((a)->s6_addr[0] == 0xfe && ((a)->s6_addr[1] & 0xc0) == 0xc0)
#define IN6_IS_ADDR_MULTICAST(a) ((a)->s6_addr[0] == 0xff)
#define __IRIX_IN6_MC_SCOPE(a) ((a)->s6_addr[1] & 0x0f)
#define IN6_IS_ADDR_MC_NODELOCAL(a)                                            \
  (IN6_IS_ADDR_MULTICAST(a) && __IRIX_IN6_MC_SCOPE(a) == 0x1)
#define IN6_IS_ADDR_MC_LINKLOCAL(a)                                            \
  (IN6_IS_ADDR_MULTICAST(a) && __IRIX_IN6_MC_SCOPE(a) == 0x2)
#define IN6_IS_ADDR_MC_SITELOCAL(a)                                            \
  (IN6_IS_ADDR_MULTICAST(a) && __IRIX_IN6_MC_SCOPE(a) == 0x5)
#define IN6_IS_ADDR_MC_ORGLOCAL(a)                                             \
  (IN6_IS_ADDR_MULTICAST(a) && __IRIX_IN6_MC_SCOPE(a) == 0x8)
#define IN6_IS_ADDR_MC_GLOBAL(a)                                               \
  (IN6_IS_ADDR_MULTICAST(a) && __IRIX_IN6_MC_SCOPE(a) == 0xe)
#define IN6_ARE_ADDR_EQUAL(a, b)                                               \
  ((a)->__u6_addr.__u6_addr32[0] == (b)->__u6_addr.__u6_addr32[0] &&           \
   (a)->__u6_addr.__u6_addr32[1] == (b)->__u6_addr.__u6_addr32[1] &&           \
   (a)->__u6_addr.__u6_addr32[2] == (b)->__u6_addr.__u6_addr32[2] &&           \
   (a)->__u6_addr.__u6_addr32[3] == (b)->__u6_addr.__u6_addr32[3])

struct ipv6_mreq {
  struct in6_addr ipv6mr_multiaddr;
  unsigned int ipv6mr_interface;
};

/* Option names only: IRIX has no IPv6 sockets for them to act on. */
#ifndef IPV6_UNICAST_HOPS
#define IPV6_UNICAST_HOPS 4
#define IPV6_MULTICAST_IF 9
#define IPV6_MULTICAST_HOPS 10
#define IPV6_MULTICAST_LOOP 11
#define IPV6_JOIN_GROUP 12
#define IPV6_LEAVE_GROUP 13
#define IPV6_V6ONLY 27
#define IPV6_TCLASS 61
#endif
#endif /* !INET6 */

#endif /* __CLANG_IRIX_NETINET_IN_H */
