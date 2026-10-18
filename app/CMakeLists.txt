file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

qt_add_executable(CanLogVisualizer ${APP_SOURCES})

target_include_directories(CanLogVisualizer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(CanLogVisualizer PRIVATE CanLogCore Qt6::Widgets)
target_compile_options(CanLogVisualizer PRIVATE -Wall -Wextra -Wpedantic)
