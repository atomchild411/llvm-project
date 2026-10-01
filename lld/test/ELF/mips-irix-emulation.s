# REQUIRES: mips
## GNU ld's IRIX emulation names, which libtool passes (-melf32bsmip even for
## n32 objects, as file(1) calls them "32-bit"), take the ABI from the inputs.

# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %s -o %t.o
# RUN: ld.lld -m elf32bsmip -shared %t.o -o %t1.so
# RUN: llvm-readelf -h %t1.so | FileCheck %s
# RUN: ld.lld -melf32bmipn32 -shared %t.o -o %t2.so
# RUN: llvm-readelf -h %t2.so | FileCheck %s

# CHECK: Class: ELF32
# CHECK: Type: DYN
# CHECK: Flags: {{.*}}abi2

  .globl f
f:
  jr $ra
  nop
