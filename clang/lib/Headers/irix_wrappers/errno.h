/*===---- errno.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __CLANG_IRIX_ERRNO_H
#define __CLANG_IRIX_ERRNO_H

#include_next <errno.h>

/* errno is the thread's, *__oserror(), as IRIX's <errno.h> makes it only
 * with _SGI_MP_SOURCE (which <pthread.h> defines) or X/Open 5 and POSIX
 * 1995 modes. Otherwise it is the global int, and a program that loads
 * libpthread -- nearly every one: libintl, libncurses and the driver's
 * -lpthread bring it -- keeps two: libc sets both when a call fails, but
 * `errno = 0' clears only the global, and libc copies the thread's back
 * into it on some paths (stdio's isatty). bash then rejected every number
 * (errno = 0; strtoimax; errno != 0), and in openssh a file built with
 * <pthread.h> set the EPIPE another one tested, so no key would load.
 * Without libpthread, __oserror() is &errno: nothing changes there. */
#ifdef __cplusplus
extern "C"
#endif
int *__oserror(void);
#undef errno
#define errno (*__oserror())

/* oserror(3C), in IRIX's libc and documented in <errno.h>, which declares
 * them only when _SGIAPI && _NO_ANSIMODE: MIPSpro's default mode defines
 * __EXTENSIONS__, clang's C modes do not. IRIX programs call oserror()
 * without a thought (IRIX's audio library reports its errors through it),
 * and C99 makes a call to an undeclared function an error: declared here
 * otherwise, with IRIX's own prototypes. */
#if !(_SGIAPI && _NO_ANSIMODE)
#ifdef __cplusplus
extern "C" {
#endif
int oserror(void);
int setoserror(int);
int goserror(void);
#ifdef __cplusplus
}
#endif
#endif

#endif /* __CLANG_IRIX_ERRNO_H */
