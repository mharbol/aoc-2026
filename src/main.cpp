
#include "LuaIntf.h"
#include <iostream>
#include <string>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
}

int main() {

    aoc::lua::LuaIntf intf{};

    const std::string cmd = "local b = 7\na = b + 11";

    const auto ok = intf.doString(cmd);

    if (ok) {
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

    intf.openBaseLibs();

    if (LUA_OK == luaL_dofile(intf.L, "lua/myscript.lua")) {
        // nothing
    } else {
        std::cerr << lua_tostring(intf.L, -1) << std::endl;
        return 1;
    }

    lua_getglobal(intf.L, "a");
    std::cout << "from file: " << intf.getHeadString() << std::endl;

    // calling a lua function...
    const std::string func = "AddStuff";
    constexpr float a = 3.5f;
    constexpr float b = 7.1f;
    lua_getglobal(intf.L, func.c_str());
    if (lua_isfunction(intf.L, -1)) {
        lua_pushnumber(intf.L, a);
        lua_pushnumber(intf.L, b);
        lua_pcall(intf.L, 2, 1, 0);
        const auto ret = static_cast<float>(lua_tonumber(intf.L, -1));
        std::cout << "Called AddStuff with a = " << a << " and b = " << b
                  << " and got return: " << ret << "." << std::endl;
    }
    const std::string other_func = "MySort";
    lua_getglobal(intf.L, other_func.c_str());
    lua_pcall(intf.L, 0, 0, 0);
    return 0;
}
