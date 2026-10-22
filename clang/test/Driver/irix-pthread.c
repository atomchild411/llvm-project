// IRIX: libpthread goes before libc on every link, as needed, so an object
// that calls pthread functions gets NEEDED libpthread rather than libc's
// stubs (rld's init order crashes such an object's init code otherwise).
// -pthread links it unconditionally; -nolibc leaves both out.
// RUN: %clang -### --target=mips-sgi-irix6.5 %s 2>&1 | FileCheck %s
// CHECK: "--push-state" "--as-needed" "-lpthread" "--pop-state" "-lc"

// RUN: %clang -### --target=mips-sgi-irix6.5 -pthread %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=PTHREAD
// PTHREAD-NOT: "--as-needed" "-lpthread"
// PTHREAD: "-lpthread" "-lc"

// RUN: %clang -### --target=mips-sgi-irix6.5 -nolibc %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=NOLIBC
// NOLIBC-NOT: "-lpthread"
// NOLIBC-NOT: "-lc"
