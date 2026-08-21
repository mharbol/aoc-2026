
#ifndef __AOC_LUA_INTF_H__
#define __AOC_LUA_INTF_H__

#include <cstddef>
#include <expected>
#include <string>
#include <utility>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace aoc::lua {

class LuaIntf {
    lua_State *L;

public:
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

    void openBaseLibs();
    bool luaDoString(const std::string &cmd);
    std::string getHeadString();
    void luaPop(size_t pop);
    // void luaPush(size_t pop); // TODO

    void appendPath(const std::string &path);

    // TODO remove this eventually
    inline lua_State *getL() {
        return L;
    }

    template <typename T>
    std::expected<T, std::string> getGlobal(const std::string &var);

    template <typename ReturnT, typename... Args>
    std::expected<ReturnT, std::string> callLuaFunc(const std::string &function, Args... args);
};
} // namespace aoc::lua

#include "LuaIntf.ipp"

#endif // __AOC_LUA_INTF_H__
