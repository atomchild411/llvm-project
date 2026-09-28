/*===---- sys/socket.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * socklen_t: IRIX 6.5.22's <sys/socket.h> defines it, behind _SOCKLEN_T, to
 * the type its socket calls take. 6.5.7's has no socklen_t at all; its calls
 * take int * in IRIX's own API (_NO_XOPEN4, which XPG5 alone does not turn
 * off there) and size_t * under XPG4. When the system header has not
 * defined it -- only 6.5.7's -- define it to match 6.5.7's prototypes.
 *
 * struct sockaddr_storage: POSIX's socket address big and aligned enough for
 * any family; no IRIX release declares it (OpenSSL, among many, uses it with
 * or without IPv6). IRIX's struct sockaddr starts with a 16-bit sa_family_t
 * and has no length byte, so ss_family sits where sa_family does.
 *
 * struct msghdr: IRIX's own API (_SGIAPI, the default) gives BSD 4.3's, whose
 * msg_control is a #define for msg_accrights (a plain array of descriptors)
 * and which has no msg_flags; the POSIX one, with control messages
 * (cmsghdr, SCM_RIGHTS), is IRIX's X/Open msghdr, which only a strict X/Open
 * compilation sees, through __xpg4_sendmsg and __xpg4_recvmsg. Code written
 * since (GLib's GIO, OpenSSH, tmux) wants the POSIX one, so it is the one
 * declared here in every mode: IRIX's declarations are renamed out of the way
 * while its header is read, and sendmsg and recvmsg are bound to IRIX's X/Open
 * entry points, which pass descriptors with SCM_RIGHTS (recvmsg through
 * getsockname and getpeername: for an unnamed AF_UNIX socket (either end
 * of a socketpair) IRIX succeeds with a length of 0 and no family; Linux
 * and the BSDs return the family alone, and GLib's GSocket needs it. On n32
 * both are bound to compiler-rt's irix/socket.c, which reports AF_UNIX then.
 *
 * compiler-rt's irix/socket.c, which clears a bit IRIX's kernel leaves in
 * msg_flags that no MSG_ flag names). CMSG_SPACE, CMSG_LEN
 * and SCM_RIGHTS are defined where IRIX hides them.
 */

#ifndef __CLANG_IRIX_SYS_SOCKET_H
#define __CLANG_IRIX_SYS_SOCKET_H

#define msghdr __irix_bsd43_msghdr
#define sendmsg __irix_bsd43_sendmsg
#define recvmsg __irix_bsd43_recvmsg
#define getsockname __irix_libc_getsockname
#define getpeername __irix_libc_getpeername
#include_next <sys/socket.h>
#undef msghdr
#undef sendmsg
#undef recvmsg
#undef getsockname
#undef getpeername
#undef msg_control
#undef msg_controllen

struct msghdr {
  void *msg_name;
  size_t msg_namelen;
  struct iovec *msg_iov;
  int msg_iovlen;
  void *msg_control;
  size_t msg_controllen;
  int msg_flags;
};

#ifdef __cplusplus
extern "C" {
#endif
ssize_t sendmsg(int, const struct msghdr *, int) __asm__("__xpg4_sendmsg");
ssize_t recvmsg(int, struct msghdr *, int) __asm__("__irix_recvmsg");
#ifdef __cplusplus
}
#endif

#ifndef CMSG_LEN
/* As IRIX's own (under INET6): no padding. Its kernel refuses a control
 * buffer longer than the messages in it (EINVAL). */
#define CMSG_LEN(length) (sizeof(struct cmsghdr) + (length))
#define CMSG_SPACE(length) (sizeof(struct cmsghdr) + (length))
#endif
#ifndef SCM_RIGHTS
#define SCM_RIGHTS 0x01
#endif

#ifndef _SOCKLEN_T
#define _SOCKLEN_T
#if _NO_XOPEN4
typedef int socklen_t;
#else
typedef size_t socklen_t;
#endif
#endif

#if _MIPS_SZLONG == 32
/* socklen_t is int or size_t here, 32 bits either way. */
#ifdef __cplusplus
extern "C" {
#endif
int getsockname(int, struct sockaddr *__restrict, socklen_t *__restrict)
    __asm__("__irix_getsockname");
int getpeername(int, struct sockaddr *__restrict, socklen_t *__restrict)
    __asm__("__irix_getpeername");
#ifdef __cplusplus
}
#endif
#endif

#ifndef _SS_MAXSIZE
#define _SS_MAXSIZE 128
#define _SS_ALIGNSIZE (sizeof(long long))
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE                                                           \
  (_SS_MAXSIZE - (sizeof(sa_family_t) + _SS_PAD1SIZE + _SS_ALIGNSIZE))
struct sockaddr_storage {
  sa_family_t ss_family;
  char __ss_pad1[_SS_PAD1SIZE];
  long long __ss_align;
  char __ss_pad2[_SS_PAD2SIZE];
};
#endif

#endif /* __CLANG_IRIX_SYS_SOCKET_H */
