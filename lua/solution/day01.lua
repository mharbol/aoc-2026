local M = {}

local function parse_iter(lines)
    local turns = {}
    for _, line in ipairs(lines) do
        if 'L' == line:sub(1, 1) then
            table.insert(turns, -math.tointeger(line:sub(2, -1)))
        else
            table.insert(turns, math.tointeger(line:sub(2, -1)))
        end
    end
    return turns
end

--- Solution to Day 1 part 1
---@param lines string[] the lines of the problem
---@return integer|string solution the solution
function M.part1(lines)
    local state = 50
    local count = 0
    for _, add in ipairs(parse_iter(lines)) do
        state = (state + add) % 100
        if 0 == state then
            count = count + 1
        end
    end
    return count
end

--- Solution to Day 1 part 2
---@param lines string[] the lines of the problem
---@return integer|string solution the solution
function M.part2(lines)
    local state = 50
    local count = 0
    for _, add in ipairs(parse_iter(lines)) do
        local old_state = state
        count = count + math.abs(add) // 100
        state = state + math.fmod(add, 100)
        if 0 == state or 0 ~= old_state and (0 > state or 100 <= state) then
            count = count + 1
        end
        state = state % 100
    end
    return count
end

return M
