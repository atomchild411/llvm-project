// IRIX: the compiler-rt builtins are linked as -L<dir> -lclang_rt.builtins,
// never by the archive's path, so libtool (which keeps only -L, -l and
// objects from the driver's link line) links them into C++ shared libraries.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s 2>&1 | FileCheck %s
// RUN: %clangxx -### --target=mips-sgi-irix6.5 %s 2>&1 | FileCheck %s
// RUN: %clangxx -### --target=mips-sgi-irix6.5 -shared %s 2>&1 | FileCheck %s

// CHECK: "-L{{[^"]*}}lib{{/|\\\\}}{{[^"]*}}" "-lclang_rt.builtins"
// CHECK-NOT: libclang_rt.builtins.a
