# Registers every *Test.cpp below the current directory as its own test
# executable and CTest test. ENVIRONMENT entries are set on each CTest test.
# Expects the following arguments:
#   LIBRARIES: Libraries to link with the test executables.
#   ENVIRONMENT: Environment variables to set for the tests.
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

