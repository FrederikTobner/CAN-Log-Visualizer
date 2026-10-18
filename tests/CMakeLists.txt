find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

function(add_can_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE CanLogCore Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_can_test(CanFrameTest)
add_can_test(CanTraceModelTest)
add_can_test(SocketCanSourceTest)
