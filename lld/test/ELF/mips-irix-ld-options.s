# REQUIRES: mips
## MIPSpro ld's options that IRIX build systems pass are accepted and ignored.

# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %s -o %t.o
# RUN: ld.lld -n32 -shared -rdata_shared -elf -set_version sgi1.0 \
# RUN:   -update_registry %t.reg -check_registry %t.reg %t.o -o %t.so
# RUN: llvm-readelf -h %t.so | FileCheck %s

# CHECK: Type: DYN

  .globl f
f:
  jr $ra
  nop
