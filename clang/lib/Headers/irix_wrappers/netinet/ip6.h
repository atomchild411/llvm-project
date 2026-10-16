/*===---- netinet/ip6.h - IRIX: IPv6 headers --------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3542's IPv6 packet headers, which IRIX has no <netinet/ip6.h> for: the
 * wire formats, laid out as the BSDs and glibc lay them out (big-endian
 * values), for code that parses or builds IPv6 packets (haproxy, iftop,
 * dnstop, dnscap). IRIX gives no IPv6 networking with them.
 */

#ifndef __CLANG_IRIX_NETINET_IP6_H
#define __CLANG_IRIX_NETINET_IP6_H

#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

struct ip6_hdr {
  union {
    struct ip6_hdrctl {
      uint32_t ip6_un1_flow; /* 4 bits version, 8 traffic class, 20 flow label */
      uint16_t ip6_un1_plen; /* payload length */
      uint8_t ip6_un1_nxt;   /* next header */
      uint8_t ip6_un1_hlim;  /* hop limit */
    } ip6_un1;
    uint8_t ip6_un2_vfc; /* 4 bits version, top 4 bits traffic class */
  } ip6_ctlun;
  struct in6_addr ip6_src;
  struct in6_addr ip6_dst;
};
#define ip6_vfc ip6_ctlun.ip6_un2_vfc
#define ip6_flow ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops ip6_ctlun.ip6_un1.ip6_un1_hlim

#define IPV6_VERSION 0x60
#define IPV6_VERSION_MASK 0xf0
#define IPV6_FLOWINFO_MASK 0x0fffffff
#define IPV6_FLOWLABEL_MASK 0x000fffff

struct ip6_ext {
  uint8_t ip6e_nxt;
  uint8_t ip6e_len;
};

struct ip6_hbh {
  uint8_t ip6h_nxt;
  uint8_t ip6h_len;
};

struct ip6_dest {
  uint8_t ip6d_nxt;
  uint8_t ip6d_len;
};

struct ip6_rthdr {
  uint8_t ip6r_nxt;
  uint8_t ip6r_len;
  uint8_t ip6r_type;
  uint8_t ip6r_segleft;
};

struct ip6_rthdr0 {
  uint8_t ip6r0_nxt;
  uint8_t ip6r0_len;
  uint8_t ip6r0_type;
  uint8_t ip6r0_segleft;
  uint8_t ip6r0_reserved;
  uint8_t ip6r0_slmap[3];
  struct in6_addr ip6r0_addr[1];
};

struct ip6_frag {
  uint8_t ip6f_nxt;
  uint8_t ip6f_reserved;
  uint16_t ip6f_offlg; /* offset, reserved, and flag */
  uint32_t ip6f_ident;
};
#define IP6F_OFF_MASK 0xfff8
#define IP6F_RESERVED_MASK 0x0006
#define IP6F_MORE_FRAG 0x0001

struct ip6_opt {
  uint8_t ip6o_type;
  uint8_t ip6o_len;
};
#define IP6OPT_TYPE(o) ((o) & 0xc0)
#define IP6OPT_TYPE_SKIP 0x00
#define IP6OPT_TYPE_DISCARD 0x40
#define IP6OPT_TYPE_FORCEICMP 0x80
#define IP6OPT_TYPE_ICMP 0xc0
#define IP6OPT_MUTABLE 0x20
#define IP6OPT_PAD1 0x00
#define IP6OPT_PADN 0x01
#define IP6OPT_JUMBO 0xc2
#define IP6OPT_NSAP_ADDR 0xc3
#define IP6OPT_TUNNEL_LIMIT 0x04
#define IP6OPT_ROUTER_ALERT 0x05
#define IP6OPT_JUMBO_LEN 6

#endif /* __CLANG_IRIX_NETINET_IP6_H */
