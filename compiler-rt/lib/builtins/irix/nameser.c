//===-- irix/nameser.c - BIND 8's DNS message parser for IRIX -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// ns_initparse, ns_parserr and the rest of BIND 8's message parser, which
// glibc and the BSDs have (in libresolv) and IRIX, whose resolver is BIND
// 4.9's, lacks; clang's IRIX <arpa/nameser.h> declares them. Names are read
// with IRIX's own dn_expand and dn_skipname (libc). Errors set errno:
// EMSGSIZE for a malformed message, ENODEV for a record that is not there.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include "irix_errno.h"
#include <arpa/nameser.h>
#include <errno.h>
#include <string.h>

// IRIX's, under their own names: <resolv.h> renames dn_expand.
extern int __irix_libc_dn_expand(const unsigned char *, const unsigned char *,
                                 const unsigned char *, char *, int)
    __asm__("dn_expand");
extern int __irix_libc_dn_skipname(const unsigned char *,
                                   const unsigned char *)
    __asm__("dn_skipname");

unsigned int ns_get16(const unsigned char *src) {
  unsigned int v;
  NS_GET16(v, src);
  return v;
}

unsigned long ns_get32(const unsigned char *src) {
  unsigned long v;
  NS_GET32(v, src);
  return v;
}

void ns_put16(unsigned int v, unsigned char *dst) { NS_PUT16(v, dst); }

void ns_put32(unsigned long v, unsigned char *dst) { NS_PUT32(v, dst); }

static const struct {
  unsigned short mask, shift;
} flag_bits[ns_f_max] = {
    {0x8000, 15}, // qr
    {0x7800, 11}, // opcode
    {0x0400, 10}, // aa
    {0x0200, 9},  // tc
    {0x0100, 8},  // rd
    {0x0080, 7},  // ra
    {0x0040, 6},  // z
    {0x0020, 5},  // ad
    {0x0010, 4},  // cd
    {0x000f, 0},  // rcode
};

int ns_msg_getflag(ns_msg handle, int flag) {
  if (flag < 0 || flag >= ns_f_max)
    return 0;
  return (handle._flags & flag_bits[flag].mask) >> flag_bits[flag].shift;
}

static int bad_size(void) {
  __irix_seterrno(EMSGSIZE);
  return -1;
}

int ns_name_skip(const unsigned char **ptrptr, const unsigned char *eom) {
  int n = __irix_libc_dn_skipname(*ptrptr, eom);
  if (n < 0)
    return bad_size();
  *ptrptr += n;
  return 0;
}

int ns_name_uncompress(const unsigned char *msg, const unsigned char *eom,
                       const unsigned char *src, char *dst, size_t dstsiz) {
  int n = __irix_libc_dn_expand(msg, eom, src, dst, (int)dstsiz);
  if (n < 0)
    return bad_size();
  return n;
}

// The length of count records of the section starting at ptr.
int ns_skiprr(const unsigned char *ptr, const unsigned char *eom,
              ns_sect section, int count) {
  const unsigned char *start = ptr;
  for (; count > 0; count--) {
    int n = __irix_libc_dn_skipname(ptr, eom);
    if (n < 0)
      return bad_size();
    ptr += n + NS_INT16SZ + NS_INT16SZ; // type, class
    if (section != ns_s_qd) {
      unsigned int rdlength;
      if (ptr + NS_INT32SZ + NS_INT16SZ > eom)
        return bad_size();
      ptr += NS_INT32SZ; // ttl
      NS_GET16(rdlength, ptr);
      ptr += rdlength;
    }
  }
  if (ptr > eom)
    return bad_size();
  return (int)(ptr - start);
}

static void setsection(ns_msg *msg, ns_sect sect) {
  msg->_sect = sect;
  if (sect == ns_s_max) {
    msg->_rrnum = -1;
    msg->_msg_ptr = 0;
  } else {
    msg->_rrnum = 0;
    msg->_msg_ptr = msg->_sections[(int)sect];
  }
}

int ns_initparse(const unsigned char *msg, int msglen, ns_msg *handle) {
  const unsigned char *eom = msg + msglen;
  int i;
  memset(handle, 0, sizeof *handle);
  handle->_msg = msg;
  handle->_eom = eom;
  if (msglen < NS_HFIXEDSZ)
    return bad_size();
  NS_GET16(handle->_id, msg);
  NS_GET16(handle->_flags, msg);
  for (i = 0; i < ns_s_max; i++)
    NS_GET16(handle->_counts[i], msg);
  for (i = 0; i < ns_s_max; i++) {
    if (handle->_counts[i] == 0) {
      handle->_sections[i] = 0;
    } else {
      int n = ns_skiprr(msg, eom, (ns_sect)i, handle->_counts[i]);
      if (n < 0)
        return -1;
      handle->_sections[i] = msg;
      msg += n;
    }
  }
  if (msg != eom)
    return bad_size();
  setsection(handle, ns_s_max);
  return 0;
}

int ns_parserr(ns_msg *handle, ns_sect section, int rrnum, ns_rr *rr) {
  int n;
  if ((int)section < 0 || section >= ns_s_max) {
    __irix_seterrno(ENODEV);
    return -1;
  }
  if (section != handle->_sect)
    setsection(handle, section);
  if (rrnum == -1)
    rrnum = handle->_rrnum;
  if (rrnum < 0 || rrnum >= handle->_counts[(int)section]) {
    __irix_seterrno(ENODEV);
    return -1;
  }
  if (rrnum < handle->_rrnum)
    setsection(handle, section);
  if (rrnum > handle->_rrnum) {
    n = ns_skiprr(handle->_msg_ptr, handle->_eom, section,
                  rrnum - handle->_rrnum);
    if (n < 0)
      return -1;
    handle->_msg_ptr += n;
    handle->_rrnum = rrnum;
  }
  n = __irix_libc_dn_expand(handle->_msg, handle->_eom, handle->_msg_ptr,
                            rr->name, NS_MAXDNAME);
  if (n < 0)
    return bad_size();
  handle->_msg_ptr += n;
  if (handle->_msg_ptr + NS_INT16SZ + NS_INT16SZ > handle->_eom)
    return bad_size();
  NS_GET16(rr->type, handle->_msg_ptr);
  NS_GET16(rr->rr_class, handle->_msg_ptr);
  if (section == ns_s_qd) {
    rr->ttl = 0;
    rr->rdlength = 0;
    rr->rdata = 0;
  } else {
    if (handle->_msg_ptr + NS_INT32SZ + NS_INT16SZ > handle->_eom)
      return bad_size();
    NS_GET32(rr->ttl, handle->_msg_ptr);
    NS_GET16(rr->rdlength, handle->_msg_ptr);
    if (handle->_msg_ptr + rr->rdlength > handle->_eom)
      return bad_size();
    rr->rdata = handle->_msg_ptr;
    handle->_msg_ptr += rr->rdlength;
  }
  if (++handle->_rrnum > handle->_counts[(int)section])
    setsection(handle, (ns_sect)((int)section + 1));
  return 0;
}

#endif // defined(__sgi)
