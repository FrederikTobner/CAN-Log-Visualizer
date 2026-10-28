# Building 

## Requirements

- Linux (SocketCAN)
- A C++23 compiler; the presets use Clang
- CMake and Ninja
- Qt 6 with the Widgets and Test modules (minimum version: see [CMakeLists.txt](../CMakeLists.txt))
- Optional: `can-utils` to generate test traffic, `clang-format` and Lua to run the format script

## Build

```
cmake --preset release 
cmake --build --preset release
```

Use `release` instead of `debug` for an optimized build. Binaries end up in `build/<preset>/`; the application is `build/<preset>/app/CanLogVisualizer`.
