cmake_minimum_required(VERSION 3.2)

add_subdirectory(googletest EXCLUDE_FROM_ALL)

add_executable(tests test_print.cpp test_command_handler.cpp ../src/bulk/print.cpp)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

set_target_properties(
        tests PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(tests gtest gtest_main)

enable_testing()