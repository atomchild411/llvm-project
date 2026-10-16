//===-- irix/ifnameindex.c - RFC 3493's interface indexes for IRIX --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// if_nameindex, if_freenameindex, if_nametoindex and if_indextoname, which
// IRIX declares only in its INET6 (IPv6 kit) builds and its libc lacks;
// clang's IRIX <net/if.h> declares them. The interfaces are SIOCGIFCONF's,
// one per name (the list has an entry per address), numbered from 1 in the
// kernel's order: the four agree with each other, and with getifaddrs
// (irix/ifaddrs.c).
//
// Weak, so that a program's own copy wins.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include "irix_errno.h"
#include <net/if.h>
#include <net/soioctl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef char ifname_t[IFNAMSIZ + 1];

// The interface names, each once, in SIOCGIFCONF's order: a malloc'd array,
// and their count; -1 (errno set) on failure.
static int list_names(ifname_t **namesp) {
  struct ifconf ifc;
  char *buf = NULL, *p;
  ifname_t *names = NULL;
  int len = 16 * (int)sizeof(struct ifreq), s, n = 0, i, e;

  *namesp = NULL;
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    return -1;
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
  names = calloc(ifc.ifc_len / sizeof(struct ifreq) + 1, sizeof *names);
  if (!names)
    goto fail;
  for (p = buf; p + sizeof(struct ifreq) <= buf + ifc.ifc_len;
       p += sizeof(struct ifreq)) {
    const struct ifreq *ifr = (const struct ifreq *)p;
    ifname_t name;
    memcpy(name, ifr->ifr_name, IFNAMSIZ);
    name[IFNAMSIZ] = '\0';
    for (i = 0; i < n && strcmp(names[i], name) != 0; ++i)
      ;
    if (i == n)
      strcpy(names[n++], name);
  }
  close(s);
  free(buf);
  *namesp = names;
  return n;
fail:
  e = errno;
  close(s);
  free(buf);
  free(names);
  __irix_seterrno(e);
  return -1;
}

__attribute__((weak)) struct if_nameindex *if_nameindex(void) {
  ifname_t *names;
  struct if_nameindex *r;
  char *str;
  int n = list_names(&names), i;

  if (n < 0)
    return NULL;
  // One allocation, for if_freenameindex: the entries, then the names.
  r = malloc((n + 1) * sizeof *r + n * sizeof(ifname_t));
  if (!r) {
    free(names);
    __irix_seterrno(ENOBUFS);
    return NULL;
  }
  str = (char *)(r + n + 1);
  for (i = 0; i < n; ++i) {
    r[i].if_index = i + 1;
    r[i].if_name = strcpy(str + i * sizeof(ifname_t), names[i]);
  }
  r[n].if_index = 0;
  r[n].if_name = NULL;
  free(names);
  return r;
}

__attribute__((weak)) void if_freenameindex(struct if_nameindex *ptr) {
  free(ptr);
}

__attribute__((weak)) unsigned int if_nametoindex(const char *name) {
  ifname_t *names;
  int n = list_names(&names), i;
  unsigned int r = 0;

  if (n < 0)
    return 0;
  for (i = 0; i < n; ++i)
    if (strcmp(names[i], name) == 0) {
      r = i + 1;
      break;
    }
  free(names);
  if (!r)
    __irix_seterrno(ENXIO);
  return r;
}

// name has room for IF_NAMESIZE (IFNAMSIZ) bytes, the terminator included.
__attribute__((weak)) char *if_indextoname(unsigned int index, char *name) {
  ifname_t *names;
  int n = list_names(&names);
  char *r = NULL;

  if (n < 0)
    return NULL;
  if (index >= 1 && index <= (unsigned int)n) {
    strncpy(name, names[index - 1], IFNAMSIZ - 1);
    name[IFNAMSIZ - 1] = '\0';
    r = name;
  } else {
    __irix_seterrno(ENXIO);
  }
  free(names);
  return r;
}

#endif // defined(__sgi)
