# REQUIRES: mips
## IRIX: an executable that refers to _DYNAMIC_LINK (crt1.o does) gets it
## defined locally, absolute 2, so the reference does not make a library
## that also defines it (libc, libpthread) NEEDED under --as-needed.

# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %s -o %t.o
# RUN: echo '.globl _DYNAMIC_LINK; _DYNAMIC_LINK = 2' | \
# RUN:   llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 - -o %t2.o
# RUN: ld.lld -m elf32btsmipn32_irix -shared -soname=libp.so %t2.o -o %t.so
# RUN: ld.lld -m elf32btsmipn32_irix %t.o --as-needed %t.so -o %t
# RUN: llvm-readelf -d -s %t | FileCheck %s

# CHECK-NOT: (NEEDED)
# CHECK: 00000002 0 NOTYPE {{.*}} ABS _DYNAMIC_LINK

.globl __start
__start:
  lw $2, %got_disp(_DYNAMIC_LINK)($gp)
