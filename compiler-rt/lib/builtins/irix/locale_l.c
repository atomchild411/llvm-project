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
#include <locale.h>
#include <string.h>
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

#endif // __sgi
