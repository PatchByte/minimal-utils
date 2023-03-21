cmake_minimum_required(VERSION 3.24)
project("MinimalUtilsBuild")

option(MINIMALUTILS_AS_VENDOR "Use this project as a vendor." OFF)
option(MINIMALUTILS_BUILD_TESTS "Build tests of this project." ON)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("MinimalUtils")

if(NOT MINIMALUTILS_AS_VENDOR)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(NOT MINIMALUTILS_AS_VENDOR)

if(MINIMALUTILS_BUILD_TESTS)
    # @todo
endif(MINIMALUTILS_BUILD_TESTS)