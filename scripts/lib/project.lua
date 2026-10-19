--- Project-root resolution and file selection shared by the project's Lua scripts.
--
-- @copyright Copyright (C) 2026 Frederik Tobner
-- @license   GNU General Public License v3.0 or later

local shell = require("shell")
local cli   = require("cli")

local M = {}

local cached_root

--- Resolve the project root from the location of the running entry script.
-- The entry script lives at <root>/scripts/<name>.lua, so the root is the
-- parent of the directory that contains the script.
---
--- @return string absolute path to the project root
function M.root()
    if cached_root then return cached_root end

    local abs = shell.capture(string.format("realpath %q 2>/dev/null", arg[0]))
    if not abs then
        cli.die("cannot resolve script path from '%s'", arg[0])
    end
    abs = abs:match("^%s*(.-)%s*$")
    local root = abs:match("^(.*)/[^/]+/[^/]+$")  -- strip /scripts/<name>.lua
    if not root or root == "" then
        cli.die("cannot derive project root from script path '%s'", abs)
    end

    cached_root = root
    return root
end

--- Strip the project root prefix from a path for readable output.
---
--- @param path string absolute path
--- @return string path relative to the project root
function M.short_path(path)
    local prefix = M.root() .. "/"
    return path:sub(1, #prefix) == prefix and path:sub(#prefix + 1) or path
end

--- Restrict a file list to paths matching a Lua pattern; dies if nothing is left.
---
--- @param files  table list of paths
--- @param filter string|nil Lua pattern, or nil to keep everything
--- @return table filtered list
function M.select(files, filter)
    local out = {}
    for _, f in ipairs(files) do
        if not filter or f:match(filter) then out[#out + 1] = f end
    end
    if #out == 0 then
        cli.die(filter and "no files matched filter '%s'" or "no source files found", filter)
    end
    return out
end

return M
