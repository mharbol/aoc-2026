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

    const auto solution = intf.call<std::string>("run_problem", day, part, lines);

    if (!solution.has_value()) {
        std::cerr << "Error while running solution: " << solution.error() << std::endl;
        return 1;
    }

    std::cout << solution.value() << std::endl;

    return 0;
}
