#!/usr/bin/env lua
--- clang-tidy runner for the CAN Log Visualizer.
--
-- Runs clang-tidy on all app/ core/ and gui/ translation units listed in a build
-- directory's compile_commands.json, mirroring the CI clang-tidy job.
--
-- Usage: lua scripts/tidy.lua [OPTIONS]
-- Run with --help for full usage information.
--
-- @copyright Copyright (C) 2026 Frederik Tobner
-- @license   GNU General Public License v3.0 or later

package.path = (arg[0]:match("^(.*)/[^/]+$") or ".") .. "/lib/?.lua;" .. package.path

local C         = require("ansi").C
local shell     = require("shell")
local cli       = require("cli")
local project   = require("project")
local time      = require("time")
local progress  = require("progress")
local toolchain = require("toolchain")

local ROOT = project.root()

-- The CI pipeline pins clang-tidy-23. Other versions ship different checks,
-- silencing or adding diagnostics relative to CI.
local TOOL = toolchain.pinned_tool("clang-tidy", "23", { "22", "21" })

local USAGE = [[
Usage: lua scripts/tidy.lua [OPTIONS]

Runs clang-tidy against .clang-tidy on the app and core sources, using the
compile_commands.json of a CMake preset's build directory.

Options:
  -p, --preset PRESET   CMake preset whose build dir to use (default: debug).
                        It must be configured and built, since generated
                        moc files are part of the analysed sources.
      --filter PATTERN  Only analyse files whose path matches PATTERN
  -j, --jobs N          Files analysed in parallel (default: number of CPUs;
                        1 with --fix)
  -f, --fix             Apply clang-tidy suggested fixes in-place
  -v, --verbose         Show full clang-tidy output even for passing files
  -h, --help            Print this message and exit

Exit codes:
  0   All files passed clang-tidy
  1   One or more files produced diagnostics
]]

local opts = cli.parse(USAGE, {
    { "-p", "--preset", "preset", true },
    { nil, "--filter", "filter", true },
    { "-j", "--jobs", "jobs", true },
    { "-f", "--fix", "fix" },
    { "-v", "--verbose", "verbose" },
}, { preset = "debug", jobs = shell.capture("nproc 2>/dev/null") })

local BUILD_DIR = ROOT .. "/build/" .. opts.preset
local DB = BUILD_DIR .. "/compile_commands.json"

local db = io.open(DB) or cli.die("%s not found\n  Configure and build first: cmake --preset %s && cmake --build --preset %s",
    project.short_path(DB), opts.preset, opts.preset)
local content = db:read("*a")
db:close()

-- Only project sources; generated moc/autogen files live in the build tree.
local files, seen = {}, {}
for file in content:gmatch('"file"%s*:%s*"([^"]+)"') do
    local p = project.short_path(file)
    if (p:match("^app/") or p:match("^core/") or p:match("^gui/")) and not seen[file] then
        seen[file] = true
        files[#files + 1] = file
    end
end
files = project.select(files, opts.filter)
table.sort(files)

io.write(string.format("%s── CAN Log Visualizer clang-tidy ──%s  (%s)\nBuild dir: %s%s%s\n\n",
    C.bold, C.reset, TOOL, C.cyan, project.short_path(BUILD_DIR), C.reset))

local function indent(text)
    if text:match("%S") then
        io.write((text:gsub("[^\n]+", "  %0")), "\n")
    end
end

local n, failed, t_total = #files, 0, time.now()

-- Parsing Qt headers dominates the run time, so keep `jobs` clang-tidy
-- processes in flight and collect their output in file order.
-- --fix runs serially because files share headers.
local jobs = opts.fix and 1 or math.max(1, math.tointeger(tonumber(opts.jobs)) or 1)
local running = {}

local function launch(i)
    running[i] = {
        started = time.now(),
        pipe = io.popen(string.format("%s -p %s %s%s 2>&1",
            TOOL, shell.quote(BUILD_DIR), opts.fix and "--fix " or "", shell.quote(files[i]))),
    }
end

for i = 1, math.min(jobs, n) do launch(i) end
for i, path in ipairs(files) do
    local name = project.short_path(path)
    progress.status(i, n, name, "analysing...")

    local job = running[i]
    local output = job.pipe:read("*a")
    job.pipe:close()
    running[i] = nil
    if i + jobs <= n then launch(i + jobs) end

    -- Exit codes are not reliable across versions, so detect diagnostics by output.
    local ok = not (output:find(": error:", 1, true) or output:find(": warning:", 1, true))

    progress.status_done(i, n, name, ok, time.now() - job.started)
    if not ok then failed = failed + 1 end
    if not ok or opts.verbose then indent(output) end
end

local sep = string.rep("─", 52)
io.write(string.format("\n%s\n Total time: %s\n ", sep, time.fmt_elapsed(time.now() - t_total)))
if failed == 0 then
    io.write(string.format("%sAll %d files passed clang-tidy%s\n", C.green, n, C.reset))
else
    io.write(string.format("%s%d of %d files have diagnostics%s\n", C.red, failed, n, C.reset))
end
io.write(sep .. "\n")

os.exit(failed == 0 and 0 or 1)
