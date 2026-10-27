// IRIX: rld reads DT_RPATH only, so links get --disable-new-dtags, after
// the inputs: a -Wl,--enable-new-dtags there (sudo's configure adds one)
// does not win, as lld takes the last of the two.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -Wl,--enable-new-dtags 2>&1 \
// RUN:   | FileCheck %s --check-prefix=USER
// RUN: %clang -### --target=mips-sgi-irix6.5 -shared %s \
// RUN:   -Wl,--enable-new-dtags 2>&1 | FileCheck %s --check-prefix=USER

// CHECK: "--disable-new-dtags"
// USER: "--enable-new-dtags"
// USER-SAME: "--disable-new-dtags"
