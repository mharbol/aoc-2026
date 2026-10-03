
#ifndef _AOC_LUA_TEMPLATE_STRING_H_
#define _AOC_LUA_TEMPLATE_STRING_H_

#include <algorithm>
#include <cstddef>

namespace aoc::lua {

template <size_t N>
struct TemplateString {
    char value[N];
    constexpr TemplateString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
};

} // namespace aoc::lua
#endif // _AOC_LUA_TEMPLATE_STRING_H_
