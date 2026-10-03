//===-- irix/fts.c - fts for IRIX -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// fts_open, fts_read, fts_children, fts_set and fts_close (the BSDs' and
// glibc's), which IRIX lacks, for clang's IRIX <fts.h>.
//
// The walk never changes directory: every entry carries its whole path, which
// is also its fts_accpath, so FTS_NOCHDIR is how it always behaves. The order
// of visits and the fts_info each entry gets are the BSDs': a directory
// before its contents (FTS_D) and after them (FTS_DP), an unreadable one
// (FTS_DNR) or an empty one (FTS_DP) a second time instead, the roots in
// argument order or the comparison function's, each directory's entries in
// readdir order or the comparison function's. IRIX's dirent has no d_type,
// so FTS_NOSTAT still stats each entry to find the directories, and reports
// everything else as FTS_NSOK.
//
// Weak, so that a program's own copy wins.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include "irix_errno.h"
#include <fts.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma weak fts_open
#pragma weak fts_read
#pragma weak fts_children
#pragma weak fts_set
#pragma weak fts_close

#define ISDOT(a) ((a)[0] == '.' && (!(a)[1] || ((a)[1] == '.' && !(a)[2])))

// fts_children's list: to read the names only, or the entries in full.
enum { BCHILD, BNAMES, BREAD };

// One allocation: the entry, its stat buffer, its name and its path.
static FTSENT *fts_alloc(const char *dir, size_t dirlen, const char *name,
                         size_t namelen) {
  size_t sep = dirlen && dir[dirlen - 1] != '/';
  size_t pathlen = dirlen + sep + namelen;
  size_t stoff = (offsetof(FTSENT, fts_name) + namelen + 1 + 7) & ~(size_t)7;
  FTSENT *p = malloc(stoff + sizeof(struct stat) + pathlen + 1);
  if (p == NULL)
    return NULL;
  memset(p, 0, offsetof(FTSENT, fts_name));
  memcpy(p->fts_name, name, namelen);
  p->fts_name[namelen] = '\0';
  p->fts_namelen = namelen;
  p->fts_statp = (struct stat *)((char *)p + stoff);
  p->fts_path = (char *)(p->fts_statp + 1);
  memcpy(p->fts_path, dir, dirlen);
  if (sep)
    p->fts_path[dirlen] = '/';
  memcpy(p->fts_path + dirlen + sep, name, namelen);
  p->fts_path[pathlen] = '\0';
  p->fts_pathlen = pathlen;
  p->fts_accpath = p->fts_path;
  p->fts_instr = FTS_NOINSTR;
  return p;
}

static void fts_lfree(FTSENT *p) {
  while (p != NULL) {
    FTSENT *next = p->fts_link;
    free(p);
    p = next;
  }
}

static unsigned short fts_stat(FTS *sp, FTSENT *p, int follow) {
  struct stat *sb = p->fts_statp;
  if ((sp->fts_options & FTS_LOGICAL) || follow) {
    if (stat(p->fts_accpath, sb) != 0) {
      int e = errno;
      if (e == ENOENT && lstat(p->fts_accpath, sb) == 0) {
        __irix_seterrno(0);
        return FTS_SLNONE;
      }
      p->fts_errno = e;
      memset(sb, 0, sizeof *sb);
      return FTS_NS;
    }
  } else if (lstat(p->fts_accpath, sb) != 0) {
    p->fts_errno = errno;
    memset(sb, 0, sizeof *sb);
    return FTS_NS;
  }
  if (S_ISDIR(sb->st_mode)) {
    FTSENT *t;
    p->fts_dev = sb->st_dev;
    p->fts_ino = sb->st_ino;
    p->fts_nlink = sb->st_nlink;
    if (ISDOT(p->fts_name))
      return FTS_DOT;
    for (t = p->fts_parent; t != NULL && t->fts_level >= FTS_ROOTLEVEL;
         t = t->fts_parent)
      if (t->fts_ino == p->fts_ino && t->fts_dev == p->fts_dev) {
        p->fts_cycle = t;
        return FTS_DC;
      }
    return FTS_D;
  }
  if (S_ISLNK(sb->st_mode))
    return FTS_SL;
  if (S_ISREG(sb->st_mode))
    return FTS_F;
  return FTS_DEFAULT;
}

// Sorts a list by the comparison function; returns its new head.
static FTSENT *fts_sort(FTS *sp, FTSENT *head, size_t n) {
  FTSENT **a, *p;
  size_t i;
  if (sp->fts_compar == NULL || n < 2)
    return head;
  a = malloc(n * sizeof *a);
  if (a == NULL)
    return head;
  for (i = 0, p = head; p != NULL; p = p->fts_link)
    a[i++] = p;
  qsort(a, n, sizeof *a,
        (int (*)(const void *, const void *))sp->fts_compar);
  for (i = 0; i + 1 < n; i++)
    a[i]->fts_link = a[i + 1];
  a[n - 1]->fts_link = NULL;
  head = a[0];
  free(a);
  return head;
}

// The entries of the current directory, or NULL if it has none or cannot be
// read (then, for fts_read, it becomes FTS_DNR or FTS_DP).
static FTSENT *fts_build(FTS *sp, int type) {
  FTSENT *cur = sp->fts_cur, *head = NULL, *tail = NULL;
  struct dirent *dp;
  size_t n = 0;
  DIR *d = opendir(cur->fts_accpath);
  if (d == NULL) {
    if (type == BREAD) {
      cur->fts_info = FTS_DNR;
      cur->fts_errno = errno;
    }
    return NULL;
  }
  while ((dp = readdir(d)) != NULL) {
    FTSENT *p;
    if (!(sp->fts_options & FTS_SEEDOT) && ISDOT(dp->d_name))
      continue;
    p = fts_alloc(cur->fts_path, cur->fts_pathlen, dp->d_name,
                  strlen(dp->d_name));
    if (p == NULL) {
      int e = errno;
      closedir(d);
      fts_lfree(head);
      cur->fts_info = FTS_ERR;
      sp->fts_options |= FTS_STOP;
      __irix_seterrno(e);
      return NULL;
    }
    p->fts_level = cur->fts_level + 1;
    p->fts_parent = cur;
    if (type == BNAMES) {
      p->fts_info = FTS_NSOK;
    } else {
      p->fts_info = fts_stat(sp, p, 0);
      if ((sp->fts_options & FTS_NOSTAT) && p->fts_info != FTS_D &&
          p->fts_info != FTS_DC && p->fts_info != FTS_DOT)
        p->fts_info = FTS_NSOK;
    }
    if (tail == NULL)
      head = p;
    else
      tail->fts_link = p;
    tail = p;
    n++;
  }
  closedir(d);
  if (n == 0) {
    if (type == BREAD)
      cur->fts_info = FTS_DP;
    return NULL;
  }
  return fts_sort(sp, head, n);
}

FTS *fts_open(char *const *argv, int options,
              int (*compar)(const FTSENT **, const FTSENT **)) {
  FTS *sp;
  FTSENT *parent, *head = NULL, *tail = NULL, *p;
  size_t n = 0;
  if (options & ~FTS_OPTIONMASK) {
    __irix_seterrno(EINVAL);
    return NULL;
  }
  sp = calloc(1, sizeof *sp);
  if (sp == NULL)
    return NULL;
  sp->fts_compar = compar;
  sp->fts_options = options | ((options & FTS_LOGICAL) ? FTS_NOCHDIR : 0);
  sp->fts_rfd = -1;
  parent = fts_alloc("", 0, "", 0);
  if (parent == NULL)
    goto fail;
  parent->fts_level = FTS_ROOTPARENTLEVEL;
  for (; *argv != NULL; argv++) {
    size_t len = strlen(*argv);
    if (len == 0) {
      __irix_seterrno(ENOENT);
      goto fail;
    }
    p = fts_alloc("", 0, *argv, len);
    if (p == NULL)
      goto fail;
    p->fts_level = FTS_ROOTLEVEL;
    p->fts_parent = parent;
    p->fts_info = fts_stat(sp, p, options & FTS_COMFOLLOW);
    // A "." or ".." named as a root is a directory like any other.
    if (p->fts_info == FTS_DOT)
      p->fts_info = FTS_D;
    if (tail == NULL)
      head = p;
    else
      tail->fts_link = p;
    tail = p;
    n++;
  }
  head = fts_sort(sp, head, n);
  // fts_read starts from this placeholder, which leads to the first root.
  sp->fts_cur = fts_alloc("", 0, "", 0);
  if (sp->fts_cur == NULL)
    goto fail;
  sp->fts_cur->fts_link = head;
  sp->fts_cur->fts_parent = parent;
  sp->fts_cur->fts_info = FTS_INIT;
  return sp;
fail: {
  int e = errno;
  fts_lfree(head);
  free(parent);
  free(sp);
  __irix_seterrno(e);
  return NULL;
}
}

FTSENT *fts_read(FTS *sp) {
  FTSENT *p, *tmp;
  int instr;
  if (sp->fts_cur == NULL || (sp->fts_options & FTS_STOP))
    return NULL;
  p = sp->fts_cur;
  instr = p->fts_instr;
  p->fts_instr = FTS_NOINSTR;

  // Any entry may be visited again, stat'ed anew.
  if (instr == FTS_AGAIN) {
    p->fts_info = fts_stat(sp, p, 0);
    return p;
  }
  // A symbolic link to follow is visited again as what it points to.
  if (instr == FTS_FOLLOW &&
      (p->fts_info == FTS_SL || p->fts_info == FTS_SLNONE)) {
    p->fts_info = fts_stat(sp, p, 1);
    if (p->fts_info == FTS_D)
      p->fts_flags |= FTS_SYMFOLLOW;
    return p;
  }

  if (p->fts_info == FTS_D) {
    // Skipped, or on another device: its post-order visit, now.
    if (instr == FTS_SKIP ||
        ((sp->fts_options & FTS_XDEV) && p->fts_dev != sp->fts_dev)) {
      fts_lfree(sp->fts_child);
      sp->fts_child = NULL;
      p->fts_info = FTS_DP;
      return p;
    }
    // fts_children read only the names: read the entries in full.
    if (sp->fts_child != NULL && (sp->fts_options & FTS_NAMEONLY)) {
      sp->fts_options &= ~FTS_NAMEONLY;
      fts_lfree(sp->fts_child);
      sp->fts_child = NULL;
    }
    if (sp->fts_child == NULL &&
        (sp->fts_child = fts_build(sp, BREAD)) == NULL) {
      if (sp->fts_options & FTS_STOP)
        return NULL;
      return p;
    }
    p = sp->fts_child;
    sp->fts_child = NULL;
    return sp->fts_cur = p;
  }

  // The next entry in this directory, unless the program skipped it.
next:
  tmp = p;
  if ((p = p->fts_link) != NULL) {
    free(tmp);
    if (p->fts_level == FTS_ROOTLEVEL) {
      sp->fts_dev = p->fts_dev;
      return sp->fts_cur = p;
    }
    if (p->fts_instr == FTS_SKIP)
      goto next;
    if (p->fts_instr == FTS_FOLLOW) {
      p->fts_info = fts_stat(sp, p, 1);
      if (p->fts_info == FTS_D)
        p->fts_flags |= FTS_SYMFOLLOW;
      p->fts_instr = FTS_NOINSTR;
    }
    return sp->fts_cur = p;
  }

  // Back up to the directory, for its post-order visit.
  p = tmp->fts_parent;
  free(tmp);
  if (p->fts_level == FTS_ROOTPARENTLEVEL) {
    free(p);
    __irix_seterrno(0);
    return sp->fts_cur = NULL;
  }
  p->fts_info = p->fts_errno ? FTS_ERR : FTS_DP;
  return sp->fts_cur = p;
}

FTSENT *fts_children(FTS *sp, int instr) {
  FTSENT *p;
  if (instr != 0 && instr != FTS_NAMEONLY) {
    __irix_seterrno(EINVAL);
    return NULL;
  }
  p = sp->fts_cur;
  __irix_seterrno(0);
  if (p == NULL || (sp->fts_options & FTS_STOP))
    return NULL;
  if (p->fts_info == FTS_INIT)
    return p->fts_link;
  if (p->fts_info != FTS_D)
    return NULL;
  fts_lfree(sp->fts_child);
  if (instr == FTS_NAMEONLY)
    sp->fts_options |= FTS_NAMEONLY;
  else
    sp->fts_options &= ~FTS_NAMEONLY;
  sp->fts_child = fts_build(sp, instr == FTS_NAMEONLY ? BNAMES : BCHILD);
  return sp->fts_child;
}

int fts_set(FTS *sp, FTSENT *p, int instr) {
  (void)sp;
  if (instr != 0 && instr != FTS_AGAIN && instr != FTS_FOLLOW &&
      instr != FTS_NOINSTR && instr != FTS_SKIP) {
    __irix_seterrno(EINVAL);
    return -1;
  }
  p->fts_instr = instr;
  return 0;
}

int fts_close(FTS *sp) {
  FTSENT *p = sp->fts_cur;
  if (p != NULL) {
    // What is left: the rest of each directory up to the roots, and the
    // roots' parent.
    while (p->fts_level >= FTS_ROOTLEVEL) {
      FTSENT *t = p;
      p = p->fts_link != NULL ? p->fts_link : p->fts_parent;
      free(t);
    }
    free(p);
  }
  fts_lfree(sp->fts_child);
  free(sp);
  return 0;
}

#endif
