/*===---- netinet/udp.h - IRIX wrapper --------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's header has no include guard: a second #include redefines
 * struct udphdr (configure scripts that include it among the prerequisites of
 * another check, as net-snmp's does, then decide it is not there).
 */

#ifndef __CLANG_IRIX_NETINET_UDP_H
#define __CLANG_IRIX_NETINET_UDP_H

#include_next <netinet/udp.h>

#endif /* __CLANG_IRIX_NETINET_UDP_H */
