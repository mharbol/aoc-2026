
#include <iostream>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

int main() {
    const std::string cmd = "a = 7 + 11";
    lua_State *L = luaL_newstate();

    const int ret = luaL_dostring(L, cmd.c_str());
    if (LUA_OK == ret) {
        lua_getglobal(L, "a");
        if (lua_isnumber(L, -1)) {
            float a_cpp = static_cast<float>(lua_tonumber(L, -1));
            std::cout << "Command: \"" << cmd << "\"" << std::endl;
            std::cout << "a_cpp = " << a_cpp << std::endl;
        }
    } else {
        std::string err = lua_tostring(L, -1);
        std::cerr << err << std::endl;
    }

    lua_close(L);
    return 0;
}
