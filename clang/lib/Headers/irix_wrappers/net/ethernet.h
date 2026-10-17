/*===---- net/ethernet.h - IRIX wrapper ------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX has no <net/ethernet.h>, the BSD and glibc header for the Ethernet
 * frame (dnsmasq, darkstat, ...). IRIX keeps struct ether_header and its
 * ETHERTYPE_ names in <netinet/if_ether.h>: include that (with the
 * <net/if_arp.h> it needs for struct arphdr), then give the rest of the
 * BSD header -- struct ether_addr and the ETHER_*_LEN sizes -- and the
 * ETHERTYPE_ values IRIX's lacks.
 */

#ifndef __CLANG_IRIX_NET_ETHERNET_H
#define __CLANG_IRIX_NET_ETHERNET_H

#include <sys/types.h>
#include <sys/bsd_types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#define ETHER_ADDR_LEN 6
#define ETHER_TYPE_LEN 2
#define ETHER_CRC_LEN 4
#define ETHER_HDR_LEN (ETHER_ADDR_LEN * 2 + ETHER_TYPE_LEN)
#define ETHER_MIN_LEN 64
#define ETHER_MAX_LEN 1518

struct ether_addr {
  unsigned char ether_addr_octet[ETHER_ADDR_LEN];
};

#ifndef ETHERTYPE_SPRITE
#define ETHERTYPE_SPRITE 0x0500
#endif
#ifndef ETHERTYPE_REVARP
#define ETHERTYPE_REVARP 0x8035
#endif
#ifndef ETHERTYPE_AT
#define ETHERTYPE_AT 0x809B
#endif
#ifndef ETHERTYPE_AARP
#define ETHERTYPE_AARP 0x80F3
#endif
#ifndef ETHERTYPE_VLAN
#define ETHERTYPE_VLAN 0x8100
#endif
#ifndef ETHERTYPE_IPX
#define ETHERTYPE_IPX 0x8137
#endif
#ifndef ETHERTYPE_LOOPBACK
#define ETHERTYPE_LOOPBACK 0x9000
#endif

#endif /* __CLANG_IRIX_NET_ETHERNET_H */
