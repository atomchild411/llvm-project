# REQUIRES: mips
## IRIX's rld ignores a dynamic relocation with symbol index 0, so the page
## entries of a secondary GOT, which lld relocates by the load address alone,
## must name the section symbol of the output section they point into, like
## every other relocation that would name no symbol. Otherwise a moved
## multi-GOT library keeps its link-time page addresses there.

# RUN: rm -rf %t && split-file %s %t
# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %t/a.s -o %t/a.o
# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %t/b.s -o %t/b.o
# RUN: ld.lld -shared -m elf32btsmipn32_irix -mips-got-size 12 %t/a.o %t/b.o -o %t.so
# RUN: llvm-readobj -r %t.so | FileCheck %s
# RUN: llvm-objdump -s --section=.got %t.so | FileCheck --check-prefix=GOT %s

## The secondary GOT's page entries name .data and hold its page addresses.
# CHECK:      Section ({{.*}}) .rel.dyn {
# CHECK-NEXT:   0x0 R_MIPS_NONE -
# CHECK-NOT:    R_MIPS_REL32 -
# CHECK:        R_MIPS_REL32 .data
# CHECK-NOT:    R_MIPS_REL32 -
# CHECK:      }

# GOT:      Contents of section .got:
# GOT-NEXT: {{[0-9a-f]+}} 00000000 80000000 00430000 00440000

#--- a.s
  .text
  .globl fa
fa:
  lw    $2, %got_page(da)($gp)
  addiu $2, $2, %got_ofst(da)

  .data
da:
  .4byte 0

#--- b.s
  .text
  .globl fb
fb:
  lw    $2, %got_page(db)($gp)
  addiu $2, $2, %got_ofst(db)

  .data
db:
  .4byte 0
