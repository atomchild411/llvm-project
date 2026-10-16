// IRIX: MIPSpro's options, which IRIX makefiles and build scripts pass, are
// accepted and ignored, in compiles and in links.
// RUN: %clang -### --target=mips-sgi-irix6.5 -c %s -woff 1110,1552 \
// RUN:   -OPT:Olimit=0 -fullwarn -n32 -IPA -LNO:opt=0 -TARG:platform=ip28 \
// RUN:   -TENV:large_GOT -LANG:std -INLINE:none -CG:unique_exit=on 2>&1 \
// RUN:   | FileCheck %s
// RUN: %clang -### --target=mips-sgi-irix6.5 %s -woff 1110 -OPT:Olimit=0 \
// RUN:   -fullwarn -n32 2>&1 | FileCheck %s

// CHECK-NOT: error:
// CHECK-NOT: warning:
// CHECK-NOT: "-woff"
// CHECK-NOT: "1110
// CHECK-NOT: OPT:
// CHECK-NOT: "-n32"
