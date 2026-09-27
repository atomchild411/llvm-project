/*===---- wchar.h - IRIX wrapper --------------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * IRIX's <wchar.h> names its extra character classes _E1 to _E6, names C++
 * libraries use for template parameters (libc++'s expected<_T2, _E2>). For
 * C++, spell out the class masks built from them, and drop the names.
 */

#ifndef __CLANG_IRIX_WCHAR_H
#define __CLANG_IRIX_WCHAR_H

#include_next <wchar.h>

#if defined(__cplusplus) && defined(_E1)
#undef _ISwprint
#undef _ISwgraph
#undef _ISwphonogram
#undef _ISwideogram
#undef _ISwenglish
#undef _ISwnumber
#undef _ISwspecial
#undef _ISwother
#define _ISwprint (_ISprint | 0x00003300)
#define _ISwgraph (_ISgraph | 0x00003300)
#define _ISwphonogram 0x00000100
#define _ISwideogram 0x00000200
#define _ISwenglish 0x00000400
#define _ISwnumber 0x00000800
#define _ISwspecial 0x00001000
#define _ISwother 0x00002000
#undef _E1
#undef _E2
#undef _E3
#undef _E4
#undef _E5
#undef _E6
#endif

#endif /* __CLANG_IRIX_WCHAR_H */
