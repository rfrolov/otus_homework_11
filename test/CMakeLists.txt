cmake_minimum_required(VERSION 3.2)

add_subdirectory(googletest EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(tests ${SOURCES} ${CMAKE_SOURCE_DIR}/src/bulk/print.cpp)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

set_target_properties(
        tests PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(tests gtest gtest_main)

enable_testing()