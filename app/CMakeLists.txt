# The GUI is a library so that tests can link it; main.cpp only wires it up.
qt_add_executable(CanLogVisualizer src/main.cpp)
target_include_directories(CanLogVisualizer PUBLIC ${PROJECT_SOURCE_DIR}/gui/include)
target_link_libraries(CanLogVisualizer PRIVATE CanLogGui)
target_compile_options(CanLogVisualizer PRIVATE -Wall -Wextra -Wpedantic)

