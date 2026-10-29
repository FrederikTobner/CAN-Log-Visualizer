# CAN Log Visualizer

> 🚧 **Work in Progress**: This project is currently under development.

A small CAN log visualizer, written in C++ using Qt 6 Widgets and CMake.

It reads CAN frames directly from Linux SocketCAN interfaces and shows them in a desktop UI.

![CAN Visualizer GUI](images/CAN_Log_Visualizer.png)

## Features

* Log file support using the same format as candump
* Statistics for CAN frames grouped by ID
* Filtering using CAN IDs and masks
* In-app quick reference (F1) and tooltips

## Requirements

* Linux (SocketCAN)
* A C++23 compiler
* CMake
* Qt 6 with the Widgets and Test modules (minimum version: see [CMakeLists.txt](CMakeLists.txt))

## Contributing

* [Building](docs/building.md)
* [Coding conventions](docs/coding-conventions.md)
* [Manual testing](docs/manual-testing.md)

## License

This project is licensed under the [GNU GENERAL PUBLIC LICENSE Version 3](LICENSE)
