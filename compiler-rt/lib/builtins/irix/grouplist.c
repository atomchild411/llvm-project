//===-- irix/grouplist.c - getgrouplist for IRIX --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// getgrouplist, the BSDs' and glibc's, which IRIX's libc lacks; clang's IRIX
// <grp.h> and <unistd.h> declare it. The groups a user is in: GROUP first,
// then every group in the group database that lists USER as a member, each
// once. As glibc's, it returns the number of groups, or -1 when they do not
// fit in *NGROUPS, which it sets to the number either way, so a caller can
// grow its array and ask again.
//
// It walks the database with setgrent/getgrent/endgrent, so it is not
// thread-safe, and it moves the getgrent position of a caller in the middle
// of a walk of its own.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <grp.h>
#include <string.h>
#include <sys/types.h>

#pragma weak getgrouplist

static int have(const gid_t *groups, int n, gid_t gid) {
  for (int i = 0; i < n; i++)
    if (groups[i] == gid)
      return 1;
  return 0;
}

int getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
  int max = *ngroups, n = 0;
  struct group *gr;

  if (n < max)
    groups[n] = group;
  n++;
  setgrent();
  while ((gr = getgrent()) != NULL) {
    char **m;
    if (gr->gr_gid == group || !gr->gr_mem)
      continue;
    for (m = gr->gr_mem; *m; m++)
      if (strcmp(*m, user) == 0)
        break;
    // Past the array, duplicates cannot be told apart: count them, as an
    // over-estimate of the size to ask with next time.
    if (*m && !(n <= max && have(groups, n, gr->gr_gid))) {
      if (n < max)
        groups[n] = gr->gr_gid;
      n++;
    }
  }
  endgrent();
  *ngroups = n;
  return n > max ? -1 : n;
}

#endif // __sgi
