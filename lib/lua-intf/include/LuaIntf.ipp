
#include "LuaIntf.h"

extern "C" {
#include "lualib.h"
}

namespace aoc::lua {

inline bool LuaIntf::doString(const std::string &cmd) {
    return LUA_OK == luaL_dostring(L, cmd.c_str());
}

inline std::string LuaIntf::getHeadString() {
    size_t len;
    const char *str = lua_tolstring(L, -1, &len);
    return std::string(str, len);
}

inline void LuaIntf::luaPop(size_t pop) {
    lua_pop(L, pop);
}

inline void LuaIntf::openBaseLibs() {
    luaopen_base(L);
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
    luaPop(2);
}

template <class T>
std::expected<T, std::string> LuaIntf::getGlobal(const std::string &var) {
    std::expected<T, std::string> ret;
    lua_getglobal(L, var.c_str());
    if (lua_isnumber(L, -1)) {
        ret = static_cast<T>(lua_tonumber(L, -1));
    } else {
        ret = std::unexpected(lua_tostring(L, -1));
    }
    luaPop(1);
    return ret;
}
} // namespace aoc::lua
