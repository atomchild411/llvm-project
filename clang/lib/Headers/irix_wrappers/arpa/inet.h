/*===---- arpa/inet.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_ARPA_INET_H
#define __CLANG_IRIX_ARPA_INET_H

#include <sys/endian.h> /* htonl and friends (macros) */
#include_next <arpa/inet.h>

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
/* 6.5.22 (internal/ headers) declares them outside X/Open 5 mode itself. */
#if !_SGIAPI && (!__has_include(<internal/wchar_core.h>) || _XOPEN5)
#ifdef __cplusplus
extern "C" {
#endif
const char *inet_ntop(int, const void *, char *, size_t);
int inet_pton(int, const char *, void *);
#ifdef __cplusplus
}
#endif
#endif


/* IRIX has htonl and friends only as macros, and only in some modes. It is
 * big-endian: they change nothing. */
#ifndef htonl
#define htonl(x) ((__uint32_t)(x))
#define htons(x) ((__uint16_t)(x))
#define ntohl(x) ((__uint32_t)(x))
#define ntohs(x) ((__uint16_t)(x))
#endif

#endif /* __CLANG_IRIX_ARPA_INET_H */
