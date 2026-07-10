
#ifndef __AOC_LUA_INTF_H__
#define __AOC_LUA_INTF_H__

#include <cstddef>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace aoc::lua {
class LuaIntf {
public:
    inline LuaIntf() : L(luaL_newstate()) {};

    inline virtual ~LuaIntf() {
        lua_close(L);
    };

    inline bool doString(const std::string &cmd) {
        return LUA_OK == luaL_dostring(L, cmd.c_str());
    }

    inline std::string getHeadString() {
        size_t len;
        const char *str = lua_tolstring(L, -1, &len);
        return std::string(str, len);
    }

    inline void openBaseLibs() {
        luaopen_base(L);
        luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
        lua_pop(L, 1);
        luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
        lua_pop(L, 1);
    }

    lua_State *L;
};
} // namespace aoc::lua
#endif // __AOC_LUA_INTF_H__
