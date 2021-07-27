#ifndef __RAWJSON_H__
#define __RAWJSON_H__

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

/* compiler version check (GCC) */
#ifndef __GNUC__
#define __GNUC__ 0
#endif

/* compiler builtin check (since gcc 10.0, clang 2.6, icc 2021) */
#ifndef rawjson_has_builtin
#ifdef __has_builtin
#define rawjson_has_builtin(x) __has_builtin(x)
#else
#define rawjson_has_builtin(x) 0
#endif
#endif

/* compiler attribute check (since gcc 5.0, clang 2.9, icc 17) */
#ifndef rawjson_has_attribute
#ifdef __has_attribute
#define rawjson_has_attribute(x) __has_attribute(x)
#else
#define rawjson_has_attribute(x) 0
#endif
#endif

/* inline */
#ifndef rawjson_inline
#if rawjson_has_attribute(always_inline) || YYJSON_GCC_VER >= 4
#define rawjson_inline __inline__ __attribute__((__always_inline__))
#elif defined(__clang__) || defined(__GNUC__)
#define rawjson_inline __inline__
#else
#define rawjson_inline
#endif
#endif

/* noinline */
#ifndef rawjson_noinline
#if rawjson_has_attribute(noinline) || YYJSON_GCC_VER >= 4
#define rawjson_noinline __attribute__((noinline))
#else
#define rawjson_noinline
#endif
#endif

/* align */
#ifndef rawjson_align
#if rawjson_has_attribute(aligned) || defined(__GNUC__)
#define rawjson_align(x) __attribute__((aligned(x)))
#else
#define rawjson_align(x)
#endif
#endif

/* likely */
#ifndef rawjson_likely
#if rawjson_has_builtin(__builtin_expect) || __GNUC__ >= 4
#define rawjson_likely(expr) __builtin_expect(!!(expr), 1)
#else
#define rawjson_likely(expr) (expr)
#endif
#endif

/* unlikely */
#ifndef rawjson_unlikely
#if rawjson_has_builtin(__builtin_expect) || __GNUC__ >= 4
#define rawjson_unlikely(expr) __builtin_expect((expr), 0)
#else
#define rawjson_unlikely(expr) (expr)
#endif
#endif

#endif