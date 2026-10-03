
#ifndef _AOC_LUA_INTF_H_
#define _AOC_LUA_INTF_H_

#include <cstddef>
#include <expected>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <lua.hpp>

namespace lua {

class LuaIntf {
    lua_State *L;

public:
    // start rule of 5
    LuaIntf() : L(luaL_newstate()) {};
    ~LuaIntf() {
        lua_close(L);
    };
    LuaIntf(const LuaIntf &) = delete;
    LuaIntf &operator=(const LuaIntf &) = delete;
    LuaIntf &operator=(LuaIntf &&other) noexcept {
        std::swap(L, other.L);
        return *this;
    };
    LuaIntf(LuaIntf &&other) noexcept : L(std::exchange(other.L, nullptr)) {};
    // end rule of 5

    // TODO redo this one
    template <typename T>
    std::expected<T, std::string> getGlobal(const std::string &var);

    // TODO update for failed exec
    bool luaDoString(const std::string &cmd);

    // TODO (maybe) improve the path stuff... maybe
    void appendPath(const std::string &path);

    // TODO really good doc
    void openBaseLibs();

    size_t getStackSize();

    void luaPop(size_t pop);

    template <typename T>
        requires std::is_null_pointer_v<T> || std::is_integral_v<T> ||
                 std::is_floating_point_v<T>
    void pushStack(const T var);

    template <typename T>
        requires std::same_as<std::decay_t<T>, std::string>
    void pushStack(const T &var);

    template <typename T>
        requires std::same_as<std::decay_t<T>, char *> ||
                 std::same_as<std::decay_t<T>, const char *>
    void pushStack(const T var);

    template <typename ReturnT>
    std::expected<ReturnT, std::string> popStack();

    template <typename ReturnT, typename... Args>
    std::expected<ReturnT, std::string> call(const std::string &function, Args... args);

    template <typename ReturnT>
    std::expected<ReturnT, std::string> luaGetStack(const int stack_idx);

    // TODO remove this eventually
    inline lua_State *getL() {
        return L;
    }

    template <typename... ReturnT>
    std::expected<std::tuple<ReturnT...>, std::string> popMultiStack();

    template <typename T>
    bool luaCheckType(const int stackIdx);

private:
    template <typename... ReturnT, size_t... Is>
    std::expected<std::tuple<ReturnT...>, std::string> popMultiStack(
        const std::index_sequence<Is...>);

    template <typename ReturnT, auto LuaGetFunc>
    std::expected<ReturnT, std::string> luaGetStackTemplate(const int stack_idx,
        const std::string &type_name);

    template <typename ReturnT>
    std::expected<std::optional<ReturnT>, std::string> luaGetStackOptional(
        const int stack_idx);
};
} // namespace lua

#include "LuaIntf.ipp"

#endif // _AOC_LUA_INTF_H_
