//===-- irix/locale_l.c - POSIX 2008 *_l functions for IRIX ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The locale-taking character classification, case mapping and collation
// functions of POSIX 2008, which no IRIX release has; clang's IRIX <ctype.h>,
// <wchar.h> (IRIX's <wctype.h>) and <string.h> declare them. Programs that
// find locale_t (locale.c) take these to exist too: PostgreSQL does.
//
// A locale_t is LC_GLOBAL_LOCALE, for which these are IRIX's own functions
// and so follow setlocale, or one that newlocale made, which is always the C
// locale: there they classify and map the portable character set (ASCII)
// and nothing else, whatever the program's locale, and collate as strcmp.
//
//===----------------------------------------------------------------------===//

#if defined(__sgi)

#include <ctype.h>
#include <langinfo.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#pragma weak isalnum_l
#pragma weak isalpha_l
#pragma weak isblank_l
#pragma weak iscntrl_l
#pragma weak isdigit_l
#pragma weak isgraph_l
#pragma weak islower_l
#pragma weak isprint_l
#pragma weak ispunct_l
#pragma weak isspace_l
#pragma weak isupper_l
#pragma weak isxdigit_l
#pragma weak tolower_l
#pragma weak toupper_l
#pragma weak iswalnum_l
#pragma weak iswalpha_l
#pragma weak iswblank_l
#pragma weak iswcntrl_l
#pragma weak iswdigit_l
#pragma weak iswgraph_l
#pragma weak iswlower_l
#pragma weak iswprint_l
#pragma weak iswpunct_l
#pragma weak iswspace_l
#pragma weak iswupper_l
#pragma weak iswxdigit_l
#pragma weak towlower_l
#pragma weak towupper_l
#pragma weak strcoll_l
#pragma weak strxfrm_l
#pragma weak wcscoll_l
#pragma weak wcsxfrm_l
#pragma weak strftime_l
#pragma weak nl_langinfo_l

// The C locale's classes, for any value: EOF, WEOF and everything outside
// ASCII are in none of them.
static int c_upper(long c) { return c >= 'A' && c <= 'Z'; }
static int c_lower(long c) { return c >= 'a' && c <= 'z'; }
static int c_alpha(long c) { return c_upper(c) || c_lower(c); }
static int c_digit(long c) { return c >= '0' && c <= '9'; }
static int c_alnum(long c) { return c_alpha(c) || c_digit(c); }
static int c_xdigit(long c) {
  return c_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
static int c_blank(long c) { return c == ' ' || c == '\t'; }
static int c_space(long c) { return c == ' ' || (c >= '\t' && c <= '\r'); }
static int c_cntrl(long c) { return (c >= 0 && c < ' ') || c == 0x7f; }
static int c_print(long c) { return c >= ' ' && c < 0x7f; }
static int c_graph(long c) { return c > ' ' && c < 0x7f; }
static int c_punct(long c) { return c_graph(c) && !c_alnum(c); }
static long c_tolower(long c) { return c_upper(c) ? c - 'A' + 'a' : c; }
static long c_toupper(long c) { return c_lower(c) ? c - 'a' + 'A' : c; }

#define GLOBAL(loc) ((loc) == LC_GLOBAL_LOCALE)

int isalnum_l(int c, locale_t l) { return GLOBAL(l) ? isalnum(c) : c_alnum(c); }
int isalpha_l(int c, locale_t l) { return GLOBAL(l) ? isalpha(c) : c_alpha(c); }
int isblank_l(int c, locale_t l) { return GLOBAL(l) ? isblank(c) : c_blank(c); }
int iscntrl_l(int c, locale_t l) { return GLOBAL(l) ? iscntrl(c) : c_cntrl(c); }
int isdigit_l(int c, locale_t l) { return GLOBAL(l) ? isdigit(c) : c_digit(c); }
int isgraph_l(int c, locale_t l) { return GLOBAL(l) ? isgraph(c) : c_graph(c); }
int islower_l(int c, locale_t l) { return GLOBAL(l) ? islower(c) : c_lower(c); }
int isprint_l(int c, locale_t l) { return GLOBAL(l) ? isprint(c) : c_print(c); }
int ispunct_l(int c, locale_t l) { return GLOBAL(l) ? ispunct(c) : c_punct(c); }
int isspace_l(int c, locale_t l) { return GLOBAL(l) ? isspace(c) : c_space(c); }
int isupper_l(int c, locale_t l) { return GLOBAL(l) ? isupper(c) : c_upper(c); }
int isxdigit_l(int c, locale_t l) {
  return GLOBAL(l) ? isxdigit(c) : c_xdigit(c);
}
int tolower_l(int c, locale_t l) {
  return GLOBAL(l) ? tolower(c) : (int)c_tolower(c);
}
int toupper_l(int c, locale_t l) {
  return GLOBAL(l) ? toupper(c) : (int)c_toupper(c);
}

// wint_t is unsigned on IRIX: WEOF is outside ASCII, so in no C class.
int iswalnum_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswalnum(c) : c_alnum((long)c);
}
int iswalpha_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswalpha(c) : c_alpha((long)c);
}
int iswblank_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswblank(c) : c_blank((long)c);
}
int iswcntrl_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswcntrl(c) : c_cntrl((long)c);
}
int iswdigit_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswdigit(c) : c_digit((long)c);
}
int iswgraph_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswgraph(c) : c_graph((long)c);
}
int iswlower_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswlower(c) : c_lower((long)c);
}
int iswprint_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswprint(c) : c_print((long)c);
}
int iswpunct_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswpunct(c) : c_punct((long)c);
}
int iswspace_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswspace(c) : c_space((long)c);
}
int iswupper_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswupper(c) : c_upper((long)c);
}
int iswxdigit_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? iswxdigit(c) : c_xdigit((long)c);
}
wint_t towlower_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? towlower(c) : (wint_t)c_tolower((long)c);
}
wint_t towupper_l(wint_t c, locale_t l) {
  return GLOBAL(l) ? towupper(c) : (wint_t)c_toupper((long)c);
}

// The C locale collates by code point, and its transformed string is the
// string itself.
int strcoll_l(const char *a, const char *b, locale_t l) {
  return GLOBAL(l) ? strcoll(a, b) : strcmp(a, b);
}

size_t strxfrm_l(char *restrict dst, const char *restrict src, size_t n,
                 locale_t l) {
  if (GLOBAL(l))
    return strxfrm(dst, src, n);
  size_t len = strlen(src);
  if (len < n)
    memcpy(dst, src, len + 1);
  return len;
}

int wcscoll_l(const wchar_t *a, const wchar_t *b, locale_t l) {
  return GLOBAL(l) ? wcscoll(a, b) : wcscmp(a, b);
}

size_t wcsxfrm_l(wchar_t *restrict dst, const wchar_t *restrict src, size_t n,
                 locale_t l) {
  if (GLOBAL(l))
    return wcsxfrm(dst, src, n);
  size_t len = wcslen(src);
  if (len < n)
    wmemcpy(dst, src, len + 1);
  return len;
}

// strftime in the C locale: the conversions that depend on the locale (the
// day and month names, AM/PM, and %c, %x, %X and %r) are rewritten to the C
// locale's text, and the E and O modifiers dropped (the C locale has no
// alternatives), before IRIX's strftime does the rest.
static const char *const c_wday[7] = {"Sunday",   "Monday", "Tuesday",
                                      "Wednesday", "Thursday", "Friday",
                                      "Saturday"};
static const char *const c_month[12] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

static char *c_put(char *o, const char *str, size_t n) {
  size_t len = strlen(str);
  if (n > len)
    n = len;
  memcpy(o, str, n);
  return o + n;
}

size_t strftime_l(char *restrict s, size_t max, const char *restrict fmt,
                  const struct tm *restrict tm, locale_t l) {
  if (GLOBAL(l))
    return strftime(s, max, fmt, tm);
  // A conversion of two or three characters becomes at most 22 ("%c").
  char *f = malloc(strlen(fmt) * 11 + 1);
  if (!f)
    return 0;
  const char *wday =
      tm->tm_wday >= 0 && tm->tm_wday < 7 ? c_wday[tm->tm_wday] : "?";
  const char *mon =
      tm->tm_mon >= 0 && tm->tm_mon < 12 ? c_month[tm->tm_mon] : "?";
  const char *ampm = tm->tm_hour >= 12 ? "PM" : "AM";
  char *o = f;
  for (const char *p = fmt; *p;) {
    if (*p != '%' || !p[1]) {
      *o++ = *p++;
      continue;
    }
    const char *q = p + 1;
    if ((*q == 'E' || *q == 'O') && q[1])
      q++;
    switch (*q) {
    case 'a':
      o = c_put(o, wday, 3);
      break;
    case 'A':
      o = c_put(o, wday, 9);
      break;
    case 'b':
    case 'h':
      o = c_put(o, mon, 3);
      break;
    case 'B':
      o = c_put(o, mon, 9);
      break;
    case 'p':
      o = c_put(o, ampm, 2);
      break;
    case 'c':
      o = c_put(o, wday, 3);
      *o++ = ' ';
      o = c_put(o, mon, 3);
      o = c_put(o, " %e %H:%M:%S %Y", 15);
      break;
    case 'x':
      o = c_put(o, "%m/%d/%y", 8);
      break;
    case 'X':
      o = c_put(o, "%H:%M:%S", 8);
      break;
    case 'r':
      o = c_put(o, "%I:%M:%S ", 9);
      o = c_put(o, ampm, 2);
      break;
    default:
      *o++ = '%';
      *o++ = *q;
    }
    p = q + 1;
  }
  *o = '\0';
  size_t n = strftime(s, max, f, tm);
  free(f);
  return n;
}

// nl_langinfo in the C locale: POSIX's C locale values (glibc's). Not IRIX's
// own nl_langinfo, even when the program's locale is "C": IRIX's C locale
// has no day and month names there (it answers "C" for them).
static const char *const c_abday[7] = {"Sun", "Mon", "Tue", "Wed",
                                       "Thu", "Fri", "Sat"};
static const char *const c_abmon[12] = {"Jan", "Feb", "Mar", "Apr",
                                        "May", "Jun", "Jul", "Aug",
                                        "Sep", "Oct", "Nov", "Dec"};

static const char *c_langinfo(nl_item item) {
  if (item >= DAY_1 && item <= DAY_7)
    return c_wday[item - DAY_1];
  if (item >= ABDAY_1 && item <= ABDAY_7)
    return c_abday[item - ABDAY_1];
  if (item >= MON_1 && item <= MON_12)
    return c_month[item - MON_1];
  if (item >= ABMON_1 && item <= ABMON_12)
    return c_abmon[item - ABMON_1];
  switch (item) {
  case RADIXCHAR:
    return ".";
  case D_T_FMT:
    return "%a %b %e %H:%M:%S %Y";
  case D_FMT:
    return "%m/%d/%y";
  case T_FMT:
    return "%H:%M:%S";
  case T_FMT_AMPM:
    return "%I:%M:%S %p";
  case AM_STR:
    return "AM";
  case PM_STR:
    return "PM";
  case CODESET:
    return "ANSI_X3.4-1968";
  case YESEXPR:
    return "^[yY]";
  case NOEXPR:
    return "^[nN]";
  case CRNCYSTR:
    return "-";
  default: // THOUSEP, YESSTR, NOSTR, the eras, ALT_DIGITS
    return "";
  }
}

char *nl_langinfo_l(nl_item item, locale_t l) {
  if (GLOBAL(l))
    return nl_langinfo(item);
  return (char *)c_langinfo(item);
}

#endif // __sgi
