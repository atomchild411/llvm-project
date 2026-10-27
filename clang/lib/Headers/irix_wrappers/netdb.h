/*===---- netdb.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * RFC 3493's getaddrinfo, freeaddrinfo, getnameinfo and gai_strerror.
 * IRIX 6.5.22's libc has them, but <netdb.h> declares them, struct addrinfo
 * and the AI_, NI_ and EAI_ names only outside X/Open modes: here they are
 * otherwise, with IRIX's own layout and values, so that code built in
 * either mode agrees.
 *
 * The functions every object calls are compiler-rt's (irix/netdb.c: IPv4
 * over IRIX's reentrant gethostbyname_r, gethostbyaddr_r and
 * getservbyname_r, IPv6 addresses as numbers only), under their own names.
 * libc's reject a numeric service with no socket type, and they cannot be
 * mixed with a stand-in of the same name: rld binds a module loaded with
 * dlopen (Python's _socket, Perl's Socket) to libc's copy, loaded first,
 * not its own, so a program took libc's getaddrinfo and the stand-in's
 * freeaddrinfo, or the other way round.
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
  char *ai_canonname;
  struct sockaddr *ai_addr;
  struct addrinfo *ai_next;
};

#define AI_PASSIVE 0x00000001
#define AI_CANONNAME 0x00000002
#define AI_NUMERICHOST 0x00000004
#define AI_NUMERICSERV 0x00000008
#define AI_ALL 0x00000100
#define AI_ADDRCONFIG 0x00000400
#define AI_V4MAPPED 0x00000800
#define AI_DEFAULT (AI_V4MAPPED | AI_ADDRCONFIG)

#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define NI_NOFQDN 0x00000001
#define NI_NUMERICHOST 0x00000002
#define NI_NAMEREQD 0x00000004
#define NI_NUMERICSERV 0x00000008
#define NI_DGRAM 0x00000010
#define NI_WITHSCOPEID 0x00000020

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
#define EAI_BADHINTS 12
#define EAI_OVERFLOW 13
#define EAI_MAX 14
#endif

#define getaddrinfo __irix_getaddrinfo
#define freeaddrinfo __irix_freeaddrinfo
#define gai_strerror __irix_gai_strerror
#define getnameinfo __irix_getnameinfo

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

/* BIND's gethostbyname2, which IRIX lacks: compiler-rt's irix/netdb.c. */
#ifdef __cplusplus
extern "C" {
#endif
struct hostent *gethostbyname2(const char *, int);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_NETDB_H */
