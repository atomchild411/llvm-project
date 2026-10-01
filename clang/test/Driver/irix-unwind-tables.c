// IRIX: asynchronous unwind tables by default, for C as for C++.
// RUN: %clang -### --target=mips-sgi-irix6.5 -c %s 2>&1 | FileCheck %s --check-prefix=ASYNC
// RUN: %clang -### --target=mips-sgi-irix6.5 -fno-asynchronous-unwind-tables -funwind-tables -c %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=SYNC
// RUN: %clang -### --target=mips-sgi-irix6.5 -fno-asynchronous-unwind-tables -c %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NONE

// ASYNC: "-funwind-tables=2"
// SYNC: "-funwind-tables=1"
// NONE-NOT: "-funwind-tables
