//===-- irix/math_c99.c - C99 math functions IRIX 6.5.7 lacks -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// C99 <math.h> functions that IRIX 6.5.22's libc has and 6.5.7's does not:
// nan, nearbyint, fmin, fmax and float forms of frexp, ldexp, fabs, ilogb,
// logb and nextafter; and C23's roundeven, which no IRIX has. clang's IRIX
// <math.h> declares them. A program built against 6.5.7 takes these; one
// linked against 6.5.22's libc gets libc's, since an archive member is only
// taken for a symbol nothing else defines.
//
// The float forms go through the double ones, which are exact for every
// float argument, so each result is rounded once.
//
// Its own file, so a program is only given these when it asks for them.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <math.h>
#include <stdint.h>
#include <string.h>

double nan(const char *tag) {
  (void)tag;
  return __builtin_nan("");
}

float nanf(const char *tag) {
  (void)tag;
  return __builtin_nanf("");
}

// Like rint, which may also raise the inexact flag; nothing here reads it.
double nearbyint(double x) { return rint(x); }
float nearbyintf(float x) { return rintf(x); }

// C99 F.9.9.2: a NaN argument is ignored; fmax(-0, +0) is +0.
double fmax(double x, double y) {
  if (x != x)
    return y;
  if (y != y)
    return x;
  if (x == y)
    return __builtin_signbit(x) ? y : x;
  return x > y ? x : y;
}

double fmin(double x, double y) {
  if (x != x)
    return y;
  if (y != y)
    return x;
  if (x == y)
    return __builtin_signbit(x) ? x : y;
  return x < y ? x : y;
}

float fmaxf(float x, float y) { return (float)fmax(x, y); }
float fminf(float x, float y) { return (float)fmin(x, y); }

float frexpf(float x, int *e) { return (float)frexp(x, e); }
float ldexpf(float x, int n) { return (float)ldexp(x, n); }
float fabsf(float x) { return __builtin_fabsf(x); }
int ilogbf(float x) { return ilogb(x); }
float logbf(float x) { return (float)logb(x); }

float nextafterf(float x, float y) {
  uint32_t u;
  if (x != x || y != y)
    return x + y;
  if (x == y)
    return y;
  if (x == 0.0f) {
    // The smallest subnormal, with y's sign.
    u = 1;
    memcpy(&x, &u, sizeof x);
    return y > 0.0f ? x : -x;
  }
  memcpy(&u, &x, sizeof u);
  if ((x < y) == (x > 0.0f))
    u++;
  else
    u--;
  memcpy(&x, &u, sizeof x);
  return x;
}

// C23: to nearest, ties to even, whatever the rounding mode.
double roundeven(double x) {
  double t = trunc(x);
  double d = x - t;
  if (d == 0.5 || d == -0.5)
    return fmod(t, 2.0) == 0.0 ? t : t + (x > 0 ? 1.0 : -1.0);
  return round(x);
}

float roundevenf(float x) { return (float)roundeven(x); }

#endif // defined(__sgi)
