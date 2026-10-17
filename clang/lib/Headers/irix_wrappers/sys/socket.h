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
#define send __irix_libc_send
#define sendto __irix_libc_sendto
#define recvmsg __irix_bsd43_recvmsg
#define getsockname __irix_libc_getsockname
#define getpeername __irix_libc_getpeername
/* IRIX's struct sockaddr (and sockaddr_new) wraps its fields in
 * union { struct { ... } sa_generic; int sa_align; } sa_union and reaches
 * them through macros: sa_family is sa_union.sa_generic.sa_family2, and so
 * on. The macros take the names everywhere, so a parameter or variable
 * called sa_family, sa_data or sa_len stops compiling (libssh, bind,
 * toxcore). Make the union and the struct anonymous and give the members
 * their own names: the same layout (the int keeps it 4-aligned), and
 * s.sa_family is a plain member. IRIX's struct sockaddr_new, after its
 * macros, would declare its members through them: leave it out (only
 * kernel code uses its members; <net/if.h> and <net/route.h> keep pointers
 * to it, which need no body), as IRIX does when _HAVE_SIN_LEN is set. */
#ifndef _HAVE_SIN_LEN
#define __IRIX_WRAPPER_HAVE_SIN_LEN
#define _HAVE_SIN_LEN
#endif
#define sa_union
#define sa_generic
#define sa_family2 sa_family
#define sa_data2 sa_data
#define sa_len2 sa_len
#include_next <sys/socket.h>
#ifdef __IRIX_WRAPPER_HAVE_SIN_LEN
#undef _HAVE_SIN_LEN
#undef __IRIX_WRAPPER_HAVE_SIN_LEN
#endif
#undef sa_union
#undef sa_generic
#undef sa_family2
#undef sa_data2
#undef sa_len2
#undef sa_family
#undef sa_data
/* sa_len as IRIX defines it names a member it compiles out (no
 * _HAVE_SA_LEN: the kernel's sockaddr has no length byte); a variable
 * called sa_len stopped compiling (dbus). */
#undef sa_len
#undef msghdr
#undef sendmsg
#undef send
#undef sendto
#undef recvmsg
#undef getsockname
#undef getpeername
#undef msg_control
#undef msg_controllen

#ifndef _SOCKLEN_T
#define _SOCKLEN_T
#if _NO_XOPEN4
typedef int socklen_t;
#else
typedef size_t socklen_t;
#endif
#endif

/* msg_namelen is a socklen_t, as POSIX has it (code such as asio passes its
 * address to accept()): int or size_t, 32 bits either way, as the
 * kernel's. */
struct msghdr {
  void *msg_name;
  socklen_t msg_namelen;
  struct iovec *msg_iov;
  int msg_iovlen;
  void *msg_control;
  size_t msg_controllen;
  int msg_flags;
};

#ifdef __cplusplus
extern "C" {
#endif
ssize_t sendmsg(int, const struct msghdr *, int) __asm__("__irix_sendmsg");
ssize_t recvmsg(int, struct msghdr *, int) __asm__("__irix_recvmsg");
#ifdef __cplusplus
}
#endif

/* Linux's and the BSDs' MSG_NOSIGNAL, which IRIX lacks: a flag bit IRIX
 * does not use (its are 0x1 to 0x80 and the top two bits), and send, sendto
 * and sendmsg (above) bound to compiler-rt's irix/socket.c, which holds
 * SIGPIPE off for a call with it: the call fails with EPIPE and no signal
 * is delivered. Without the flag they are IRIX's own. */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x20000
#endif
#ifdef __cplusplus
extern "C" {
#endif
ssize_t send(int, const void *, size_t, int) __asm__("__irix_send");
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
               socklen_t) __asm__("__irix_sendto");
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
