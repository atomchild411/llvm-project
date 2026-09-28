// IRIX's headers pick C, C++ or assembly declarations by _LANGUAGE_C,
// _LANGUAGE_C_PLUS_PLUS and _LANGUAGE_ASSEMBLY, as MIPSpro defined them.

// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -target-abi n32 -E -dM -x c /dev/null \
// RUN:   | FileCheck %s --check-prefix=C
// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -target-abi n32 -E -dM -x c++ /dev/null \
// RUN:   | FileCheck %s --check-prefix=CXX
// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -target-abi n32 -E -dM -x assembler-with-cpp /dev/null \
// RUN:   | FileCheck %s --check-prefix=ASM

// C: #define _LANGUAGE_C 1
// C-NOT: _LANGUAGE_ASSEMBLY
// C-NOT: _LANGUAGE_C_PLUS_PLUS

// CXX: #define _LANGUAGE_C_PLUS_PLUS 1
// CXX-NOT: _LANGUAGE_ASSEMBLY
// CXX-NOT: #define _LANGUAGE_C 1

// ASM: #define _LANGUAGE_ASSEMBLY 1
// ASM-NOT: _LANGUAGE_C
