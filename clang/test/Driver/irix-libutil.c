// IRIX: no libutil; its pseudo-terminal functions are in the builtins
// library (irix/pty.c), and the driver drops -lutil from links. And
// -lstdc++ (libtool's IRIX case, for g++) is libc++.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -lutil 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -l util 2>&1 | FileCheck %s

// CHECK: "-lc"
// CHECK-NOT: "-lutil"
// CHECK-NOT: "util"

// RUN: %clang -### --target=mips-sgi-irix6.5 %s -lstdc++ 2>&1 \
// RUN:   | FileCheck %s --check-prefix=CXX
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -l stdc++ 2>&1 \
// RUN:   | FileCheck %s --check-prefix=CXX
// CXX: "-lc++"
// CXX-NOT: stdc++

// A link that names libc++ itself gets the DT_RPATH to the driver's libc++
// only when the driver runs on IRIX (where that tree is the program's); a
// cross link leaves it to the user's -rpath.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -nostdlib++ -lc++ 2>&1 \
// RUN:   | FileCheck %s --check-prefix=CROSS
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -nostdlib -l stdc++ 2>&1 \
// RUN:   | FileCheck %s --check-prefix=CROSS
// CROSS: "-lc++"
// CROSS-NOT: "-rpath"
