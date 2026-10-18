qt_add_executable(CanLogVisualizer
    main.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
)

target_include_directories(CanLogVisualizer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CanLogVisualizer PRIVATE Qt6::Widgets)
target_compile_options(CanLogVisualizer PRIVATE -Wall -Wextra -Wpedantic)
