local M = {}

--- Solve the day and part problem for the given lines.
--- This requires that the day and part have been validated.
---@param day integer the day's number
---@param part integer the part to solve
---@param lines string[] the lines in the problem
---@return integer|string solution the solution
function M.runProblem(day, part, lines)
    local cases = {
        { -- index 1
            require("solution.day01").part1,
            require("solution.day01").part2,
        },
        { -- index 2
            require("solution.day02").part1,
            require("solution.day02").part2,
        },
        { -- index 3
            require("solution.day03").part1,
            require("solution.day03").part2,
        },
        { -- index 4
            require("solution.day04").part1,
            require("solution.day04").part2,
        },
        { -- index 5
            require("solution.day05").part1,
            require("solution.day05").part2,
        },
        { -- index 6
            require("solution.day06").part1,
            require("solution.day06").part2,
        },
        { -- index 7
            require("solution.day07").part1,
            require("solution.day07").part2,
        },
        { -- index 8
            require("solution.day08").part1,
            require("solution.day08").part2,
        },
        { -- index 9
            require("solution.day09").part1,
            require("solution.day09").part2,
        },
        { -- index 10
            require("solution.day10").part1,
            require("solution.day10").part2,
        },
        { -- index 11
            require("solution.day11").part1,
            require("solution.day11").part2,
        },
        { -- index 12
            require("solution.day12").part1,
            require("solution.day12").part2,
        },
    }
    return cases[day][part](lines)
end

return M
