
#include "LuaIntf.h"
#include <expected>

extern "C" {
#include "lua.h"
}

namespace aoc::lua {

template <>
std::expected<float, std::string> LuaIntf::getGlobal(const std::string &var) {
    std::expected<float, std::string> ret;
    if (lua_isnumber(L, -1)) {
        ret = static_cast<float>(lua_tonumber(L, -1));
    } else {
        ret = std::unexpected(lua_tostring(L, -1));
    }
    luaPop(1);
    return ret;
}

} // namespace aoc::lua
