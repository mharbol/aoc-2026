
#ifndef _AOC_LUA_INTF_H_
#define _AOC_LUA_INTF_H_

#include <cstddef>
#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <lua.hpp>

/**
 * @brief The Lua Interface
 */
namespace lua {

/**
 * @brief traits and concepts for `lua` namespace
 */
namespace impl {
template <typename...>
inline constexpr bool dependent_false_v = false; // TODO, maybe this becomes better

template <typename T>
struct is_optional : std::false_type {};

template <typename U>
struct is_optional<std::optional<U>> : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> : std::true_type {};

template <typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T, typename Variant>
struct variant_contains : std::false_type {};

template <typename T, typename... V>
struct variant_contains<T, std::variant<V...>>
    : std::bool_constant<(std::is_same_v<T, V> || ...)> {};

template <typename T, typename V>
inline constexpr bool variant_contains_v = variant_contains<T, V>::value;
} // namespace impl

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

    // TODO redo this one to actually be a type T
    template <typename T>
    std::expected<T, std::string> getGlobal(const std::string &var);

    // TODO update for failed exec
    bool luaDoString(const std::string &cmd);

    // TODO update for failed exec
    bool luaDoFile(const std::string &file);

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

    template <std::ranges::input_range R>
        requires(!std::same_as<std::decay_t<R>, std::string>) &&
                (!std::same_as<std::decay_t<R>, char *>) &&
                (!std::same_as<std::decay_t<R>, const char *>)
    void pushStack(const R &var);

    template <typename ReturnT>
    std::expected<ReturnT, std::string> popStack();

    template <typename... ReturnT>
        requires(sizeof...(ReturnT) > 1)
    std::expected<std::tuple<ReturnT...>, std::string> popStack();

    template <typename ReturnT, typename... Args>
    std::expected<ReturnT, std::string> call(const std::string &function, Args... args);

    template <typename... ReturnT, typename... Args>
        requires(sizeof...(ReturnT) > 1)
    std::expected<std::tuple<ReturnT...>, std::string> call(const std::string &function,
        Args... args);

    template <typename ReturnT>
    std::expected<ReturnT, std::string> luaGetStack(const int stack_idx);

    // TODO remove this eventually
    inline lua_State *getL() {
        return L;
    }

    template <typename... ReturnT>
    std::expected<std::tuple<ReturnT...>, std::string> popMultiStack();

    template <typename ReturnT>
    std::vector<ReturnT> luaGetStackVecUnchecked(const int stack_idx);

    template <typename T>
    bool luaCheckType(const int stack_idx);

    template <typename... ReturnT, size_t... Is>
    std::expected<std::variant<ReturnT...>, std::string> luaGetStackVariant(
        const int stack_idx);

    template <typename... ReturnT, size_t... Is>
    std::expected<std::variant<ReturnT...>, std::string> luaGetStackVariantHelper(
        const int stack_idx, const std::index_sequence<Is...>);

    // TODO: This will eventually become private.
    // TODO: This is the terminal point for all "get stack" operations, along with check stack
    template <typename ReturnT>
    ReturnT luaGetStackUnchecked(const int stack_idx);

private:
    template <typename... ReturnT, size_t... Is>
    std::expected<std::tuple<ReturnT...>, std::string> popMultiStackHelper(
        const std::index_sequence<Is...>);

    template <typename ReturnT>
    std::expected<ReturnT, std::string> luaGetStackExpected(const int stack_idx,
        const std::string &type_name);

    template <typename ReturnT>
    std::expected<std::optional<ReturnT>, std::string> luaGetStackOptional(
        const int stack_idx);
};
} // namespace lua

#include "LuaIntf.ipp"

#endif // _AOC_LUA_INTF_H_
