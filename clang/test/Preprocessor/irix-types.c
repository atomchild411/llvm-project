// RUN: %clang_cc1 -E -dM -triple mips64-sgi-irix6.5 -target-abi n32 < /dev/null | FileCheck --check-prefix=N32 %s
// RUN: %clang_cc1 -E -dM -triple mips64-sgi-irix6.5 -target-abi n64 < /dev/null | FileCheck --check-prefix=N64 %s

// IRIX's headers make wchar_t, wint_t and intptr_t long where long is 32
// bits, and long double is a double.

// N32: #define __INTPTR_TYPE__ long int
// N32: #define __SIZEOF_LONG_DOUBLE__ 8
// N32: #define __WCHAR_TYPE__ long int
// N32: #define __WINT_TYPE__ long int

// N64: #define __INTPTR_TYPE__ long int
// N64: #define __SIZEOF_LONG_DOUBLE__ 8
// N64: #define __WCHAR_TYPE__ int
// N64: #define __WINT_TYPE__ int
