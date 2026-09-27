//===-- irix/fenv.c - C99 <fenv.h> for IRIX -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// C99's floating-point environment for IRIX roots whose headers have no
// <fenv.h> (it comes with MIPSpro 7.3/7.4; a stock 6.5.22 has none, and its
// libc and libm lack the functions). clang's IRIX <fenv.h> sends the fe*
// functions here by asm label in that case.
//
// The environment is the FPU's control and status register ($31): the
// rounding mode in bits 0-1, the sticky flags in bits 2-6 (inexact,
// underflow, overflow, divide by zero, invalid), the trap enables in 7-11
// and the causes in 12-17 -- the MIPS architecture's layout, on which the
// header's FE_* values are the flag bits.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#define ALL 0x7cu          // the flag bits
#define ENABLES (ALL << 5) // the trap-enable bits
#define CAUSES (0x3fu << 12)

const unsigned int __irix_fe_dfl_env = 0; // round to nearest, no flags or traps

static unsigned int fcsr(void) {
  unsigned int r;
  __asm__ __volatile__("cfc1 %0, $31" : "=r"(r));
  return r;
}
static void set_fcsr(unsigned int r) {
  __asm__ __volatile__("ctc1 %0, $31" : : "r"(r));
}

int __irix_feclearexcept(int e) {
  set_fcsr(fcsr() & ~((unsigned)e & ALL));
  return 0;
}
int __irix_fegetexceptflag(unsigned int *f, int e) {
  *f = fcsr() & (unsigned)e & ALL;
  return 0;
}
// Raising sets the flags. A trap that is enabled is taken as the hardware
// would: setting a cause bit whose enable bit is set traps on the ctc1.
int __irix_feraiseexcept(int e) {
  unsigned int x = (unsigned)e & ALL, r = fcsr();
  set_fcsr(r | x);
  if ((r & ENABLES) & (x << 5))
    set_fcsr((r | x) | (x << 10)); // x's cause bits
  return 0;
}
int __irix_fesetexceptflag(const unsigned int *f, int e) {
  unsigned int m = (unsigned)e & ALL;
  set_fcsr((fcsr() & ~m) | (*f & m));
  return 0;
}
int __irix_fetestexcept(int e) { return (int)(fcsr() & (unsigned)e & ALL); }
int __irix_fegetround(void) { return (int)(fcsr() & 3); }
int __irix_fesetround(int r) {
  if (r & ~3)
    return -1;
  set_fcsr((fcsr() & ~3u) | (unsigned)r);
  return 0;
}
int __irix_fegetenv(unsigned int *env) {
  *env = fcsr();
  return 0;
}
int __irix_feholdexcept(unsigned int *env) {
  *env = fcsr();
  set_fcsr(*env & ~(ALL | ENABLES | CAUSES)); // clear flags, no traps
  return 0;
}
int __irix_fesetenv(const unsigned int *env) {
  set_fcsr(*env & ~CAUSES);
  return 0;
}
int __irix_feupdateenv(const unsigned int *env) {
  unsigned int raised = fcsr() & ALL;
  __irix_fesetenv(env);
  return __irix_feraiseexcept((int)raised);
}

#endif // __sgi
