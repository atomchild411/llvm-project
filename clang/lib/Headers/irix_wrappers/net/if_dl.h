/*===---- net/if_dl.h - IRIX wrapper ----------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <net/if_dl.h> has no include guard: a second #include (Erlang's
 * socket code has two) redefines struct sockaddr_dl. Guard it here.
 *
 * It also uses the BSD types u_char and u_short, which <sys/types.h> gives
 * only outside POSIX and X/Open modes: bring them in first, as <net/if.h>
 * does.
 */

#ifndef __CLANG_IRIX_NET_IF_DL_H
#define __CLANG_IRIX_NET_IF_DL_H

#include <sys/types.h>
#include <sys/bsd_types.h>
#include_next <net/if_dl.h>

#endif /* __CLANG_IRIX_NET_IF_DL_H */
