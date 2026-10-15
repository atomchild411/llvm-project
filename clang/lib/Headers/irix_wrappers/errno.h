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
