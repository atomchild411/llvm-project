/*===---- stdlib.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <stdlib.h> does not say that abort and exit never return, so code
 * ending in either reads as falling off the end of a function. Say so.
 */

#ifndef __CLANG_IRIX_STDLIB_H
#define __CLANG_IRIX_STDLIB_H

#include_next <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
void abort(void) __attribute__((__noreturn__));
void exit(int) __attribute__((__noreturn__));
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_STDLIB_H */
