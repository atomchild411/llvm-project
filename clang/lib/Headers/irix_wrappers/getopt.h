/*===---- getopt.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * getopt_long and getopt_long_only, which IRIX lacks: its <getopt.h> has only
 * getopt. compiler-rt's IRIX builtins (irix/getopt_long.c) implement them as
 * GNU and the BSDs do -- non-options permuted to the end unless the option
 * string starts with '+' or POSIXLY_CORRECT is set, '-' returning them as
 * option 1, a leading ':' for silent missing-argument reports -- using libc's
 * optind, optarg, opterr and optopt. They restart when optind is set to 0,
 * or optreset to 1.
 *
 * IRIX's <stdio.h>, <stdlib.h> and <unistd.h> include <getopt.h> too; their
 * wrappers say so, and only a program's own #include <getopt.h> declares
 * these, as with glibc, so programs that define a struct option of their own
 * keep compiling.
 */

#include_next <getopt.h>

#if !defined(__IRIX_GETOPT_INDIRECT) && !defined(__CLANG_IRIX_GETOPT_LONG)
#define __CLANG_IRIX_GETOPT_LONG

#ifdef __cplusplus
extern "C" {
#endif

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2

extern int optreset;

int getopt_long(int, char *const *, const char *, const struct option *,
                int *);
int getopt_long_only(int, char *const *, const char *, const struct option *,
                     int *);

#ifdef __cplusplus
}
#endif

#endif
