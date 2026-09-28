/*===---- assert.h - IRIX wrapper -------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * C requires <assert.h> to define assert anew on every inclusion, according
 * to NDEBUG at that point. IRIX's has an include guard, so after an
 * `#undef assert` (gnulib's own assert.h replacement makes one) including it
 * again defines nothing, and assert(...) compiles as a call to a function
 * that does not exist (libintl ended up needing a symbol `assert`). This
 * wrapper has no guard: it lets IRIX's header declare what it declares, then
 * defines assert itself every time, calling IRIX libc's __assert as IRIX's
 * own macro does. It also gives C11 its static_assert spelling.
 */

#include_next <assert.h>

#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#ifdef __cplusplus
extern "C" void __assert(const char *, const char *, int);
#else
extern void __assert(const char *, const char *, int);
#endif
#define assert(e) ((e) ? (void)0 : __assert(#e, __FILE__, __LINE__))
#endif

#if !defined(__cplusplus) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 201112L && __STDC_VERSION__ < 202311L && \
    !defined(static_assert)
#define static_assert _Static_assert
#endif
