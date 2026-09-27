//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// dl_iterate_phdr for IRIX, whose runtime linker (rld) has none.
//
// rld keeps a doubly-linked list of the objects it has loaded, one
// Obj_Info record each, and every executable exports the head as
// __rld_obj_head (crt1.o holds it; the linker points it at .rld.map, which
// rld fills in). A record gives the address of the object's ELF header in
// memory and the address it was linked at; their difference is the load
// bias dl_iterate_phdr reports. The program headers follow from the ELF
// header, so the rest of libunwind's ELF path -- PT_GNU_EH_FRAME, the
// .eh_frame_hdr search table -- works as it does elsewhere.
//
//===----------------------------------------------------------------------===//

#ifndef __LIBUNWIND_IRIX_DL_ITERATE_PHDR_H__
#define __LIBUNWIND_IRIX_DL_ITERATE_PHDR_H__

#include <elf.h>
#include <objlist.h>
#include <stddef.h>
#include <stdint.h>

// The ELF extension that points at .eh_frame_hdr; IRIX's <elf.h> predates it.
#ifndef PT_GNU_EH_FRAME
#define PT_GNU_EH_FRAME 0x6474e550
#endif

#if _MIPS_SZPTR == 64
#define ElfW(type) Elf64_##type
typedef Elf64_Obj_Info __libunwind_irix_obj_info;
#else
#define ElfW(type) Elf32_##type
typedef Elf32_Obj_Info __libunwind_irix_obj_info;
#endif

struct dl_phdr_info {
  ElfW(Addr) dlpi_addr;        // load bias
  const char *dlpi_name;
  const ElfW(Phdr) *dlpi_phdr;
  ElfW(Half) dlpi_phnum;
};

// In the executable; weak so that libunwind links on its own.
extern "C" __libunwind_irix_obj_info *__rld_obj_head __attribute__((weak));

static inline int dl_iterate_phdr(int (*callback)(struct dl_phdr_info *,
                                                  size_t, void *),
                                  void *data) {
  if (!&__rld_obj_head)
    return 0;
  for (const __libunwind_irix_obj_info *obj = __rld_obj_head; obj;
       obj = (const __libunwind_irix_obj_info *)(uintptr_t)obj->oi_next) {
    // Records in the old format, if any, carry no ELF header address.
    if (obj->oi_magic != NEW_OBJ_INFO_MAGIC || obj->oi_ehdr == 0)
      continue;
    const ElfW(Ehdr) *ehdr = (const ElfW(Ehdr) *)(uintptr_t)obj->oi_ehdr;
    struct dl_phdr_info info;
    info.dlpi_addr = obj->oi_ehdr - obj->oi_orig_ehdr;
    info.dlpi_name = (const char *)(uintptr_t)obj->oi_pathname;
    info.dlpi_phdr =
        (const ElfW(Phdr) *)((const char *)ehdr + ehdr->e_phoff);
    info.dlpi_phnum = ehdr->e_phnum;
    if (int ret = callback(&info, sizeof info, data))
      return ret;
  }
  return 0;
}

#endif // __LIBUNWIND_IRIX_DL_ITERATE_PHDR_H__
