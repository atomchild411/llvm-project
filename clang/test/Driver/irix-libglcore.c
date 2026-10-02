// IRIX: libGL.so needs libGLcore.so, which has the GL functions, and lld does
// not resolve symbols through a library's dependencies: -lGL brings
// -lGLcore right after it.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -lGL 2>&1 | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 %s 2>&1 | FileCheck --check-prefix=NOGL %s

// CHECK: "-lGL" "-lGLcore"
// NOGL-NOT: "-lGLcore"
