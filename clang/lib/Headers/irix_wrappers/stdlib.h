/*===---- stdlib.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <stdlib.h> does not say that abort and exit never return, so code
 * ending in either reads as falling off the end of a function. Say so.
 *
 * Before 6.5.22 it also lacks C99's strtof: see below.
 *
 * C99's long long functions (atoll, strtoll, strtoull, llabs, lldiv, and
 * lldiv_t) are in IRIX's libc, but its <stdlib.h> declares them only for
 * its own API (_SGIAPI or _ABIAPI, outside ANSI mode): a program that
 * defines _XOPEN_SOURCE (ICU does) loses them, and libc++'s <stdlib.h>
 * needs lldiv_t. In C99 modes and C++, declare them whenever IRIX did not.
 */

#ifndef __CLANG_IRIX_STDLIB_H
#define __CLANG_IRIX_STDLIB_H

/* IRIX's <stdlib.h> includes <getopt.h>: tell the <getopt.h> wrapper, so that
 * getopt_long and struct option are only declared for a direct include. */
#ifndef __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT
#define __IRIX_GETOPT_INDIRECT_STDLIB
#endif
/* IRIX declares putenv(const char *) (6.5.7 always, 6.5.22 outside XPG5);
 * POSIX, glibc and the code written for them say putenv(char *), and in
 * C++ the two cannot both be declared (gnulib's checks in gnutls).  Read
 * IRIX's under another name; the POSIX one is declared below.  Same libc
 * function, same calling convention. */
#define putenv __irix_putenv_declaration
#include_next <stdlib.h>

/* alloca: glibc's and the BSDs' <stdlib.h> declare it outside strict ISO C
 * (and for C++ always, where g++ defines _GNU_SOURCE); IRIX's only in
 * <alloca.h>, which maps it to the compiler's builtin. */
#if defined(__cplusplus) || !defined(__STRICT_ANSI__)
#include <alloca.h>
#endif
#undef putenv
#ifdef __IRIX_GETOPT_INDIRECT_STDLIB
#undef __IRIX_GETOPT_INDIRECT
#undef __IRIX_GETOPT_INDIRECT_STDLIB
#endif

#ifdef __cplusplus
extern "C" {
#endif
void abort(void) __attribute__((__noreturn__));
void exit(int) __attribute__((__noreturn__));
/* BSD's, which IRIX's libc lacks: compiler-rt's irix/progname.c. */
const char *getprogname(void);
void setprogname(const char *);
/* OpenBSD's, from irix/compat_bsd2.c. */
long long strtonum(const char *, long long, long long, const char **);
/* POSIX 2001's, which IRIX's libc lacks: compiler-rt's irix/env.c. */
int setenv(const char *, const char *, int);
int unsetenv(const char *);
/* POSIX 2008's, from irix/misc.c. */
char *mkdtemp(char *);
/* POSIX 2001's posix_memalign, C11's aligned_alloc and POSIX 2024's
 * reallocarray and mkostemp, from irix/posix2008.c. */
int posix_memalign(void **, size_t, size_t);
void *aligned_alloc(size_t, size_t);
void *reallocarray(void *, size_t, size_t);
int mkostemp(char *, int);
#ifdef __cplusplus
}
#endif

/* C99's strtof: IRIX 6.5.7 has none, 6.5.22 declares one. Declare it as
 * compiler-rt's __irix_strtof (irix/strtof.c; asm label), which is right
 * whichever headers are present, where a definition of our own would clash
 * with 6.5.22's declaration. */
#if !defined(strtof)
#ifdef __cplusplus
extern "C" float strtof(const char *__restrict, char **__restrict)
    __asm__("__irix_strtof");
#else
extern float strtof(const char *__restrict, char **__restrict)
    __asm__("__irix_strtof");
#endif
#endif

/* clang's long double is a double on IRIX; IRIX's strtold and atold return
 * MIPSpro's, a pair of doubles. Use strtod and atof under their names. */
#ifdef __cplusplus
extern "C" {
#endif
extern long double strtold(const char *__restrict, char **__restrict)
    __asm__("strtod");
#if _COMPILER_VERSION >= 400
extern long double atold(const char *) __asm__("atof");
#endif
#ifdef __cplusplus
}
#endif

#if defined(__c99) && !((_SGIAPI || _ABIAPI) && _NO_ANSIMODE)
/* 6.5.22's headers (internal/) define it themselves in C99 mode. */
#if !__has_include(<internal/stdlib_core.h>)
typedef struct {
  long long quot;
  long long rem;
} lldiv_t;
#endif
#ifdef __cplusplus
extern "C" {
#endif
long long atoll(const char *);
long long strtoll(const char *__restrict, char **__restrict, int);
unsigned long long strtoull(const char *__restrict, char **__restrict, int);
long long llabs(long long);
lldiv_t lldiv(long long, long long);
#ifdef __cplusplus
}
#endif
#endif

/* POSIX's, in IRIX's libc, which its header declares only in SGI mode (and
 * some X/Open modes): declared here outside SGI mode, with IRIX's own
 * prototypes (found by compiling every POSIX header in six feature-macro
 * modes against the 6.5.7 and 6.5.22 headers). */
#if !_SGIAPI
#ifdef __cplusplus
extern "C" {
#endif
long a64l(const char *);
double drand48(void);
double erand48(unsigned short[3]);
int getsubopt(char **, char *const *, char **);
int grantpt(int);
char *initstate(unsigned int, char *, size_t);
long jrand48(unsigned short[3]);
char *l64a(long);
void lcong48(unsigned short[7]);
long lrand48(void);
int mkstemp(char *);
char *mktemp(char *);
long mrand48(void);
long nrand48(unsigned short[3]);
char *ptsname(int);
long random(void);
char *realpath(const char *, char *);
unsigned short *seed48(unsigned short[3]);
void setkey(const char *);
char *setstate(const char *);
void srand48(long);
void srandom(unsigned int);
int unlockpt(int);
void *valloc(size_t);
#ifdef __cplusplus
}
#endif
#endif
/* POSIX's putenv (see the top), in every mode, as glibc has it. */
#ifdef __cplusplus
extern "C" {
#endif
int putenv(char *);
#ifdef __cplusplus
}
#endif

/* _Exit (C99): in clang's IRIX runtime (compiler-rt). */
#ifdef __cplusplus
extern "C" {
#endif
void _Exit(int) __attribute__((__noreturn__));
#ifdef __cplusplus
}
#endif

#endif /* __CLANG_IRIX_STDLIB_H */
