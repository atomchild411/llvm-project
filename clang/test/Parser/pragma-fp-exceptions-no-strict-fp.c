// RUN: %clang_cc1 -triple mips64-unknown-linux-gnuabin32 -fsyntax-only -verify %s
// RUN: %clang_cc1 -triple mips64-sgi-irix6.5 -fsyntax-only -verify %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -fsyntax-only -verify=x86 %s

// A target without strict FP support ignores the pragma, as it does
// FENV_ACCESS and float_control; ignore is the default and stays accepted.
// x86-no-diagnostics

#pragma clang fp exceptions(strict) // expected-warning {{clang fp exceptions' is not supported on this target - ignored}}
#pragma clang fp exceptions(maytrap) // expected-warning {{clang fp exceptions' is not supported on this target - ignored}}
#pragma clang fp exceptions(ignore)

int f(double x, double y) { return x > y; }
