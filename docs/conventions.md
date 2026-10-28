# Coding conventions

Most of these are enforced by [.clang-format](../.clang-format) and [.clang-tidy](../.clang-tidy).
You can run `lua5.4 scripts/format.lua` and `lua5.4 scripts/tidy.lua` (see [building](building.md)) to simplify the process.

## Files

- C++ headers use the `.hpp` file extension
- Sources files on the other hand use the `.cpp` file extension
- Include order, separated by blank lines: own header, project headers, Qt headers, standard library headers.
- Sources are found with `file(GLOB_RECURSE ... CONFIGURE_DEPENDS)`, so new files need no CMake change.

## Documentation

- Every header is documented using dox comments (excluding tests)

## Naming

| Kind                                       | Style        | Example                         |
| ------------------------------------------ | ------------ | ------------------------------- |
| Types, enums, enum constants               | `CamelCase`  | `CanFrame`, `Column::CycleMax`  |
| Functions, methods, variables, parameters  | `camelBack`  | `maxFrameBits`, `windowSeconds` |
| Private and protected members              | `m_` prefix  | `m_totalFrames`                 |
| Constants                                  | `k` prefix   | `kRefreshIntervalMs`            |
| Namespaces                                 | `lower_case` | `parsing`                       |

## Language

- C++23. Prefer standard types (`std::expected`, `std::optional`, `std::span`, ranges) over home-made ones.
- East const and middle allignment for pointers (type * var_name)

## Tests

- Qt Test only, one executable per `*Test.cpp`, registered through the `add_qt_tests` function defined in (`cmake/Add_QT_Test.cmake`) and run by CTest.
- Name test slots after the behaviour (`rateDecaysWhenAdvanced`), use `_data()` functions for tables, and `QAbstractItemModelTester` for every model.
- Shared frame builders live in `core/tests/TestFrames.hpp`. Tests that need `vcan0` fail when it is missing; see [usage](usage.md).
