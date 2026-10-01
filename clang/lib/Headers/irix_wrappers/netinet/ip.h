/*===---- netinet/ip.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <netinet/ip.h>, like 4.3BSD's, uses n_long and n_short without
 * including <netinet/in_systm.h>, which defines them, or <netinet/in.h>.
 * glibc's and today's BSDs' are self-contained (dbus includes it alone).
 */
#ifndef __CLANG_IRIX_NETINET_IP_H
#define __CLANG_IRIX_NETINET_IP_H
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include_next <netinet/ip.h>
#endif /* __CLANG_IRIX_NETINET_IP_H */
