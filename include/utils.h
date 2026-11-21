#ifndef _AOC_FILE_READ_H_
#define _AOC_FILE_READ_H_

#include <string>
#include <vector>

namespace aoc {

std::vector<std::string> readInput(const std::string &file_path);

bool isValidFile(const std::string &file_path);

} // namespace aoc

#endif // _AOC_FILE_READ_H_
