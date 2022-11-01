#ifndef ALGUTILS_UTILITY_HPP_
#define ALGUTILS_UTILITY_HPP_

/* assertion utilities (to make it possible to use assertions in functions made constexpr if NDEBUG is defined) */
#include <cassert>
#ifndef NDEBUG
#define NDEBUG_CONSTEXPR
#define UTIL_ASSERT(expr) assert(expr);
#else
#define NDEBUG_CONSTEXPR constexpr
#define UTIL_ASSERT(expr)
#endif

#endif
