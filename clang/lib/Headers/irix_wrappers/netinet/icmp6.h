/*===---- netinet/icmp6.h - IRIX: ICMPv6 headers ----------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3542's ICMPv6 header and message types, which IRIX has no
 * <netinet/icmp6.h> for: the wire format, laid out as the BSDs and glibc lay
 * it out (big-endian values), for code that builds or parses ICMPv6 (squid's
 * pinger). As with <netinet/ip6.h>, IRIX gives no IPv6 networking with it.
 */

#ifndef __CLANG_IRIX_NETINET_ICMP6_H
#define __CLANG_IRIX_NETINET_ICMP6_H

#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

struct icmp6_hdr {
  uint8_t icmp6_type;
  uint8_t icmp6_code;
  uint16_t icmp6_cksum;
  union {
    uint32_t icmp6_un_data32[1];
    uint16_t icmp6_un_data16[2];
    uint8_t icmp6_un_data8[4];
  } icmp6_dataun;
};
#define icmp6_data32 icmp6_dataun.icmp6_un_data32
#define icmp6_data16 icmp6_dataun.icmp6_un_data16
#define icmp6_data8 icmp6_dataun.icmp6_un_data8
#define icmp6_pptr icmp6_data32[0]
#define icmp6_mtu icmp6_data32[0]
#define icmp6_id icmp6_data16[0]
#define icmp6_seq icmp6_data16[1]
#define icmp6_maxdelay icmp6_data16[0]

#define ICMP6_DST_UNREACH 1
#define ICMP6_PACKET_TOO_BIG 2
#define ICMP6_TIME_EXCEEDED 3
#define ICMP6_PARAM_PROB 4
#define ICMP6_INFOMSG_MASK 0x80
#define ICMP6_ECHO_REQUEST 128
#define ICMP6_ECHO_REPLY 129
#define MLD_LISTENER_QUERY 130
#define MLD_LISTENER_REPORT 131
#define MLD_LISTENER_REDUCTION 132
#define ND_ROUTER_SOLICIT 133
#define ND_ROUTER_ADVERT 134
#define ND_NEIGHBOR_SOLICIT 135
#define ND_NEIGHBOR_ADVERT 136
#define ND_REDIRECT 137

#define ICMP6_DST_UNREACH_NOROUTE 0
#define ICMP6_DST_UNREACH_ADMIN 1
#define ICMP6_DST_UNREACH_BEYONDSCOPE 2
#define ICMP6_DST_UNREACH_ADDR 3
#define ICMP6_DST_UNREACH_NOPORT 4
#define ICMP6_TIME_EXCEED_TRANSIT 0
#define ICMP6_TIME_EXCEED_REASSEMBLY 1
#define ICMP6_PARAMPROB_HEADER 0
#define ICMP6_PARAMPROB_NEXTHEADER 1
#define ICMP6_PARAMPROB_OPTION 2

#endif /* __CLANG_IRIX_NETINET_ICMP6_H */
