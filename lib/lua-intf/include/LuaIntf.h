
#ifndef __AOC_LUA_INTF_H__
#define __AOC_LUA_INTF_H__

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

namespace aoc {
class LuaIntf {


public:
    LuaIntf() : L(luaL_newstate()) {};
    inline virtual ~LuaIntf() {
        lua_close(L);
    };
    lua_State *L;
};
} // namespace aoc
#endif // __AOC_LUA_INTF_H__
