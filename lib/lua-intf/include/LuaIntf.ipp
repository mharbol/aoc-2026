
#include "LuaIntf.h"
#include <string>

extern "C" {
#include "lualib.h"
}

namespace aoc::lua {

inline bool LuaIntf::luaDoString(const std::string &cmd) {
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

inline void LuaIntf::appendPath(const std::string &path) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    lua_pushstring(L, path.c_str());
    lua_concat(L, 2);
    lua_setfield(L, -2, "path");
    luaPop(1);
}

template <>
std::expected<std::string, std::string> LuaIntf::getGlobal(const std::string &var) {
    std::expected<std::string, std::string> ret;
    lua_getglobal(L, var.c_str());
    if (lua_isstring(L, -1)) {
        ret = lua_tostring(L, -1);
    } else {
        ret = std::unexpected(lua_tostring(L, -1));
    }
    luaPop(1);
    return ret;
}

template <typename T>
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
