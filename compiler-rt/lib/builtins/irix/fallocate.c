//===-- irix/fallocate.c - posix_fallocate for IRIX -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// posix_fallocate(), which IRIX lacks (i3, libtorrent), done as glibc does
// where the file system cannot: past the end of the file a zero byte is
// written into every block of the range, so the blocks are allocated, and
// the file then reaches at least offset+len. Blocks already inside the file
// are left alone (a hole there stays one). Not atomic: another writer
// extending the file meanwhile is not accounted for. Errors are returned,
// not set in errno, as POSIX has it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#pragma weak posix_fallocate

int posix_fallocate(int fd, off_t offset, off_t len) {
  // The largest off_t (64 bits in n32 and n64, a long in o32).
  const off_t max =
      (off_t)(~(unsigned long long)0 >> (65 - 8 * sizeof(off_t)));
  int saved = errno, err = 0;
  struct stat st;
  if (offset < 0 || len <= 0)
    return EINVAL;
  if (offset > max - len)
    return EFBIG;
  if (fstat(fd, &st) != 0) {
    err = errno;
    errno = saved;
    return err;
  }
  if (S_ISFIFO(st.st_mode))
    return ESPIPE;
  if (!S_ISREG(st.st_mode))
    return ENODEV;
  off_t end = offset + len;
  if (end <= st.st_size)
    return 0;
  off_t blk = st.st_blksize > 0 ? (off_t)st.st_blksize : 4096;
  // Every write is at or past the end of the file as it then is, so no
  // data is overwritten; the last one is at end - 1.
  off_t pos = st.st_size > offset ? st.st_size : offset;
  while (pos < end) {
    if (pwrite(fd, "", 1, pos) != 1) {
      err = errno ? errno : EIO;
      break;
    }
    off_t next = (pos / blk + 1) * blk;
    if (pos < end - 1 && next > end - 1)
      next = end - 1;
    pos = next;
  }
  // (pwrite on a descriptor opened O_APPEND may write at the end instead:
  // make the size right in any case.)
  if (!err && fstat(fd, &st) == 0 && st.st_size < end &&
      ftruncate(fd, end) != 0)
    err = errno;
  errno = saved;
  return err;
}

#endif // defined(__sgi)
