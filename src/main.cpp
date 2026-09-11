
#include "LuaIntf.h"
#include <iostream>
#include <string>

#include <lua.hpp>

int main() {

    lua::LuaIntf intf{};

    const std::string cmd = "local b = 7\na = b + 11";

    const auto ok = intf.luaDoString(cmd);

    if (ok) {
        lua_getglobal(intf.getL(), "a");
        if (lua_isnumber(intf.getL(), -1)) {

            std::cout << "Command: \"" << cmd << "\"" << std::endl;

            const auto a_cpp_f = static_cast<float>(lua_tonumber(intf.getL(), -1));
            std::cout << "a_cpp_f = " << a_cpp_f << std::endl;

            const auto a_cpp_i = static_cast<int32_t>(lua_tonumber(intf.getL(), -1));
            std::cout << "a_cpp_i = " << a_cpp_i << std::endl;

            const auto a_cpp_e = intf.getGlobal<float>("a");
            std::cout << "a_cpp_e = " << a_cpp_e.value() << std::endl;

            const auto a_cpp_E = intf.getGlobal<int32_t>("a");
            std::cout << "a_cpp_E = " << a_cpp_E.value() << std::endl;

            static const char *A = "a";
            const auto a_cpp_S = intf.getGlobal<std::string>(A);
            std::cout << "a_cpp_S = " << a_cpp_S.value() << std::endl;
        }
    } else {
        const std::string err = lua_tostring(intf.getL(), -1);
        std::cerr << err << std::endl;
    }
    lua_pop(intf.getL(), 1);

    intf.openBaseLibs();
    intf.appendPath(";./lua/?.lua;./lua/?/init.lua");

    if (LUA_OK != luaL_dofile(intf.getL(), "lua/myscript.lua")) {
        std::cerr << lua_tostring(intf.getL(), -1) << std::endl;
        // TODO probably pop but also we're ending sooo......
        return 1;
    }

    lua_getglobal(intf.getL(), "a");
    std::cout << "from file: " << intf.getHeadAsString() << std::endl;

    // calling some lua functions...
    const std::string add_stuff = "AddStuff";
    constexpr float a = 3.5f;
    constexpr float b = 7.1f;
    lua_getglobal(intf.getL(), add_stuff.c_str());
    if (lua_isfunction(intf.getL(), -1)) {
        lua_pushnumber(intf.getL(), a);
        lua_pushnumber(intf.getL(), b);
        lua_pcall(intf.getL(), 2, 1, 0);

        const auto ret = static_cast<float>(lua_tonumber(intf.getL(), -1));
        std::cout << "Called AddStuff with a = " << a << " and b = " << b
                  << " and got return: " << ret << "." << std::endl;
    }

    const std::string other_func = "MySort";
    lua_getglobal(intf.getL(), other_func.c_str());
    lua_pcall(intf.getL(), 0, 0, 0);

    constexpr auto path_func = "PrintPath";
    lua_getglobal(intf.getL(), path_func);
    if (lua_isfunction(intf.getL(), -1)) {
        lua_pcall(intf.getL(), 0, 0, 0);
    }

    return 0;
}
