
#include "LuaIntf.h"
#include "lua.h"
#include <expected>
#include <string>
#include <type_traits>
#include <utility>

extern "C" {
#include "lualib.h"
}

namespace aoc::lua {

template <typename...>
static constexpr bool dependent_false_v = false;

inline bool LuaIntf::luaDoString(const std::string &cmd) {
    return LUA_OK == luaL_dostring(L, cmd.c_str());
}

inline std::string LuaIntf::getHeadAsString() {
    size_t len;
    const char *str = lua_tolstring(L, -1, &len);
    return std::string(str, len);
}

inline void LuaIntf::luaPop(size_t pop) {
    lua_pop(L, pop);
}

inline size_t LuaIntf::getStackSize() {
    return static_cast<size_t>(lua_gettop(L));
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

// TODO, improve this with better template practices
template <typename T>
void LuaIntf::pushStack(T &&var) {
    if constexpr (std::is_same_v<T, bool>) {
        lua_pushboolean(L, std::forward<T>(var));
    }
    if constexpr (std::is_integral_v<T>) {
        lua_pushinteger(L, std::forward<T>(var));
    } else if constexpr (std::is_floating_point_v<T>) {
        lua_pushnumber(L, std::forward<T>(var));
    } else if constexpr (std::is_floating_point_v<T>) {
        lua_pushnumber(L, std::forward<T>(var));
    } else {
        static_assert(dependent_false_v<T>, "Unsupported type");
    }
}

template <typename T>
    requires std::same_as<std::decay_t<T>, std::string>
void LuaIntf::pushStack(T &&var) {
    lua_pushlstring(L, var.c_str(), var.size());
}

template <typename T>
    requires std::same_as<std::decay_t<T>, char *> ||
             std::same_as<std::decay_t<T>, const char *>
void LuaIntf::pushStack(T &&var) {
    lua_pushstring(L, var);
}

// right now just single return of number type
// and number args
template <typename ReturnT, typename... Args>
std::expected<ReturnT, std::string> LuaIntf::call(const std::string &function, Args... args) {

    std::expected<ReturnT, std::string> ret;

    lua_getglobal(L, function.c_str());
    if (!lua_isfunction(L, -1)) {
        ret = std::unexpected(lua_tostring(L, -1));
        luaPop(1);
        return ret;
    }

    constexpr size_t nargs = sizeof...(Args);
    constexpr size_t nrets = 1; // TODO

    // this will be replaced with pushStack()
    ((lua_pushnumber(L, args)), ...);

    if (LUA_OK != lua_pcall(L, nargs, 1, 0)) {
        ret = std::unexpected(lua_tostring(L, -1));
        luaPop(1);
        return ret;
    } else if (lua_isnumber(L, -1)) { // figure out validation on this one...
        // also figure out multi-return for tuples
        ret = static_cast<ReturnT>(lua_tonumber(L, -1));
        luaPop(1);
        return ret;
    } else {
        ret = std::unexpected(lua_tostring(L, -1));
        luaPop(nrets);
        return ret;
    }
}
} // namespace aoc::lua
