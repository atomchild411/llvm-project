/*===---- elf.h - IRIX wrapper -----------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * Elf32_Nhdr and Elf64_Nhdr, the note section header, which IRIX's <elf.h>
 * does not define and code that reads notes (Ruby's addr2line) expects.
 */
#ifndef __CLANG_IRIX_ELF_H
#define __CLANG_IRIX_ELF_H

#include_next <elf.h>

#ifndef __CLANG_IRIX_ELF_NHDR
#define __CLANG_IRIX_ELF_NHDR
typedef struct {
  Elf32_Word n_namesz;
  Elf32_Word n_descsz;
  Elf32_Word n_type;
} Elf32_Nhdr;
typedef struct {
  Elf64_Word n_namesz;
  Elf64_Word n_descsz;
  Elf64_Word n_type;
} Elf64_Nhdr;
#endif

#endif /* __CLANG_IRIX_ELF_H */
