//===-- irix/lgamma_r.c - lgamma_r and lgammaf_r for IRIX -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// lgamma_r and lgammaf_r (glibc's, the BSDs', Solaris'), which IRIX's libm
// lacks: it has lgamma and lgammaf, which leave the sign of Gamma(x) in
// signgam. Not reentrant with respect to signgam itself, as IRIX's lgamma
// is not; libc++'s std::binomial_distribution and std::lgamma callers only
// need the value and sign. clang's IRIX <math.h> declares them.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

extern int signgam;
double lgamma(double);
float lgammaf(float);

double lgamma_r(double x, int *sign) {
  double r = lgamma(x);
  *sign = signgam;
  return r;
}

float lgammaf_r(float x, int *sign) {
  float r = lgammaf(x);
  *sign = signgam;
  return r;
}

#endif
