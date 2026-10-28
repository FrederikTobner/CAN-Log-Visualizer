# Headers are listed so that AUTOMOC finds the Q_OBJECT classes.
file(GLOB_RECURSE GUI_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(CanLogGui STATIC ${GUI_SOURCES})
target_include_directories(CanLogGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(CanLogGui PUBLIC CanLogCore Qt6::Widgets)
target_compile_options(CanLogGui PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
