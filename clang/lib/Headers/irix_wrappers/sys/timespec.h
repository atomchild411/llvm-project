/*===---- sys/timespec.h - IRIX wrapper -------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <sys/timespec.h> includes <sys/types.h> before it defines
 * timespec_t, and <sys/types.h> (SGI mode) reaches <sys/select.h> and
 * <sys/time.h>; with gnulib's replacements of those, that comes to
 * <signal.h>, which uses timespec_t (gettext-tools).  So include
 * <sys/types.h> first, while this header's guard is still unset: a nested
 * <sys/timespec.h> then finds IRIX's header unread and defines timespec_t
 * (time_t is defined by then) before <signal.h> needs it.
 */

#ifndef __CLANG_IRIX_SYS_TIMESPEC_H
#include <sys/types.h>
#define __CLANG_IRIX_SYS_TIMESPEC_H
#include_next <sys/timespec.h>
#endif /* __CLANG_IRIX_SYS_TIMESPEC_H */
