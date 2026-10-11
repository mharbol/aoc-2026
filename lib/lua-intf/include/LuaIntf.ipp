
#include "LuaIntf.h"

#include <cstddef>
#include <expected>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include <lua.hpp>

namespace lua {

template <typename...>
constexpr bool dependent_false_v = false; // TODO, maybe this becomes better

template <typename T>
struct is_optional : std::false_type {};

template <typename U>
struct is_optional<std::optional<U>> : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename T, typename Variant>
struct variant_contains : std::false_type {};

template <typename T, typename... V>
struct variant_contains<T, std::variant<V...>>
    : std::bool_constant<(std::is_same_v<T, V> || ...)> {};

template <typename T, typename V>
inline constexpr bool variant_contains_v = variant_contains<T, V>::value;

inline bool LuaIntf::luaDoString(const std::string &cmd) {
    return LUA_OK == luaL_dostring(L, cmd.c_str());
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

// TODO, specialize for multi push
template <typename T>
    requires std::is_null_pointer_v<T> || std::is_integral_v<T> || std::is_floating_point_v<T>
inline void LuaIntf::pushStack(const T var) {
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
inline void LuaIntf::pushStack(const T &var) {
    lua_pushlstring(L, var.c_str(), var.size());
}

template <typename T>
    requires std::same_as<std::decay_t<T>, char *> ||
             std::same_as<std::decay_t<T>, const char *>
inline void LuaIntf::pushStack(const T var) {
    lua_pushstring(L, var);
}

// right now just single return and void
template <typename ReturnT, typename... Args>
std::expected<ReturnT, std::string> LuaIntf::call(const std::string &function, Args... args) {

    constexpr size_t nargs = sizeof...(Args);
    constexpr size_t nrets = std::is_void_v<ReturnT> ? 0 : 1;

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

template <typename T>
inline bool LuaIntf::luaCheckType(const int stack_idx) {
    if constexpr (std::is_null_pointer_v<T>) {
        return lua_isnil(L, stack_idx);
    } else if constexpr (std::is_same_v<T, bool>) {
        return lua_isboolean(L, stack_idx);
    } else if constexpr (std::is_integral_v<T>) {
        return lua_isinteger(L, stack_idx);
    } else if constexpr (std::is_floating_point_v<T>) {
        return lua_isnumber(L, stack_idx);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return lua_isstring(L, stack_idx); // TODO: make this is strictly string if you can
    } else {
        static_assert(dependent_false_v<T>, "Unsupported return type");
    }
}

// TODO maybe error names could be deduced...
// ...maybe this whole thing becomes private or protected...
template <typename ReturnT>
std::expected<ReturnT, std::string> LuaIntf::luaGetStack(const int stack_idx) {
    if constexpr (is_optional_v<ReturnT>) {
        return luaGetStackOptional<typename ReturnT::value_type>(stack_idx);
    } else if constexpr (std::is_same_v<ReturnT, bool>) {
        return luaGetStackExpected<ReturnT>(stack_idx, "bool");
    } else if constexpr (std::is_integral_v<ReturnT>) {
        return luaGetStackExpected<ReturnT>(stack_idx, "integer");
    } else if constexpr (std::is_floating_point_v<ReturnT>) {
        return luaGetStackExpected<ReturnT>(stack_idx, "floating point");
    } else if constexpr (std::is_same_v<ReturnT, std::string>) {
        return luaGetStackExpected<ReturnT>(stack_idx, "string");
    } else {
        static_assert(dependent_false_v<ReturnT>, "Unsupported return type");
    }
}

template <typename ReturnT>
inline ReturnT LuaIntf::luaGetStackUnchecked(const int stack_idx) {
    if constexpr (std::is_same_v<ReturnT, bool>) {
        return lua_toboolean(L, stack_idx);
    } else if constexpr (std::is_integral_v<ReturnT>) {
        return static_cast<ReturnT>(lua_tointeger(L, stack_idx));
    } else if constexpr (std::is_floating_point_v<ReturnT>) {
        return static_cast<ReturnT>(lua_tonumber(L, stack_idx));
    } else if constexpr (std::is_same_v<ReturnT, std::string>) {
        return lua_tostring(L, stack_idx);
    } else {
        static_assert(dependent_false_v<ReturnT>, "Unsupported return type");
    }
}

template <typename ReturnT>
inline std::expected<std::optional<ReturnT>, std::string> LuaIntf::luaGetStackOptional(
    const int stack_idx) {

    if (luaCheckType<std::nullptr_t>(stack_idx)) {
        return {};
    } else {
        return luaGetStack<ReturnT>(stack_idx);
    }
}

template <typename ReturnT>
inline std::expected<ReturnT, std::string> LuaIntf::luaGetStackExpected(const int stack_idx,
    const std::string &type_name) {

    if (luaCheckType<ReturnT>(stack_idx)) {
        return luaGetStackUnchecked<ReturnT>(stack_idx);
    } else {
        return std::unexpected("Expected " + type_name + " type but got: " +
                               std::string(lua_typename(L, lua_type(L, stack_idx))));
    }
}

// TODO - specialize for tuple pop
template <typename ReturnT>
std::expected<ReturnT, std::string> LuaIntf::popStack() {
    auto ret = luaGetStack<ReturnT>(-1);
    luaPop(1);
    return ret;
}

template <>
std::expected<void, std::string> LuaIntf::popStack() {
    return std::expected<void, std::string>{};
}

template <typename... ReturnT, size_t... Is>
inline std::expected<std::tuple<ReturnT...>, std::string> LuaIntf::popMultiStack(
    const std::index_sequence<Is...>) {

    constexpr size_t nargs = sizeof...(ReturnT);

    std::tuple<ReturnT...> returned{};

    try {
        (([&returned, this]() {
            constexpr int idx = -1 - static_cast<int>(Is);

            auto ret = luaGetStack<std::tuple_element_t<Is, std::tuple<ReturnT...>>>(idx);
            if (ret.has_value()) {
                std::get<Is>(returned) = std::move(ret.value());
            } else {
                throw std::string(
                    "Unexpected value type for stack index: " + std::to_string(idx));
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

template <typename... ReturnT, size_t... Is>
inline std::expected<std::variant<ReturnT...>, std::string> LuaIntf::luaGetStackVariantHelper(
    const int i, const std::index_sequence<Is...>) {

    using V = std::variant<ReturnT...>;
    V ret{};

    // short-circuit on the variant and return first found type match
    if (((luaCheckType<typename std::variant_alternative_t<Is, V>>(i)
                 ? (ret = luaGetStackUnchecked<typename std::variant_alternative_t<Is, V>>(i),
                       true)
                 : false) ||
            ...)) {
        return ret;
    } else {
        return std::unexpected("Unexpected return type");
    }
}

template <typename... ReturnT>
inline std::expected<std::tuple<ReturnT...>, std::string> LuaIntf::popMultiStack() {
    return popMultiStack<ReturnT...>(std::make_index_sequence<sizeof...(ReturnT)>{});
}

template <typename... ReturnT, size_t... Is>
inline std::expected<std::variant<ReturnT...>, std::string> LuaIntf::luaGetStackVariant(
    const int stack_idx) {

    return luaGetStackVariantHelper<ReturnT...>(stack_idx,
        std::make_index_sequence<sizeof...(ReturnT)>{});
}
} // namespace lua
