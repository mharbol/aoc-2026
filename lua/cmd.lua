--- Parse an integer
---@param str string the string representing the integer
---@param curr_num integer the current value of the integer
---@param min integer the minimum inclusive value for the integer
---@param max integer the maximum inclusive value for the integer
---@return integer ret the parsed value of the integer if successful
---@return string|nil err error if present, nil on success
local function parse_int(str, curr_num, min, max)
    local ret = -1
    local err = nil
    if -1 ~= curr_num then
        err = "cannot redefine value"
    else
        local num = math.tointeger(str)
        if nil == num then
            err = 'must be an integer: "' .. str .. '"'
        elseif min > num or num > max then
            err = "must be between " .. min .. " and " .. max .. ": " .. str
        else
            ret = num
        end
    end
    return ret, err
end

--- Check if there are enough args let to parse
---@param args string[] the arguments
---@param idx integer the current index
---@param needed integer the number of additionally needed arguments
---@return boolean result true if there are enough arguments, otherwise false
local function check_args(args, idx, needed)
    return #args >= idx + needed
end

--- Parse and set the day to solve. Set any errors if present
---@param opts table the options to return
---@param args string[] the command line arguments
---@param idx integer the current index parsing
local function parse_day(opts, args, idx)
    if not check_args(args, idx, 1) then
        opts.err = "Error: ran out of arguments while parsing day"
        return
    end
    local day, err = parse_int(args[idx + 1], opts.day, 1, 12)
    opts.day = day
    if err then
        opts.err = "Error while parsing day: " .. err
    end
end

--- Parse and set the part to solve. Set any errors if present
---@param opts table the options to return
---@param args string[] the command line arguments
---@param idx integer the current index parsing
local function parse_part(opts, args, idx)
    if not check_args(args, idx, 1) then
        opts.err = "Error: ran out of arguments while parsing part"
        return
    end
    local part, err = parse_int(args[idx + 1], opts.part, 1, 2)
    opts.part = part
    if err then
        opts.err = "Error while parsing part: " .. err
    end
end

--- Parse and set the input file. Set any errors if present
---@param opts table the options to return
---@param args string[] the command line arguments
---@param idx integer the current index parsing
local function parse_file(opts, args, idx)
    if not check_args(args, idx, 1) then
        opts.err = "Error: ran out of arguments while parsing file"
        return
    end
    if "" == opts.file then
        opts.file = args[idx + 1]
    else
        opts.err = "Error while parsing file: cannot redefine value"
    end
end

--- Parse the command line arguments
---@param args string[] the arguments
---@return integer day the day
---@return integer part the part
---@return string file the file
---@return string err any error
function parse_args(args)
    local opts = {
        day = -1,
        part = -1,
        file = "",
        err = "",
    }

    local idx = 1
    while idx <= #args and "" == opts.err do
        local curr_arg = args[idx]
        if "-d" == curr_arg then
            parse_day(opts, args, idx)
            idx = idx + 1
        elseif "-p" == curr_arg then
            parse_part(opts, args, idx)
            idx = idx + 1
        elseif "-f" == curr_arg then
            parse_file(opts, args, idx)
            idx = idx + 1
        else
            opts.err = "Unexpected argument at index " .. idx .. ": " .. curr_arg
        end
        idx = idx + 1
    end

    return opts.day, opts.part, opts.file, opts.err
end
