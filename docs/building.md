# Building and testing

## Requirements

- Linux (SocketCAN)
- A C++23 compiler; the presets use Clang
- CMake and Ninja
- Qt 6 with the Widgets and Test modules (minimum version: see [CMakeLists.txt](../CMakeLists.txt))
- Optional: `can-utils` to generate test traffic, `clang-format` and Lua to run the format script

[.github/workflows/ci.yml](../.github/workflows/ci.yml) shows a complete, working setup.

## Build

```
cmake --preset debug
cmake --build --preset debug
```

Use `release` instead of `debug` for an optimized build. Binaries end up in `build/<preset>/`; the application is `build/<preset>/app/CanLogVisualizer`.

## Test

```
ctest --preset debug
```

Tests that need a virtual CAN interface are skipped when it is not available; see [usage](usage.md) for setting one up.

## Formatting

```
lua scripts/format.lua          # check
lua scripts/format.lua --fix    # apply
```

The script uses the version of `clang-format` that CI is pinned to when it is installed, and warns otherwise.

## Editor setup

`.clangd` reads `build/debug/compile_commands.json`, so configure the `debug` preset once before using clangd. The presets work the same way from VS Code (CMake Tools) and from the command line.
