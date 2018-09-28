cmake_minimum_required(VERSION 3.6)

if (NOT DEFINED ENV{TRAVIS_BUILD_NUMBER})
    set(ENV{TRAVIS_BUILD_NUMBER} 1)
endif ()

find_package (Threads REQUIRED)

add_subdirectory(lib)
add_subdirectory(src)
#add_subdirectory(test)

enable_testing()
