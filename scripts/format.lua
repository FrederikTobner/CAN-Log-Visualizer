#!/usr/bin/env lua
--- clang-format runner for the CAN Log Visualizer.
--
-- Checks or fixes the formatting of all sources under app/ and core/
-- against the project's .clang-format style file.
--
-- Usage: lua scripts/format.lua [OPTIONS]
-- Run with --help for full usage information.
--
-- @copyright Copyright (C) 2026 Frederik Tobner
-- @license   GNU General Public License v3.0 or later

package.path = (arg[0]:match("^(.*)/[^/]+$") or ".") .. "/lib/?.lua;" .. package.path

local C         = require("ansi").C
local shell     = require("shell")
local cli       = require("cli")
local project   = require("project")
local toolchain = require("toolchain")

local ROOT = project.root()

-- The CI pipeline pins clang-format-23. Other versions can format differently,
-- giving a false pass locally while CI still fails.
local TOOL = toolchain.pinned_tool("clang-format", "23", { "22", "21" })

local USAGE = [[
Usage: lua scripts/format.lua [OPTIONS]

Checks or fixes the formatting of all sources against .clang-format.

Options:
  -f, --fix             Reformat files in-place (default: check only)
      --filter PATTERN  Only process files whose path matches PATTERN
  -v, --verbose         Print every file, not just failures / fixes
  -h, --help            Print this message and exit

Exit codes:
  0   All files are correctly formatted (check) or all done (fix)
  1   One or more files need formatting (check mode only)
]]

local opts = cli.parse(USAGE, {
    { "-f", "--fix", "fix" },
    { nil, "--filter", "filter", true },
    { "-v", "--verbose", "verbose" },
}, {})

local raw = shell.capture(string.format(
    "find %s/app %s/core %s/gui -type f \\( -name '*.[ch]' -o -name '*.[ch]pp' \\) | sort",
    shell.quote(ROOT), shell.quote(ROOT), shell.quote(ROOT)))
local files = {}
for path in raw:gmatch("[^\n]+") do files[#files + 1] = path end
files = project.select(files, opts.filter)

io.write(string.format("%s── CAN Log Visualizer format %s ──%s  (%s)\n\n",
    C.bold, opts.fix and "fix" or "check", C.reset, TOOL))

local n, dirty = #files, 0
for i, path in ipairs(files) do
    local quoted = shell.quote(path)
    local clean = shell.exec(string.format("%s --dry-run --Werror %s 2>/dev/null", TOOL, quoted))
    if not clean then
        dirty = dirty + 1
        if opts.fix then shell.exec(string.format("%s -i %s", TOOL, quoted)) end
    end
    if opts.verbose or not clean then
        local label = clean and (C.green .. "✓")
            or opts.fix and (C.yellow .. "fixed")
            or (C.red .. "✗  needs formatting")
        io.write(string.format("[%d/%d] %-58s %s%s\n", i, n, project.short_path(path), label, C.reset))
    end
end

local sep = string.rep("─", 52)
io.write(string.format("\n%s\n ", sep))
if dirty == 0 then
    io.write(string.format("%sAll %d files %s%s\n", C.green, n,
        opts.fix and "already correctly formatted" or "correctly formatted", C.reset))
elseif opts.fix then
    io.write(string.format("%sFixed %d of %d files%s  (%d already correct)\n",
        C.yellow, dirty, n, C.reset, n - dirty))
else
    io.write(string.format("%s%d of %d files need formatting%s\n %sRun with --fix to apply changes%s\n",
        C.red, dirty, n, C.reset, C.dim, C.reset))
end
io.write(sep .. "\n")

os.exit((dirty > 0 and not opts.fix) and 1 or 0)
