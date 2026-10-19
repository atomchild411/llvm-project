/*===---- arpa/telnet.h - IRIX wrapper ---------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <arpa/telnet.h> predates RFC 1572: it has TELOPT_ENVIRON (36, RFC
 * 1408's option) but not the names the BSDs and inetutils use for it and
 * for NEW-ENVIRON (39), nor the suboption codes of either, nor LFLOW's or
 * AUTHENTICATION's. Defined here when missing, with the BSDs' values. 39
 * is within IRIX's TELOPT_LAST (45), so TELOPT_OK and telopts[] cover it.
 */

#ifndef __CLANG_IRIX_ARPA_TELNET_H
#define __CLANG_IRIX_ARPA_TELNET_H

#include_next <arpa/telnet.h>

#ifndef TELOPT_OLD_ENVIRON
#define TELOPT_OLD_ENVIRON 36
#endif
#ifndef TELOPT_ENCRYPT
#define TELOPT_ENCRYPT 38
#endif
#ifndef TELOPT_NEW_ENVIRON
#define TELOPT_NEW_ENVIRON 39
#endif

/* RFC 1572 (NEW-ENVIRON) and RFC 1408 (OLD-ENVIRON) suboption codes. */
#ifndef NEW_ENV_VAR
#define NEW_ENV_VAR 0
#endif
#ifndef NEW_ENV_VALUE
#define NEW_ENV_VALUE 1
#endif
#ifndef ENV_ESC
#define ENV_ESC 2
#endif
#ifndef ENV_USERVAR
#define ENV_USERVAR 3
#endif
#ifndef OLD_ENV_VAR
#define OLD_ENV_VAR 1
#endif
#ifndef OLD_ENV_VALUE
#define OLD_ENV_VALUE 0
#endif

/* TELOPT_LFLOW suboption codes (RFC 1372), which IRIX's header names the
 * option for but does not define. */
#ifndef LFLOW_OFF
#define LFLOW_OFF 0
#endif
#ifndef LFLOW_ON
#define LFLOW_ON 1
#endif
#ifndef LFLOW_RESTART_ANY
#define LFLOW_RESTART_ANY 2
#endif
#ifndef LFLOW_RESTART_XON
#define LFLOW_RESTART_XON 3
#endif

/* AUTHENTICATION's qualifiers (RFC 2941) beside IS, SEND and INFO. */
#ifndef TELQUAL_REPLY
#define TELQUAL_REPLY 2
#endif
#ifndef TELQUAL_NAME
#define TELQUAL_NAME 3
#endif

#endif /* __CLANG_IRIX_ARPA_TELNET_H */
