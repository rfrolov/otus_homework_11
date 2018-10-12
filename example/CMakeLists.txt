cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE SOURCES *.cpp *.h)

set(TARGET_NAME example)

add_executable(${TARGET_NAME} ${SOURCES})

set_target_properties(
        ${TARGET_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT} async)