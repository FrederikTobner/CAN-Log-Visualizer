add_library(CanLogCore STATIC
    CanFrame.cpp
    CanFrame.h
    CanSource.h
    CanTraceModel.cpp
    CanTraceModel.h
    KernelFrame.cpp
    KernelFrame.h
    SocketCanSource.cpp
    SocketCanSource.h
)

target_include_directories(CanLogCore PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(CanLogCore PUBLIC Qt6::Core)
target_compile_options(CanLogCore PRIVATE -Wall -Wextra -Wpedantic)
