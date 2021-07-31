/*  Copyright (c) 2016 Michael Hansen

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE. */

#ifndef _ST_CONFIG_H
#define _ST_CONFIG_H

#define ST_MAJOR_VERSION 3
#define ST_MINOR_VERSION 4
#define ST_VERSION ((ST_MAJOR_VERSION * 100) + ST_MINOR_VERSION)
#define ST_VERSION_STR "3.4"

#define ST_HAVE_INT64
#if (__cplusplus > 201103L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201103L))
#define ST_HAVE_DEPRECATED_ATTR
#endif
#if (__cplusplus > 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201402L))
#define ST_HAVE_NODISCARD_ATTR
#define ST_HAVE_CXX17_STRING_VIEW
#define ST_HAVE_CXX17_FILESYSTEM
#if (__cplusplus > 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L))
//#define ST_HAVE_CXX20_U8_FSPATH
#endif
#endif
#if (__cplusplus > 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L))
#cmakedefine ST_HAVE_CXX20_CHAR8_TYPES
#endif

#define ST_ENABLE_STL_STRINGS
#define ST_ENABLE_STL_FILESYSTEM

#define ST_ENUM_CONSTANT(type, name) constexpr type name = type::name

#if defined(_MSC_VER)
#   define ST_DEPRECATED(message) __declspec(deprecated(message))
#elif defined(__GNUC__)
#   define ST_DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(ST_HAVE_DEPRECATED_ATTR)
#   define ST_DEPRECATED(message) [[deprecated(message)]]
#else
#   define ST_DEPRECATED(message)
#endif

#ifndef ST_DEPRECATED_VERSION
#   if defined(ST_NO_3_0_DEPRECATION)
#       define ST_DEPRECATED_VERSION    299
#   else
#       define ST_DEPRECATED_VERSION    ST_VERSION
#   endif
#endif

#if ST_DEPRECATED_VERSION >= 300
#   define ST_DEPRECATED_IN_3_0(message) ST_DEPRECATED(message)
#else
#   define ST_DEPRECATED_IN_3_0(message)
#endif

#if ST_DEPRECATED_VERSION >= 304
#   define ST_DEPRECATED_IN_3_4(message) ST_DEPRECATED(message)
#else
#   define ST_DEPRECATED_IN_3_4(message)
#endif

#if ST_DEPRECATED_VERSION >= 400
#   define ST_DEPRECATED_IN_4_0(message) ST_DEPRECATED(message)
#else
#   define ST_DEPRECATED_IN_4_0(message)
#endif

#if defined(ST_HAVE_NODISCARD_ATTR)
#   define ST_NODISCARD [[nodiscard]]
#else
#   define ST_NODISCARD
#endif

#define ST_MAX_SSO_LENGTH       (16)
#define ST_MAX_SSO_SIZE         (48)
#define ST_STACK_STRING_SIZE    (256)

// MSVC doesn't provide ssize_t
#ifdef _MSC_VER
#   ifdef _WIN64
        typedef __int64 ST_ssize_t;
#   else
        typedef int ST_ssize_t;
#   endif
#else
#   include <sys/types.h>
    typedef ssize_t ST_ssize_t;
#endif

#endif // _ST_CONFIG_H
