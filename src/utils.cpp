#include "utils.h"

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
} // namespace aoc
