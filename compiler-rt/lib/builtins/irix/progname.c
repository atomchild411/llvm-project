//===-- irix/progname.c - getprogname and setprogname for IRIX ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// BSD's getprogname and setprogname, which IRIX's libc lacks. gnulib, copied
// into most GNU packages, asks for the program's name through getprogname
// and has no IRIX port of its own ("getprogname module not ported to this
// OS"); with the function here, its configure finds it and leaves the
// module out, in every package at once. clang's IRIX <stdlib.h> declares
// both.
//
// IRIX's startup code keeps the program's argv in __Argv (the same array
// main gets). The reference is weak, so a shared library that uses these
// still loads into a program whose startup code did not define it.
//
// Like every builtin, each module (the program, each shared library) links
// its own hidden copy: setprogname changes the name only for the module that
// calls it. getprogname, which is what gnulib and most code use, gives the
// same answer everywhere.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <stddef.h>
#include <string.h>

extern char **__Argv __attribute__((weak));

static const char *progname;

static const char *base(const char *path) {
  const char *s = strrchr(path, '/');
  return s ? s + 1 : path;
}

const char *getprogname(void) {
  if (progname == NULL && &__Argv != NULL && __Argv != NULL &&
      __Argv[0] != NULL)
    progname = base(__Argv[0]);
  return progname != NULL ? progname : "";
}

void setprogname(const char *name) { progname = base(name); }

#endif // __sgi
