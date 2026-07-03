
#include "LuaIntf.h"
#include <iostream>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

int main() {
    aoc::LuaIntf intf{};
    const std::string cmd = "a = 7 + 11";

    const int ret = luaL_dostring(intf.L, cmd.c_str());

    if (LUA_OK == ret) {
        lua_getglobal(intf.L, "a");
        if (lua_isnumber(intf.L, -1)) {

            std::cout << "Command: \"" << cmd << "\"" << std::endl;

            const auto a_cpp = static_cast<float>(lua_tonumber(intf.L, -1));
            std::cout << "a_cpp        = " << a_cpp << std::endl;

            size_t len;
            const char *a_cpp_str = lua_tolstring(intf.L, -1, &len);
            const std::string a_cpp_string(a_cpp_str, len);
            std::cout << "a_cpp_string = " << a_cpp_string << std::endl;
        }
    } else {
        const std::string err = lua_tostring(intf.L, -1);
        std::cerr << err << std::endl;
    }

    lua_close(intf.L);
    return 0;
}
