
local hey = require('hey')

local a = 7 + 11
a = a + 100

function AddStuff(x, y)
    print("[LUA] AddStuff(" .. x .. ", " .. y .. ") called.\n")
    return x + y
end

hey.test()

function MySort()
    local t = { 5, 4, 3, 2, 1 }
    print(t[1] .. ", " .. t[2] .. ", " .. t[3] .. ", " .. t[4] .. ", " .. t[5])
    table.sort(t)
    print(t[1] .. ", " .. t[2] .. ", " .. t[3] .. ", " .. t[4] .. ", " .. t[5])
    print(package.path)
end
