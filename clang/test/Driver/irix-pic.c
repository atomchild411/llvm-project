// IRIX code is always PIC: -fno-pic and -fno-PIE are ignored (rld cannot
// load non-PIC code: no copy relocations, no PLT).
// RUN: %clang -### --target=mips-sgi-irix6.5 -c %s 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 -fno-PIE -c %s 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 -fno-pic -c %s 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 -fno-pie -fno-PIC -c %s 2>&1 | FileCheck %s
// CHECK: "-mrelocation-model" "pic" "-pic-level" "{{[12]}}"
// CHECK-NOT: "-mno-abicalls"
