# REQUIRES: mips
## IRIX's rld skips the first entry of the dynamic relocation table, which
## the MIPS ABI makes a null relocation: IRIX output starts .rel.dyn with an
## R_MIPS_NONE entry, counted in DT_RELSZ, so the first real relocation is
## applied when rld moves the object.

# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %s -o %t.o
# RUN: ld.lld -shared -m elf32btsmipn32_irix %t.o -o %t.so
# RUN: llvm-readobj -r --dynamic-table %t.so | FileCheck %s

# CHECK:      RELSZ 24 (bytes)
# CHECK:      Section ({{.*}}) .rel.dyn {
# CHECK-NEXT:   0x0 R_MIPS_NONE -
# CHECK-NEXT:   0x{{[0-9A-F]+}} R_MIPS_REL32 .data
# CHECK-NEXT:   0x{{[0-9A-F]+}} R_MIPS_REL32 .data
# CHECK-NEXT: }

  .data
  .globl ptrs
ptrs:
  .4byte here
  .4byte here + 4
here:
  .4byte 0
