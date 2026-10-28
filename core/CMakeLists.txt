add_library(CanLogCore STATIC)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE CORE_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(CanLogCore PRIVATE ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(CanLogCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CanLogCore PUBLIC Qt6::Core)
target_compile_options(CanLogCore PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
