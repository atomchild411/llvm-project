//===-- irix/loadavg.c - getloadavg for IRIX ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getloadavg (the BSDs', glibc's), which IRIX lacks. The kernel's avenrun
// (1, 5 and 15 minutes, scaled by 1024) is read with sysget(SGT_KSYM), as
// IRIX's own tools do, which needs no privilege. Returns the number of
// averages given, or -1 if the kernel would not say. clang's IRIX
// <stdlib.h> declares it.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <string.h>
#include <sys/sysget.h>

#define AVENRUN_SCALE 1024.0

int getloadavg(double loadavg[], int nelem) {
  int avenrun[3];
  sgt_cookie_t cookie;
  int i;
  if (nelem < 0)
    return -1;
  if (nelem > 3)
    nelem = 3;
  SGT_COOKIE_INIT(&cookie);
  SGT_COOKIE_SET_KSYM(&cookie, KSYM_AVENRUN);
  if (sysget(SGT_KSYM, (char *)avenrun, sizeof avenrun, SGT_READ, &cookie) !=
      (int)sizeof avenrun)
    return -1;
  for (i = 0; i < nelem; i++)
    loadavg[i] = avenrun[i] / AVENRUN_SCALE;
  return nelem;
}

#endif // defined(__sgi)
