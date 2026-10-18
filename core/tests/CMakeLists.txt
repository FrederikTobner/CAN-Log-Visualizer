find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

# Every *Test.cpp becomes its own test executable and CTest test.
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*Test.cpp)

foreach(test_source IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE CanLogCore Qt6::Test)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
