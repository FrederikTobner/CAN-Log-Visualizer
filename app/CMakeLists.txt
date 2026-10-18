add_subdirectory(core)

qt_add_executable(CanLogVisualizer
    main.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
)

target_link_libraries(CanLogVisualizer PRIVATE CanLogCore Qt6::Widgets)
target_compile_options(CanLogVisualizer PRIVATE -Wall -Wextra -Wpedantic)
