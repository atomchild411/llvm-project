//===-- irix/ifaddrs.c - getifaddrs for IRIX ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getifaddrs and freeifaddrs (the BSDs' and Linux's), which IRIX lacks, for
// clang's IRIX <ifaddrs.h>: the interface list from SIOCGIFCONF (IRIX's
// sockaddr has no length byte, so the list is a plain array of ifreq), and
// each one's flags, netmask and broadcast or point-to-point address from
// their own ioctls. IRIX has no IPv6 networking: the addresses are IPv4.
// Each entry is one allocation; freeifaddrs frees them in turn.
//
// Weak, so that a program's own copy wins.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <ifaddrs.h>
#include <net/if.h>
#include <net/soioctl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct ifaddrs_entry {
  struct ifaddrs ifa;
  char name[IFNAMSIZ + 1];
  struct sockaddr addr, netmask, dstaddr;
};

__attribute__((weak)) void freeifaddrs(struct ifaddrs *ifa) {
  while (ifa) {
    struct ifaddrs *next = ifa->ifa_next;
    free(ifa);
    ifa = next;
  }
}

__attribute__((weak)) int getifaddrs(struct ifaddrs **ifap) {
  struct ifconf ifc;
  struct ifaddrs *head = NULL, **tail = &head;
  char *buf = NULL, *p;
  int len = 16 * (int)sizeof(struct ifreq), s, err;

  *ifap = NULL;
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    return -1;
  /* Grow the buffer until the list fits with room to spare. */
  for (;;) {
    char *nbuf = realloc(buf, len);
    if (!nbuf)
      goto fail;
    buf = nbuf;
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;
    if (ioctl(s, SIOCGIFCONF, &ifc) < 0)
      goto fail;
    if (ifc.ifc_len + (int)sizeof(struct ifreq) <= len)
      break;
    len *= 2;
  }
  for (p = buf; p + sizeof(struct ifreq) <= buf + ifc.ifc_len;
       p += sizeof(struct ifreq)) {
    struct ifreq *ifr = (struct ifreq *)p, req;
    struct ifaddrs_entry *e = calloc(1, sizeof *e);
    if (!e)
      goto fail;
    memcpy(e->name, ifr->ifr_name, IFNAMSIZ);
    e->ifa.ifa_name = e->name;
    e->addr = ifr->ifr_addr;
    e->ifa.ifa_addr = &e->addr;
    memset(&req, 0, sizeof req);
    memcpy(req.ifr_name, ifr->ifr_name, IFNAMSIZ);
    if (ioctl(s, SIOCGIFFLAGS, &req) == 0)
      e->ifa.ifa_flags = req.ifr_flags;
    if (e->addr.sa_family == AF_INET) {
      if (ioctl(s, SIOCGIFNETMASK, &req) == 0) {
        e->netmask = req.ifr_addr;
        e->netmask.sa_family = AF_INET;
        e->ifa.ifa_netmask = &e->netmask;
      }
      if ((e->ifa.ifa_flags & IFF_BROADCAST) &&
          ioctl(s, SIOCGIFBRDADDR, &req) == 0) {
        e->dstaddr = req.ifr_broadaddr;
        e->ifa.ifa_dstaddr = &e->dstaddr;
      } else if ((e->ifa.ifa_flags & IFF_POINTOPOINT) &&
                 ioctl(s, SIOCGIFDSTADDR, &req) == 0) {
        e->dstaddr = req.ifr_dstaddr;
        e->ifa.ifa_dstaddr = &e->dstaddr;
      }
    }
    *tail = &e->ifa;
    tail = &e->ifa.ifa_next;
  }
  free(buf);
  close(s);
  *ifap = head;
  return 0;

fail:
  err = errno;
  freeifaddrs(head);
  free(buf);
  close(s);
  __irix_seterrno(err);
  return -1;
}

#endif // __sgi
