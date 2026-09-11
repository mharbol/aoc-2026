
#include "LuaIntf.h"
#include <expected>
#include <string>
#include <tuple>
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

template <typename T>
    requires std::is_null_pointer_v<T> || std::is_integral_v<T> || std::is_floating_point_v<T>
void LuaIntf::pushStack(const T var) {
    if constexpr (std::is_null_pointer_v<T>) {
        lua_pushnil(L);
    } else if constexpr (std::is_same_v<T, bool>) {
        lua_pushboolean(L, var);
    } else if constexpr (std::is_integral_v<T>) {
        lua_pushinteger(L, var);
    } else if constexpr (std::is_floating_point_v<T>) {
        lua_pushnumber(L, var);
    } else {
        static_assert(dependent_false_v<T>, "Unsupported type");
    }
}

template <typename T>
    requires std::same_as<std::decay_t<T>, std::string>
void LuaIntf::pushStack(const T &var) {
    lua_pushlstring(L, var.c_str(), var.size());
}

template <typename T>
    requires std::same_as<std::decay_t<T>, char *> ||
             std::same_as<std::decay_t<T>, const char *>
void LuaIntf::pushStack(const T var) {
    lua_pushstring(L, var);
}

// right now just single return
template <typename ReturnT, typename... Args>
std::expected<ReturnT, std::string> LuaIntf::call(const std::string &function, Args... args) {

    constexpr size_t nargs = sizeof...(Args);
    constexpr size_t nrets = 1; // TODO

    std::expected<ReturnT, std::string> ret;

    lua_getglobal(L, function.c_str());
    if (!lua_isfunction(L, -1)) {
        ret = std::unexpected(lua_tostring(L, -1));
        luaPop(1);
        return ret;
    }

    ((pushStack(args)), ...);

    if (LUA_OK != lua_pcall(L, nargs, nrets, 0)) {
        ret = std::unexpected(lua_tostring(L, -1));
        luaPop(1);
        return ret;
    } else {
        return popStack<ReturnT>();
    }
}

// TODO maybe do the returns and checks a little better...
template <typename ReturnT>
std::expected<ReturnT, std::string> LuaIntf::luaGetStack(const int stackIdx) {
    if constexpr (std::is_same_v<ReturnT, bool>) {
        if (lua_isboolean(L, stackIdx)) {
            return lua_toboolean(L, stackIdx);
        } else {
            return std::unexpected("Expected bool but got: " +
                                   std::string(lua_typename(L, lua_type(L, stackIdx))));
        }
    } else if constexpr (std::is_integral_v<ReturnT>) {
        if (lua_isinteger(L, stackIdx)) {
            return static_cast<ReturnT>(lua_tointeger(L, stackIdx));
        } else {
            return std::unexpected("Expected int type but got: " +
                                   std::string(lua_typename(L, lua_type(L, stackIdx))));
        }
    } else if constexpr (std::is_floating_point_v<ReturnT>) {
        if (lua_isnumber(L, stackIdx)) {
            return static_cast<ReturnT>(lua_tonumber(L, stackIdx));
        } else {
            return std::unexpected("Expected float type but got: " +
                                   std::string(lua_typename(L, lua_type(L, stackIdx))));
        }
    } else if constexpr (std::is_same_v<ReturnT, std::string>) {
        if (lua_isstring(L, stackIdx)) {
            return lua_tostring(L, stackIdx);
        } else {
            return std::unexpected("Expected string type but got: " +
                                   std::string(lua_typename(L, lua_type(L, stackIdx))));
        }
    } else {
        static_assert(dependent_false_v<ReturnT>, "Unsupported return type");
    }
}

template <typename ReturnT>
std::expected<ReturnT, std::string> LuaIntf::popStack() {
    auto ret = luaGetStack<ReturnT>(-1);
    luaPop(1);
    return ret;
}

template <typename... ReturnT, size_t... Is>
inline std::expected<std::tuple<ReturnT...>, std::string> LuaIntf::popMultiStackHelper(
    const std::index_sequence<Is...>) {

    constexpr size_t nargs = sizeof...(ReturnT);

    std::tuple<ReturnT...> returned{};

    try {
        (([&returned, this]() {
            constexpr int stackIdx = -1 - static_cast<int>(Is);

            auto ret = luaGetStack<std::tuple_element_t<Is, std::tuple<ReturnT...>>>(stackIdx);
            if (ret.has_value()) {
                std::get<Is>(returned) = std::move(ret.value());
            } else {
                throw std::string(
                    "Unexpected value type for stack index: " + std::to_string(stackIdx));
            }
        }()),
            ...);
    } catch (const std::string &err) {
        luaPop(nargs);
        return std::unexpected(std::move(err));
    }

    luaPop(nargs);
    return returned;
}

template <typename... ReturnT>
std::expected<std::tuple<ReturnT...>, std::string> LuaIntf::popMultiStack() {
    return popMultiStackHelper<ReturnT...>(std::make_index_sequence<sizeof...(ReturnT)>{});
}
} // namespace aoc::lua
