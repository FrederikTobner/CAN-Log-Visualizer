# Building 

## Requirements

- Linux (SocketCAN)
- A C++23 compiler (the presets use Clang)
- CMake (the preset uses Ninja as the build system)
- Qt 6 with the Widgets and Test modules (minimum version: see [CMakeLists.txt](../CMakeLists.txt))
- Optional: `can-utils` to generate test traffic, `clang-format` and Lua to run the format script

## Build

To simplify the build process a preset can be used.

```
cmake --preset release 
cmake --build --preset release
```

If you would like to use a different build system or compiler, configure the project directly. The only requirement is a C++23 compiler:

```
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=g++
cmake --build build
ctest --test-dir build --output-on-failure
```

## Test

```
ctest --preset release
```

Some tests need a `vcan0` interface; see [manual testing](manual-testing.md).

## Layout

- `core/`: Qt-based library without widgets (frames, sources, models, statistics)
- `gui/`: widgets (main window, filter field, statistics panel)
- `app/`: the executable (`main.cpp`)
- `scripts/`: format, tidy and vcan helper scripts
