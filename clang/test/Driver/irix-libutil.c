// IRIX: no libutil; its pseudo-terminal functions are in the builtins
// library (irix/pty.c), and the driver drops -lutil from links.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -lutil 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -l util 2>&1 | FileCheck %s

// CHECK: "-lc"
// CHECK-NOT: "-lutil"
// CHECK-NOT: "util"
