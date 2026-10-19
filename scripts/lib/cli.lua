--- CLI helpers shared by the project's Lua scripts.
--
-- @copyright Copyright (C) 2026 Frederik Tobner
-- @license   GNU General Public License v3.0 or later

local ansi = require("ansi")

local M = {}

--- Print an error message to stderr and exit with status 1.
---
--- @param fmt string printf-style format string
--- @param ... any printf-style arguments
function M.die(fmt, ...)
    io.stderr:write(string.format("%serror:%s " .. fmt .. "\n", ansi.C.red, ansi.C.reset, ...))
    os.exit(1)
end

--- Parse the global `arg` list into `opts`; -h/--help prints `usage` and exits.
---
--- @param usage string help text
--- @param defs  table list of { short, long, key, takes_value } option definitions
--- @param opts  table defaults, updated in place
--- @return table opts
function M.parse(usage, defs, opts)
    local i = 1
    while i <= #arg do
        local a, def = arg[i], nil
        if a == "-h" or a == "--help" then
            io.write(usage)
            os.exit(0)
        end
        for _, d in ipairs(defs) do
            if a == d[1] or a == d[2] then def = d end
        end
        if not def then M.die("unknown option: %s  (try --help)", a) end
        if def[4] then
            i = i + 1
            if not arg[i] then M.die("%s requires an argument", def[2]) end
            opts[def[3]] = arg[i]
        else
            opts[def[3]] = true
        end
        i = i + 1
    end
    return opts
end

return M
