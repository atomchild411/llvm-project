//===-- irix/fnmatch.c - fnmatch with FNM_CASEFOLD for IRIX ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// fnmatch with the BSDs' and glibc's FNM_CASEFOLD and FNM_LEADING_DIR, which
// IRIX's lacks; clang's IRIX <fnmatch.h> defines them and binds fnmatch
// here. A call without them is IRIX's own; one with them is matched here,
// with POSIX's rules otherwise: ? and * match any character but a / under
// FNM_PATHNAME, and not a leading period under FNM_PERIOD; brackets take
// ranges, ! or ^ for the complement and the [:class:] names, and a bracket
// with no closing ] is an ordinary [; a backslash quotes the next character
// unless FNM_NOESCAPE. FNM_CASEFOLD compares without case (in the C
// locale's sense, through tolower), FNM_LEADING_DIR also accepts a string
// that goes on past the pattern with a /. No match is IRIX's FNM_NOMATCH.
//
// Its own file, so a program is only given it when it asks for it.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <ctype.h>
#include <fnmatch.h>
#include <string.h>

extern int __irix_libc_fnmatch_fn(const char *, const char *, int)
    __asm__("fnmatch");

#define OWN_FLAGS (FNM_CASEFOLD | FNM_LEADING_DIR)

static int fold(int flags, unsigned char c) {
  return (flags & FNM_CASEFOLD) ? tolower(c) : c;
}

static int leading_period(const char *s, const char *begin, int flags) {
  return (flags & FNM_PERIOD) && *s == '.' &&
         (s == begin || ((flags & FNM_PATHNAME) && s[-1] == '/'));
}

static int in_class(const char *name, size_t len, unsigned char c) {
#define CLASS(n, f)                                                            \
  if (len == sizeof(n) - 1 && !memcmp(name, n, len))                           \
    return f(c) != 0;
  CLASS("alnum", isalnum)
  CLASS("alpha", isalpha)
  CLASS("blank", isblank)
  CLASS("cntrl", iscntrl)
  CLASS("digit", isdigit)
  CLASS("graph", isgraph)
  CLASS("lower", islower)
  CLASS("print", isprint)
  CLASS("punct", ispunct)
  CLASS("space", isspace)
  CLASS("upper", isupper)
  CLASS("xdigit", isxdigit)
#undef CLASS
  return 0;
}

// Matches c against the bracket expression at p (just past its [). Returns
// the pattern just past its ], or 0 if it has none (then [ is ordinary).
static const char *bracket(const char *p, unsigned char c, int flags,
                           int *matched) {
  int negate = 0, ok = 0, first = 1;
  int fc = fold(flags, c);
  if (*p == '!' || *p == '^') {
    negate = 1;
    p++;
  }
  for (;; first = 0) {
    unsigned char lo, hi;
    if (*p == '\0')
      return 0;
    if (*p == ']' && !first)
      break;
    if (p[0] == '[' && p[1] == ':') {
      const char *end = strstr(p + 2, ":]");
      if (!end)
        return 0;
      if (in_class(p + 2, (size_t)(end - p - 2), c))
        ok = 1;
      p = end + 2;
      continue;
    }
    if (*p == '\\' && !(flags & FNM_NOESCAPE) && p[1] != '\0')
      p++;
    lo = (unsigned char)*p++;
    hi = lo;
    if (p[0] == '-' && p[1] != ']' && p[1] != '\0') {
      p++;
      if (*p == '\\' && !(flags & FNM_NOESCAPE) && p[1] != '\0')
        p++;
      hi = (unsigned char)*p++;
    }
    if (lo <= c && c <= hi)
      ok = 1;
    else if ((flags & FNM_CASEFOLD) &&
             ((lo <= toupper(fc) && toupper(fc) <= hi) ||
              (lo <= fc && fc <= hi)))
      ok = 1;
  }
  *matched = ok != negate;
  return p + 1;
}

static int match(const char *p, const char *s, const char *begin, int flags) {
  for (;;) {
    unsigned char c = (unsigned char)*p++;
    switch (c) {
    case '\0':
      if (*s == '\0' || ((flags & FNM_LEADING_DIR) && *s == '/'))
        return 0;
      return FNM_NOMATCH;
    case '?':
      if (*s == '\0' || ((flags & FNM_PATHNAME) && *s == '/') ||
          leading_period(s, begin, flags))
        return FNM_NOMATCH;
      s++;
      break;
    case '*':
      while (*p == '*')
        p++;
      if (leading_period(s, begin, flags))
        return FNM_NOMATCH;
      if (*p == '\0') {
        if (!(flags & FNM_PATHNAME) || (flags & FNM_LEADING_DIR) ||
            !strchr(s, '/'))
          return 0;
        return FNM_NOMATCH;
      }
      if (*p == '/' && (flags & FNM_PATHNAME)) {
        s = strchr(s, '/');
        if (!s)
          return FNM_NOMATCH;
        break;
      }
      for (;; s++) {
        if (match(p, s, begin, flags) == 0)
          return 0;
        if (*s == '\0' || ((flags & FNM_PATHNAME) && *s == '/'))
          return FNM_NOMATCH;
      }
    case '[': {
      int matched;
      const char *after;
      if (*s == '\0' || ((flags & FNM_PATHNAME) && *s == '/') ||
          leading_period(s, begin, flags))
        return FNM_NOMATCH;
      after = bracket(p, (unsigned char)*s, flags, &matched);
      if (after) {
        if (!matched)
          return FNM_NOMATCH;
        p = after;
        s++;
        break;
      }
      // No closing ]: an ordinary [.
      if (*s != '[')
        return FNM_NOMATCH;
      s++;
      break;
    }
    case '\\':
      if (!(flags & FNM_NOESCAPE) && *p != '\0')
        c = (unsigned char)*p++;
      // Fall through.
    default:
      if (fold(flags, c) != fold(flags, (unsigned char)*s))
        return FNM_NOMATCH;
      s++;
      break;
    }
  }
}

int __irix_fnmatch(const char *pattern, const char *string, int flags) {
  if (!(flags & OWN_FLAGS))
    return __irix_libc_fnmatch_fn(pattern, string, flags);
  return match(pattern, string, string, flags);
}

#endif // defined(__sgi)
