cmake_minimum_required(VERSION 3.25)

project(CanLogVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

include(CTest)

# Registers every *Test.cpp below the current directory as its own test
# executable and CTest test. ENVIRONMENT entries are set on each CTest test.
function(add_qt_tests)
    cmake_parse_arguments(ARG "" "" "LIBRARIES;ENVIRONMENT" ${ARGN})
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

    file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*Test.cpp)
    foreach(test_source IN LISTS test_sources)
        get_filename_component(test_name ${test_source} NAME_WE)
        add_executable(${test_name} ${test_source})
        target_link_libraries(${test_name} PRIVATE ${ARG_LIBRARIES} Qt6::Test)
        add_test(NAME ${test_name} COMMAND ${test_name})
        if(ARG_ENVIRONMENT)
            set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "${ARG_ENVIRONMENT}")
        endif()
    endforeach()
endfunction()

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(app)
