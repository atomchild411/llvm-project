//===-- irix/netdb.c - getaddrinfo and friends for IRIX -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// RFC 3493's getaddrinfo, freeaddrinfo, getnameinfo and gai_strerror, which
// no IRIX release has, and the rest of that RFC's names the IRIX wrapper
// headers declare (<netdb.h>, <netinet/in.h>, <net/if.h>): in6addr_any,
// in6addr_loopback, if_nametoindex and if_indextoname.
//
// Names resolve over IRIX's reentrant gethostbyname_r and gethostbyaddr_r,
// services over getservbyname_r and getservbyport_r: IPv4 only, as IRIX
// resolves. IPv6 addresses are handled as numbers only -- parsed with
// inet_pton and printed with inet_ntop, which IRIX's libc does for IPv6 --
// since IRIX has no IPv6 networking; a name never resolves to one, and there
// are no interfaces to name in a scope id.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <arpa/inet.h>
#include <errno.h>
#include "irix_errno.h"
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

// Weak: a program that brings its own copy (a compat/ directory) keeps it,
// while still getting the rest of this file.
#pragma weak freeaddrinfo
#pragma weak getaddrinfo
#pragma weak gai_strerror
#pragma weak getnameinfo
#pragma weak if_nametoindex
#pragma weak if_indextoname


extern int inet_pton(int, const char *, void *);
extern const char *inet_ntop(int, const void *, char *, size_t);

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

#define KNOWN_AI_FLAGS                                                         \
  (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_NUMERICSERV | AI_V4MAPPED | \
   AI_ALL | AI_ADDRCONFIG)

// A resolver's h_errno as an EAI_ code.
static int eai_from_h_errno(int h) {
  switch (h) {
  case HOST_NOT_FOUND:
    return EAI_NONAME;
  case TRY_AGAIN:
    return EAI_AGAIN;
  case NO_DATA:
    return EAI_NODATA;
  default:
    return EAI_FAIL;
  }
}

// One result, with its sockaddr and canonical name in the same allocation.
static struct addrinfo *new_addrinfo(int family, int socktype, int protocol,
                                     const void *addr, unsigned short port,
                                     const char *canon) {
  size_t salen = family == AF_INET ? sizeof(struct sockaddr_in)
                                   : sizeof(struct sockaddr_in6);
  size_t clen = canon ? strlen(canon) + 1 : 0;
  struct addrinfo *ai =
      (struct addrinfo *)calloc(1, sizeof(struct addrinfo) + salen + clen);
  if (!ai)
    return 0;
  ai->ai_family = family;
  ai->ai_socktype = socktype;
  ai->ai_protocol = protocol;
  ai->ai_addrlen = (socklen_t)salen;
  ai->ai_addr = (struct sockaddr *)(ai + 1);
  if (family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
    sin->sin_family = AF_INET;
    sin->sin_port = port;
    memcpy(&sin->sin_addr, addr, sizeof sin->sin_addr);
  } else {
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ai->ai_addr;
    sin6->sin6_family = AF_INET6;
    sin6->sin6_port = port;
    memcpy(&sin6->sin6_addr, addr, sizeof sin6->sin6_addr);
  }
  if (canon) {
    ai->ai_canonname = (char *)ai->ai_addr + salen;
    memcpy(ai->ai_canonname, canon, clen);
  }
  return ai;
}

void freeaddrinfo(struct addrinfo *ai) {
  while (ai) {
    struct addrinfo *next = ai->ai_next;
    free(ai);
    ai = next;
  }
}

// The port for serv, in network byte order.
static int service_port(const char *serv, int socktype, int flags,
                        unsigned short *port) {
  char *end;
  unsigned long n;
  struct servent se, *sp = 0;
  char buf[1024];

  *port = 0;
  if (!serv || !*serv)
    return 0;
  n = strtoul(serv, &end, 10);
  if (*end == 0) {
    if (n > 65535)
      return EAI_SERVICE;
    *port = htons((unsigned short)n);
    return 0;
  }
  if (flags & AI_NUMERICSERV)
    return EAI_NONAME;
  if (socktype != SOCK_DGRAM)
    sp = getservbyname_r(serv, "tcp", &se, buf, sizeof buf);
  if (!sp && socktype != SOCK_STREAM)
    sp = getservbyname_r(serv, "udp", &se, buf, sizeof buf);
  if (!sp)
    return EAI_SERVICE;
  *port = (unsigned short)se.s_port;
  return 0;
}

int getaddrinfo(const char *node, const char *serv,
                const struct addrinfo *hints, struct addrinfo **res) {
  int flags = 0, family = AF_UNSPEC, socktype = 0, protocol = 0;
  int types[2], protos[2], ntypes, naddrs = 0, i, j, err;
  unsigned short port;
  unsigned char addrs[64][16];
  int addr_family = AF_INET;
  const char *canon = 0;
  struct addrinfo *head = 0, **tail = &head;
  struct hostent he, *hp = 0;
  char buf[8192];

  if (!res)
    return EAI_FAIL;
  *res = 0;
  if (hints) {
    flags = hints->ai_flags;
    family = hints->ai_family;
    socktype = hints->ai_socktype;
    protocol = hints->ai_protocol;
  }
  if (!node && !serv)
    return EAI_NONAME;
  if (flags & ~KNOWN_AI_FLAGS)
    return EAI_BADFLAGS;
  if (family != AF_UNSPEC && family != AF_INET && family != AF_INET6)
    return EAI_FAMILY;
  switch (socktype) {
  case 0:
    types[0] = SOCK_STREAM, protos[0] = IPPROTO_TCP;
    types[1] = SOCK_DGRAM, protos[1] = IPPROTO_UDP;
    ntypes = 2;
    break;
  case SOCK_STREAM:
  case SOCK_DGRAM:
  case SOCK_RAW:
    types[0] = socktype;
    protos[0] = protocol ? protocol
                         : socktype == SOCK_STREAM ? IPPROTO_TCP
                         : socktype == SOCK_DGRAM  ? IPPROTO_UDP
                                                   : 0;
    ntypes = 1;
    break;
  default:
    return EAI_SOCKTYPE;
  }
  if ((err = service_port(serv, socktype, flags, &port)) != 0)
    return err;

  if (!node) {
    // The wildcard address to bind, or the loopback address to reach.
    if (family == AF_INET6) {
      addr_family = AF_INET6;
      memcpy(addrs[0], (flags & AI_PASSIVE) ? &in6addr_any : &in6addr_loopback,
             16);
    } else {
      struct in_addr a;
      a.s_addr = htonl((flags & AI_PASSIVE) ? INADDR_ANY : INADDR_LOOPBACK);
      memcpy(addrs[0], &a, 4);
    }
    naddrs = 1;
  } else if (inet_pton(AF_INET, node, addrs[0]) == 1) {
    if (family == AF_INET6)
      return EAI_NONAME;
    naddrs = 1;
  } else if (inet_pton(AF_INET6, node, addrs[0]) == 1) {
    if (family == AF_INET)
      return EAI_NONAME;
    addr_family = AF_INET6;
    naddrs = 1;
  } else if ((flags & AI_NUMERICHOST) || family == AF_INET6) {
    return EAI_NONAME;
  } else {
    int herr = 0;
    hp = gethostbyname_r(node, &he, buf, (int)sizeof buf, &herr);
    if (!hp)
      return eai_from_h_errno(herr);
    if (hp->h_addrtype != AF_INET || hp->h_length != 4)
      return EAI_NODATA;
    for (i = 0; hp->h_addr_list[i] && naddrs < 64; i++)
      memcpy(addrs[naddrs++], hp->h_addr_list[i], 4);
    if (!naddrs)
      return EAI_NODATA;
  }
  if (flags & AI_CANONNAME)
    canon = hp ? hp->h_name : node;

  for (i = 0; i < naddrs; i++)
    for (j = 0; j < ntypes; j++) {
      struct addrinfo *ai = new_addrinfo(addr_family, types[j], protos[j],
                                         addrs[i], port, canon);
      if (!ai) {
        freeaddrinfo(head);
        return EAI_MEMORY;
      }
      canon = 0; // the first result carries it
      *tail = ai;
      tail = &ai->ai_next;
    }
  *res = head;
  return 0;
}

const char *gai_strerror(int code) {
  switch (code) {
  case 0:
    return "Success";
  case EAI_AGAIN:
    return "Temporary failure in name resolution";
  case EAI_BADFLAGS:
    return "Invalid value for ai_flags";
  case EAI_FAIL:
    return "Non-recoverable failure in name resolution";
  case EAI_FAMILY:
    return "Address family not supported";
  case EAI_MEMORY:
    return "Memory allocation failure";
  case EAI_NODATA:
    return "No address associated with name";
  case EAI_NONAME:
    return "Name or service not known";
  case EAI_SERVICE:
    return "Service not supported for socket type";
  case EAI_SOCKTYPE:
    return "Socket type not supported";
  case EAI_SYSTEM:
    return "System error";
  case EAI_OVERFLOW:
    return "Argument buffer overflow";
  default:
    return "Unknown error";
  }
}

static int copy_out(char *dst, socklen_t len, const char *src) {
  size_t n = strlen(src);
  if (n >= (size_t)len)
    return EAI_OVERFLOW;
  memcpy(dst, src, n + 1);
  return 0;
}

int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
                socklen_t hostlen, char *serv, socklen_t servlen, int flags) {
  const void *addr;
  unsigned short port;
  int family, err;

  if (!sa)
    return EAI_FAIL;
  family = sa->sa_family;
  if (family == AF_INET && salen >= (socklen_t)sizeof(struct sockaddr_in)) {
    const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;
    addr = &sin->sin_addr;
    port = sin->sin_port;
  } else if (family == AF_INET6 &&
             salen >= (socklen_t)sizeof(struct sockaddr_in6)) {
    const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)sa;
    addr = &sin6->sin6_addr;
    port = sin6->sin6_port;
  } else {
    return EAI_FAMILY;
  }

  if (host && hostlen) {
    int named = 0;
    if (family == AF_INET && !(flags & NI_NUMERICHOST)) {
      struct hostent he, *hp;
      char buf[8192];
      int herr = 0;
      hp = gethostbyaddr_r(addr, 4, AF_INET, &he, buf, (int)sizeof buf, &herr);
      if (hp && hp->h_name) {
        if (flags & NI_NOFQDN) {
          char *dot = strchr(hp->h_name, '.');
          if (dot)
            *dot = 0;
        }
        if ((err = copy_out(host, hostlen, hp->h_name)) != 0)
          return err;
        named = 1;
      }
    }
    if (!named) {
      char num[INET6_ADDRSTRLEN];
      if ((flags & NI_NAMEREQD) && !(flags & NI_NUMERICHOST))
        return EAI_NONAME;
      if (!inet_ntop(family, addr, num, sizeof num))
        return EAI_SYSTEM;
      if ((err = copy_out(host, hostlen, num)) != 0)
        return err;
    }
  }

  if (serv && servlen) {
    char num[16];
    if (!(flags & NI_NUMERICSERV)) {
      struct servent se, *sp;
      char buf[1024];
      sp = getservbyport_r(port, (flags & NI_DGRAM) ? "udp" : "tcp", &se, buf,
                           (int)sizeof buf);
      if (sp && sp->s_name)
        return copy_out(serv, servlen, sp->s_name);
    }
    snprintf(num, sizeof num, "%u", (unsigned)ntohs(port));
    if ((err = copy_out(serv, servlen, num)) != 0)
      return err;
  }
  return 0;
}

// IPv6 scope ids name interfaces; IRIX has no IPv6 to scope.
unsigned int if_nametoindex(const char *name) {
  (void)name;
  __irix_seterrno(ENXIO);
  return 0;
}

char *if_indextoname(unsigned int index, char *name) {
  (void)index;
  (void)name;
  __irix_seterrno(ENXIO);
  return 0;
}

#endif // defined(__sgi)
