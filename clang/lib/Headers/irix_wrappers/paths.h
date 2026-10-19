/*===---- paths.h - IRIX wrapper ---------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * The BSD and glibc <paths.h> names IRIX's lacks, with IRIX's values where
 * it has one: _PATH_DEFPATH is its _PATH_USERPATH without the trailing
 * empty entry (the current directory), _PATH_STDPATH its _PATH_ROOTPATH.
 * IRIX 6.5 keeps the mount table in /etc/mtab and has /var/run.
 */

#ifndef __CLANG_IRIX_PATHS_H
#define __CLANG_IRIX_PATHS_H

#include_next <paths.h>

#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "/usr/sbin:/usr/bsd:/sbin:/usr/bin:/usr/bin/X11"
#endif
#ifndef _PATH_STDPATH
#define _PATH_STDPATH _PATH_ROOTPATH
#endif
#ifndef _PATH_VARRUN
#define _PATH_VARRUN "/var/run/"
#endif
#ifndef _PATH_MOUNTED
#define _PATH_MOUNTED "/etc/mtab"
#endif
#ifndef _PATH_SHELLS
#define _PATH_SHELLS "/etc/shells"
#endif
#ifndef _PATH_DEVZERO
#define _PATH_DEVZERO "/dev/zero"
#endif

#endif /* __CLANG_IRIX_PATHS_H */
