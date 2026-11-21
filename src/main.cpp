#include <cstdint>
#include <expected>
#include <iostream>
#include <ostream>
#include <string>

#include "LuaIntf.h"
#include "utils.h"

int main(const int argc, const char **argv) {

    constexpr auto exec_path = ";./lua/?.lua;./lua/?/init.lua";
    constexpr auto cmd_file = "lua/cmd.lua";
    constexpr auto parse_args_func = "parse_args";

    lua::LuaIntf intf{};
    intf.openBaseLibs();
    intf.appendPath(exec_path);

    if (!intf.luaDoFile(cmd_file)) {
        std::cerr << "Could not execute command file \"" << cmd_file
                  << "\". Does it exist or are the errors in it?" << std::endl;
        return 1;
    }

    const std::vector<std::string> cmd_args(argv + 1, argv + argc);

    auto parse_result =
        intf.call<int32_t, int32_t, std::string, std::string>(parse_args_func, cmd_args);
    if (!parse_result.has_value()) {
        std::cerr << "Error while executing " << parse_args_func
                  << "(): " << parse_result.error() << std::endl;
        return 1;
    }

    const auto [day, part, file, err] = std::move(*parse_result);

    if (!err.empty()) {
        std::cerr << err << std::endl;
        return 1;
    }

    if (!aoc::isValidFile(file)) {
        std::cerr << "Could not read input file: \"" << file << "\"" << std::endl;
        return 1;
    }

    const std::vector<std::string> lines = aoc::readInput(file);

    // Kinda messing around
    std::cout << day << " " << part << " " << file << " " << err << std::endl;

    if (!intf.luaDoString("print(require('solve').run_problem(1,1,{'hat', 'cat'}))")) {
        std::cerr << "ahhhhh again!!!!" << std::endl;
    }

    // Messing around...
    intf.luaDoFile("lua/myscript.lua");

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
