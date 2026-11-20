#include <iostream>
#include <string>

#include "LuaIntf.h"

int main() {

    lua::LuaIntf intf{};
    intf.openBaseLibs();
    intf.appendPath(";./lua/?.lua;./lua/?/init.lua");

    const std::string cmd = "local b = 7\na = b + 11";
    const auto ok = intf.luaDoString(cmd);

    if (!ok) {
        const std::string err = lua_tostring(intf.getL(), -1);
        std::cerr << err << std::endl;
    }

    intf.luaDoFile("lua/myscript.lua");
    if (!intf.luaDoFile("lua/solve.lua")) {
        std::cerr << "ahhhhh!!!!" << std::endl;
    }

    std::cout << "from file: " << *intf.getGlobal<std::string>("a") << std::endl;

    const std::string add_stuff = "AddStuff";
    constexpr float a = 3.5f;
    constexpr float b = 7.1f;

    const auto ret = intf.call<float>("AddStuff", a, b).value();
    std::cout << "Called AddStuff with a = " << a << " and b = " << b
              << " and got return: " << ret << "." << std::endl;

    const std::string other_func = "MySort";
    static_cast<void>(*intf.call<void>("MySort"));

    constexpr auto path_func = "PrintPath";
    static_cast<void>(*intf.call<void>(path_func));

    return 0;
}
