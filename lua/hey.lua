local M = {}

--[[
    This is a multi-line comment
--]]

local function heyhey()
    print("[LUA] This was called from 'heyhey()'")
end

function M.test()
    print("[LUA] This was called from 'hey.lua'")
    heyhey()
    M.defined_below()
end

function M.defined_below()
    print("[LUA] This was defined below the calling func")
end

---@return number|string
function M.part1()
    return 4
end

return M
