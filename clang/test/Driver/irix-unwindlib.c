// IRIX: --unwindlib is no unused argument in links that add no unwinder
// (C links, C++ links with -nostdlib++), where LLVM's runtimes pass it.
// RUN: %clang -### --target=mips-sgi-irix6.5 --unwindlib=none %s 2>&1 \
// RUN:   | FileCheck %s
// RUN: %clangxx -### --target=mips-sgi-irix6.5 --unwindlib=none -nostdlib++ %s 2>&1 \
// RUN:   | FileCheck %s

// CHECK-NOT: argument unused during compilation: '--unwindlib=none'
