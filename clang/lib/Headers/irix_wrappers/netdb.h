/*===---- netdb.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3493's getaddrinfo, freeaddrinfo, getnameinfo and gai_strerror, with
 * struct addrinfo and the AI_, NI_ and EAI_ names, which no IRIX release has
 * (it has gethostbyname and friends). compiler-rt's IRIX builtins define
 * them (irix/netdb.c) for IPv4, over IRIX's reentrant gethostbyname_r,
 * gethostbyaddr_r and getservbyname_r; IPv6 addresses are handled only as
 * numbers (parsed and printed), since IRIX has no IPv6 networking.
 */

#ifndef __CLANG_IRIX_NETDB_H
#define __CLANG_IRIX_NETDB_H

#include_next <netdb.h>

#ifndef AI_PASSIVE
#include <sys/socket.h>

struct addrinfo {
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  socklen_t ai_addrlen;
  struct sockaddr *ai_addr;
  char *ai_canonname;
  struct addrinfo *ai_next;
};

#define AI_PASSIVE 0x0001
#define AI_CANONNAME 0x0002
#define AI_NUMERICHOST 0x0004
#define AI_NUMERICSERV 0x0008
#define AI_V4MAPPED 0x0010
#define AI_ALL 0x0020
#define AI_ADDRCONFIG 0x0040

#define NI_NOFQDN 0x0001
#define NI_NUMERICHOST 0x0002
#define NI_NAMEREQD 0x0004
#define NI_NUMERICSERV 0x0008
#define NI_DGRAM 0x0010
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

/* EAI_ADDRFAMILY is BSD's 1: never returned here, but programs compare
 * against it (ruby). */
#define EAI_ADDRFAMILY 1
#define EAI_AGAIN 2
#define EAI_BADFLAGS 3
#define EAI_FAIL 4
#define EAI_FAMILY 5
#define EAI_MEMORY 6
#define EAI_NODATA 7
#define EAI_NONAME 8
#define EAI_SERVICE 9
#define EAI_SOCKTYPE 10
#define EAI_SYSTEM 11
#define EAI_OVERFLOW 14

#ifdef __cplusplus
extern "C" {
#endif
int getaddrinfo(const char *, const char *, const struct addrinfo *,
                struct addrinfo **);
void freeaddrinfo(struct addrinfo *);
const char *gai_strerror(int);
int getnameinfo(const struct sockaddr *, socklen_t, char *, socklen_t, char *,
                socklen_t, int);
#ifdef __cplusplus
}
#endif
#endif

/* BSD's, in IRIX's libc, which its header declares only in SGI mode:
 * declared here outside it, with IRIX's own prototypes (glibc gives them
 * with _DEFAULT_SOURCE; Python, among others, uses them). */
#if !_SGIAPI
struct hostent;
struct servent;
#ifdef __cplusplus
extern "C" {
#endif
struct hostent *gethostbyaddr_r(const void *, size_t, int, struct hostent *,
                                char *, int, int *);
struct hostent *gethostbyname_r(const char *, struct hostent *, char *, int,
                                int *);
struct servent *getservbyname_r(const char *, const char *, struct servent *,
                                char *, int);
void herror(const char *);
char *hstrerror(int);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_NETDB_H */
