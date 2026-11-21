#include "utils.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace aoc {
std::vector<std::string> readInput(const std::string &file_path) {
    std::ifstream file{file_path};
    std::vector<std::string> ret{};
    std::string line{};
    while (std::getline(file, line)) {
        ret.push_back(std::move(line));
    }
    return ret;
}

bool isValidFile(const std::string &file_path) {
    std::error_code err{};
    std::filesystem::path path{file_path};
    if (!std::filesystem::exists(path, err) || !std::filesystem::is_regular_file(path, err)) {
        return false;
    }
    std::ifstream file{file_path};
    return file.is_open();
}
} // namespace aoc
