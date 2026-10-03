/*===---- fts.h - IRIX -------------------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * The BSDs' and glibc's file hierarchy walk, which IRIX lacks: compiler-rt's
 * IRIX builtins define it (irix/fts.c). It never changes directory, as with
 * FTS_NOCHDIR: fts_accpath is always the whole path, and each entry's
 * fts_path stays valid until the entry itself is freed.
 */

#ifndef __CLANG_IRIX_FTS_H
#define __CLANG_IRIX_FTS_H

#include <sys/types.h>
#include <sys/stat.h>

typedef struct _ftsent {
  struct _ftsent *fts_cycle;  /* the directory that makes a cycle */
  struct _ftsent *fts_parent;
  struct _ftsent *fts_link;   /* the next entry in the directory */
  long fts_number;            /* for the program */
  void *fts_pointer;          /* for the program */
  char *fts_accpath;          /* the path to access the file by */
  char *fts_path;             /* the root's path and the names below it */
  int fts_errno;
  int fts_symfd;              /* unused */
  size_t fts_pathlen;
  size_t fts_namelen;
  ino_t fts_ino;
  dev_t fts_dev;
  nlink_t fts_nlink;
  short fts_level;
  unsigned short fts_info;
  unsigned short fts_flags;
  unsigned short fts_instr;
  struct stat *fts_statp;
  char fts_name[1];
} FTSENT;

typedef struct {
  struct _ftsent *fts_cur;
  struct _ftsent *fts_child;
  struct _ftsent **fts_array; /* unused */
  dev_t fts_dev;              /* the current root's device, for FTS_XDEV */
  char *fts_path;             /* unused */
  int fts_rfd;                /* unused */
  size_t fts_pathlen;         /* unused */
  int fts_nitems;             /* unused */
  int (*fts_compar)(const struct _ftsent **, const struct _ftsent **);
  int fts_options;
} FTS;

#define FTS_ROOTPARENTLEVEL (-1)
#define FTS_ROOTLEVEL 0

/* fts_open's options */
#define FTS_COMFOLLOW 0x001
#define FTS_LOGICAL 0x002
#define FTS_NOCHDIR 0x004
#define FTS_NOSTAT 0x008
#define FTS_PHYSICAL 0x010
#define FTS_SEEDOT 0x020
#define FTS_XDEV 0x040
#define FTS_WHITEOUT 0x080
#define FTS_OPTIONMASK 0x0ff
#define FTS_NAMEONLY 0x100
#define FTS_STOP 0x200

/* fts_info */
#define FTS_D 1
#define FTS_DC 2
#define FTS_DEFAULT 3
#define FTS_DNR 4
#define FTS_DOT 5
#define FTS_DP 6
#define FTS_ERR 7
#define FTS_F 8
#define FTS_INIT 9
#define FTS_NS 10
#define FTS_NSOK 11
#define FTS_SL 12
#define FTS_SLNONE 13
#define FTS_W 14

/* fts_flags */
#define FTS_DONTCHDIR 0x01
#define FTS_SYMFOLLOW 0x02

/* fts_set's instructions */
#define FTS_AGAIN 1
#define FTS_FOLLOW 2
#define FTS_NOINSTR 3
#define FTS_SKIP 4

#ifdef __cplusplus
extern "C" {
#endif
FTS *fts_open(char *const *, int,
              int (*)(const FTSENT **, const FTSENT **));
FTSENT *fts_read(FTS *);
FTSENT *fts_children(FTS *, int);
int fts_set(FTS *, FTSENT *, int);
int fts_close(FTS *);
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_FTS_H */
