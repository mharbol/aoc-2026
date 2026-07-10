
#include "LuaIntf.h"
#include <expected>

extern "C" {
#include "lua.h"
}

namespace aoc::lua {

template <>
std::expected<float, std::string> LuaIntf::getGlobal(const std::string &var) {
    return 12;
}

} // namespace aoc::lua
