# REQUIRES: mips
## IRIX's ld defines _ftext, _fdata and _fbss at the starts of the text, data
## and bss; Boehm GC's IRIX configuration takes its data roots from _fdata.

# RUN: llvm-mc -filetype=obj -triple=mips64-sgi-irix6.5 -target-abi n32 %s -o %t.o
# RUN: ld.lld -m elf32btsmipn32_irix %t.o -o %t
# RUN: llvm-readelf -S -s %t | FileCheck %s

# CHECK:      .text    PROGBITS [[TEXT:[0-9a-f]+]]
# CHECK:      .data    PROGBITS [[DATA:[0-9a-f]+]]
# CHECK:      .bss     NOBITS   [[BSS:[0-9a-f]+]]
# CHECK-DAG:  [[TEXT]] 0 NOTYPE GLOBAL DEFAULT {{.*}} _ftext
# CHECK-DAG:  [[DATA]] 0 NOTYPE GLOBAL DEFAULT {{.*}} _fdata
# CHECK-DAG:  [[BSS]]  0 NOTYPE GLOBAL DEFAULT {{.*}} _fbss

  .globl __start
  .text
__start:
  lw $2, %got_disp(_ftext)($gp)
  lw $2, %got_disp(_fdata)($gp)
  lw $2, %got_disp(_fbss)($gp)
  .data
  .4byte 1
  .bss
  .4byte 0
