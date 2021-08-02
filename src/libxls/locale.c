/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Copyright 2020 Evan Miller
 *
 * This file is part of libxls -- A multiplatform, C/C++ library for parsing
 * Excel(TM) files.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "config.h"
#include <stdlib.h>
#include "libxls/locale.h"

#ifdef __sun
#include <string.h>
#endif

#if defined(__MINGW32__)
  static char* old_locale;
#endif

xls_locale_t xls_createlocale() {
#if defined(__MINGW32__)
    old_locale = setlocale(LC_CTYPE, ".65001");
    return NULL;
#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(WINDOWS)
    return _create_locale(LC_CTYPE, ".65001");
#elif defined(__sun)
    return NULL;
#else
    return newlocale(LC_CTYPE_MASK, "C.UTF-8", NULL);
#endif
}

void xls_freelocale(xls_locale_t locale) {
#if defined(__MINGW32__)
    setlocale(LC_ALL, old_locale);
    return;
#endif

    if (!locale)
        return;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(WINDOWS)
    _free_locale(locale);
#elif defined(__sun)
   return;
#else
    freelocale(locale);
#endif
}

size_t xls_wcstombs_l(char *restrict s, const wchar_t *restrict pwcs, size_t n, xls_locale_t loc) {
#if defined(__MINGW32__)
    return wcstombs(s, pwcs, n);
#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(WINDOWS)
    return _wcstombs_l(s, pwcs, n, loc);
#elif defined(HAVE_WCSTOMBS_L)
    return wcstombs_l(s, pwcs, n, loc);
#elif defined(__sun)
    return wcstombs(s, pwcs, n);
#else
    locale_t oldlocale = uselocale(loc);
    size_t result = wcstombs(s, pwcs, n);
    uselocale(oldlocale);
    return result;
#endif
}
