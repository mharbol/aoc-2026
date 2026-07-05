
#ifndef __AOC_LUA_INTF_H__
#define __AOC_LUA_INTF_H__

#include <string>
extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace aoc {
class LuaIntf {

public:
    inline LuaIntf() : L(luaL_newstate()) {};

    inline virtual ~LuaIntf() {
        lua_close(L);
    };

    inline bool doString(const std::string &cmd) {
        return LUA_OK == luaL_dostring(L, cmd.c_str());
    }
    lua_State *L;
};
} // namespace aoc
#endif // __AOC_LUA_INTF_H__
