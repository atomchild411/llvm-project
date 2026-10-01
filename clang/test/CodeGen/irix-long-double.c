// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -target-abi n32 -fmath-errno -emit-llvm -o - %s | FileCheck %s

// long double is a double on IRIX, and the long double library builtins call
// the double functions: IRIX's own *l take MIPSpro's pair of doubles.

// CHECK: @size = global i32 8
int size = sizeof(long double);

// CHECK-LABEL: define{{.*}} double @f(
// CHECK: call double @sqrt(double
// CHECK: call double @cbrt(double
// CHECK: call signext i32 @lround(double
// CHECK: call double @nextafter(double
// CHECK: call double @nextafter(double
long double f(long double x) {
  return __builtin_sqrtl(x) + __builtin_cbrtl(x) + __builtin_lroundl(x) +
         __builtin_nexttowardl(x, x) + __builtin_nexttoward(x, x);
}
