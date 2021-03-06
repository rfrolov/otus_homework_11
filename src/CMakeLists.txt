cmake_minimum_required(VERSION 3.6)

project(async VERSION 1.0.$ENV{TRAVIS_BUILD_NUMBER})

file(GLOB_RECURSE SOURCES *.cpp *.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} )

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES async.h DESTINATION include)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "rfrolov")
set(CPACK_PACKAGE_CONTACT rfrolov@engineer.com)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Otus homework 11")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/../README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../LICENSE")
set(CPACK_PACKAGE_VERSION_BUILD $ENV{TRAVIS_BUILD_NUMBER})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

include(CPack)