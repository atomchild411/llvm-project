//===-----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___LOCALE_DIR_SUPPORT_IRIX_H
#define _LIBCPP___LOCALE_DIR_SUPPORT_IRIX_H

// IRIX 6.5 has setlocale but no per-thread locales (newlocale, uselocale),
// so every __locale_t here stands for the process's locale, and only "C",
// "POSIX" and "" (the environment's, which a program sets with setlocale)
// can be named. IRIX 6.5.7 also lacks C95's restartable conversions
// (mbrtowc and friends), which these build on the stateless mbtowc and
// wctomb it does have; its locales are stateless.

#include <__config>
#include <__cstddef/size_t.h>
#include <__utility/forward.h>
#include <clocale>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#if _LIBCPP_HAS_WIDE_CHARACTERS
#  include <cwchar>
#  include <wctype.h>
#endif

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD
namespace __locale {

// Masks for __newlocale; IRIX has no LC_*_MASK.
#define _LIBCPP_COLLATE_MASK (1 << LC_COLLATE)
#define _LIBCPP_CTYPE_MASK (1 << LC_CTYPE)
#define _LIBCPP_MONETARY_MASK (1 << LC_MONETARY)
#define _LIBCPP_NUMERIC_MASK (1 << LC_NUMERIC)
#define _LIBCPP_TIME_MASK (1 << LC_TIME)
#define _LIBCPP_MESSAGES_MASK (1 << LC_MESSAGES)
#define _LIBCPP_ALL_MASK                                                                                                \
  (_LIBCPP_COLLATE_MASK | _LIBCPP_CTYPE_MASK | _LIBCPP_MONETARY_MASK | _LIBCPP_NUMERIC_MASK | _LIBCPP_TIME_MASK |      \
   _LIBCPP_MESSAGES_MASK)
#define _LIBCPP_LC_ALL LC_ALL

struct __irix_locale {
  int __unused_;
};
using __locale_t _LIBCPP_NODEBUG = __irix_locale*;

#if defined(_LIBCPP_BUILDING_LIBRARY)
using __lconv_t _LIBCPP_NODEBUG = lconv;

inline _LIBCPP_HIDE_FROM_ABI __locale_t __newlocale(int, const char* __name, __locale_t) {
  static __irix_locale __c_locale;
  if (__name == nullptr) {
    errno = EINVAL;
    return nullptr;
  }
  if (__name[0] == '\0' || std::strcmp(__name, "C") == 0 || std::strcmp(__name, "POSIX") == 0)
    return &__c_locale;
  errno = ENOENT;
  return nullptr;
}

inline _LIBCPP_HIDE_FROM_ABI void __freelocale(__locale_t) {}

inline _LIBCPP_HIDE_FROM_ABI char* __setlocale(int __category, char const* __locale) {
  return ::setlocale(__category, __locale);
}

inline _LIBCPP_HIDE_FROM_ABI __lconv_t* __localeconv(__locale_t&) { return ::localeconv(); }

inline _LIBCPP_HIDE_FROM_ABI int __mb_len_max(__locale_t) { return MB_CUR_MAX; }

#  if _LIBCPP_HAS_WIDE_CHARACTERS
inline _LIBCPP_HIDE_FROM_ABI wint_t __btowc(int __ch, __locale_t) { return ::btowc(__ch); }
inline _LIBCPP_HIDE_FROM_ABI int __wctob(wint_t __ch, __locale_t) { return ::wctob(__ch); }

inline _LIBCPP_HIDE_FROM_ABI size_t __mbrtowc(wchar_t* __pwc, const char* __s, size_t __n, mbstate_t*, __locale_t) {
  if (__s == nullptr)
    return 0;
  if (__n == 0)
    return static_cast<size_t>(-2);
  wchar_t __wc;
  int __r = ::mbtowc(&__wc, __s, __n);
  if (__r < 0) {
    errno = EILSEQ;
    return static_cast<size_t>(-1);
  }
  if (__pwc)
    *__pwc = __wc;
  return static_cast<size_t>(__r);
}

inline _LIBCPP_HIDE_FROM_ABI int __mbtowc(wchar_t* __pwc, const char* __pmb, size_t __max, __locale_t) {
  return ::mbtowc(__pwc, __pmb, __max);
}

inline _LIBCPP_HIDE_FROM_ABI size_t __mbrlen(const char* __s, size_t __n, mbstate_t* __ps, __locale_t __loc) {
  return __locale::__mbrtowc(nullptr, __s, __n, __ps, __loc);
}

inline _LIBCPP_HIDE_FROM_ABI size_t __wcrtomb(char* __s, wchar_t __wc, mbstate_t*, __locale_t) {
  char __buf[MB_LEN_MAX];
  int __r = ::wctomb(__s ? __s : __buf, __wc);
  if (__r < 0) {
    errno = EILSEQ;
    return static_cast<size_t>(-1);
  }
  return static_cast<size_t>(__r);
}

// Up to __nms bytes of *__src into at most __len wide characters (any
// number when __dest is null); *__src is left after the last one converted,
// or null at a terminating NUL.
inline _LIBCPP_HIDE_FROM_ABI size_t
__mbsnrtowcs(wchar_t* __dest, const char** __src, size_t __nms, size_t __len, mbstate_t*, __locale_t) {
  const char* __s = *__src;
  size_t __n      = 0;
  while (__dest == nullptr || __n < __len) {
    if (__nms == 0)
      break;
    wchar_t __wc;
    int __r = ::mbtowc(&__wc, __s, __nms);
    if (__r < 0) {
      errno = EILSEQ;
      if (__dest)
        *__src = __s;
      return static_cast<size_t>(-1);
    }
    if (__r == 0) {
      if (__dest) {
        __dest[__n] = L'\0';
        *__src      = nullptr;
      }
      return __n;
    }
    if (__dest)
      __dest[__n] = __wc;
    __s += __r;
    __nms -= static_cast<size_t>(__r);
    ++__n;
  }
  if (__dest)
    *__src = __s;
  return __n;
}

inline _LIBCPP_HIDE_FROM_ABI size_t
__mbsrtowcs(wchar_t* __dest, const char** __src, size_t __len, mbstate_t* __ps, __locale_t __loc) {
  return __locale::__mbsnrtowcs(__dest, __src, static_cast<size_t>(-1), __len, __ps, __loc);
}

// Up to __nwc wide characters of *__src into at most __len bytes (any
// number when __dest is null).
inline _LIBCPP_HIDE_FROM_ABI size_t
__wcsnrtombs(char* __dest, const wchar_t** __src, size_t __nwc, size_t __len, mbstate_t*, __locale_t) {
  const wchar_t* __s = *__src;
  size_t __n         = 0;
  for (; __nwc > 0; --__nwc, ++__s) {
    char __buf[MB_LEN_MAX];
    int __r = ::wctomb(__buf, *__s);
    if (__r < 0) {
      errno = EILSEQ;
      if (__dest)
        *__src = __s;
      return static_cast<size_t>(-1);
    }
    if (__dest) {
      if (__n + static_cast<size_t>(__r) > __len) {
        *__src = __s;
        return __n;
      }
      std::memcpy(__dest + __n, __buf, static_cast<size_t>(__r));
    }
    if (*__s == L'\0') {
      if (__dest)
        *__src = nullptr;
      return __n; // the NUL is not counted
    }
    __n += static_cast<size_t>(__r);
  }
  if (__dest)
    *__src = __s;
  return __n;
}
#  endif // _LIBCPP_HAS_WIDE_CHARACTERS
#endif   // _LIBCPP_BUILDING_LIBRARY

//
// Strtonum functions: IRIX 6.5.7 has no strtof.
//
inline _LIBCPP_HIDE_FROM_ABI float __strtof(const char* __nptr, char** __endptr, __locale_t) {
  return static_cast<float>(::strtod(__nptr, __endptr));
}
inline _LIBCPP_HIDE_FROM_ABI double __strtod(const char* __nptr, char** __endptr, __locale_t) {
  return ::strtod(__nptr, __endptr);
}
inline _LIBCPP_HIDE_FROM_ABI long double __strtold(const char* __nptr, char** __endptr, __locale_t) {
  return ::strtold(__nptr, __endptr);
}
inline _LIBCPP_HIDE_FROM_ABI long long __strtoll(const char* __nptr, char** __endptr, int __base, __locale_t) {
  return ::strtoll(__nptr, __endptr, __base);
}
inline _LIBCPP_HIDE_FROM_ABI unsigned long long
__strtoull(const char* __nptr, char** __endptr, int __base, __locale_t) {
  return ::strtoull(__nptr, __endptr, __base);
}

//
// Formatted output and input.
//
_LIBCPP_DIAGNOSTIC_PUSH
_LIBCPP_CLANG_DIAGNOSTIC_IGNORED("-Wgcc-compat")
_LIBCPP_GCC_DIAGNOSTIC_IGNORED("-Wformat-nonliteral")
#ifdef _LIBCPP_COMPILER_CLANG_BASED
#  define _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT(...) _LIBCPP_ATTRIBUTE_FORMAT(__VA_ARGS__)
#else
#  define _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT(...) /* nothing */
#endif

template <class... _Args>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT(__printf__, 4, 5) int __snprintf(
    char* __s, size_t __n, __locale_t, const char* __format, _Args&&... __args) {
  return std::snprintf(__s, __n, __format, std::forward<_Args>(__args)...);
}

// IRIX has no asprintf, and its pre-C99 snprintf does not report the length
// it needed when it truncates, so grow the buffer until the output fits.
template <class... _Args>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT(__printf__, 3, 4) int __asprintf(
    char** __s, __locale_t, const char* __format, _Args&&... __args) {
  size_t __size = 128;
  for (;;) {
    char* __buf = static_cast<char*>(std::malloc(__size));
    if (!__buf) {
      *__s = nullptr;
      return -1;
    }
    int __r = std::snprintf(__buf, __size, __format, __args...);
    if (__r >= 0 && static_cast<size_t>(__r) < __size - 1) {
      *__s = __buf;
      return __r;
    }
    std::free(__buf);
    __size *= 2;
  }
}

template <class... _Args>
_LIBCPP_HIDE_FROM_ABI _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT(__scanf__, 3, 4) int __sscanf(
    const char* __s, __locale_t, const char* __format, _Args&&... __args) {
  return std::sscanf(__s, __format, std::forward<_Args>(__args)...);
}

_LIBCPP_DIAGNOSTIC_POP
#undef _LIBCPP_VARIADIC_ATTRIBUTE_FORMAT

} // namespace __locale
_LIBCPP_END_NAMESPACE_STD

#include <__locale_dir/support/no_locale/characters.h>

#endif // _LIBCPP___LOCALE_DIR_SUPPORT_IRIX_H
