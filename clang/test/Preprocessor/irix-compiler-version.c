// IRIX: MIPSpro's version macros, above its last release (7.4.4), so
// "MIPSpro 7.4.4 or older" workarounds in portable sources stay off.
// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -target-abi n32 -E -dM -x c /dev/null \
// RUN:   | FileCheck %s
// CHECK: #define _COMPILER_VERSION 750
// CHECK: #define _SGI_COMPILER_VERSION 750
