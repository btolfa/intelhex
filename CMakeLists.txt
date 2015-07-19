# Test for minimum required CMake version 2.8.12
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# Project description and (meta) information

set(META_PROJECT_NAME        "intelhex")
set(META_PROJECT_DESCRIPTION "Class to decode, encode and manipulate Intel HEX files")
set(META_VERSION_MAJOR       "1")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_AUTHOR_DOMAIN       "https://github.com/btolfa/intelhex/")
set(META_AUTHOR_MAINTAINER   "btolfa@gmail.com")

string(TOUPPER ${META_PROJECT_NAME} META_PROJECT_NAME_UPPER)

# Set project name and type (C/C++)
project(${META_PROJECT_NAME})

# Configuration options
option(OPTION_BUILD_TESTS "Build tests." ON)

find_package(Boost REQUIRED)
include_directories(${BOOST_INCLUDEDIRS})
list(APPEND CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")

if(OPTION_BUILD_TESTS)
    enable_testing()

    # Enable ExternalProject CMake module
    include(ExternalProject)

    # Set default ExternalProject root directory
    set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

    # Add gmock
    ExternalProject_Add(
        googlemock
        SVN_REPOSITORY http://googlemock.googlecode.com/svn/trunk/
        SVN_REVISION -r 535
        TIMEOUT 10
        UPDATE_COMMAND ""
        # Force separate output paths for debug and release builds to allow easy
        # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
                   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
                   -Dgtest_disable_pthreads:BOOL=ON
        # Disable install step
        INSTALL_COMMAND ""
        # Wrap download, configure and build steps in a script to log output
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON)

    # Specify include dir
    ExternalProject_Get_Property(googlemock source_dir)
    set(GMOCK_INCLUDE_DIRS ${source_dir}/include)
    set(GTEST_INCLUDE_DIRS ${source_dir}/gtest/include)

    set(CMAKE_FIND_LIBRARY_PREFIXES lib)
    # Specify link libraries
    ExternalProject_Get_Property(googlemock binary_dir)
    set (GMOCK_BOTH_LIBRARIES
        debug ${binary_dir}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a
        debug ${binary_dir}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a
        optimized ${binary_dir}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a
        optimized ${binary_dir}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a)
    set (GTEST_BOTH_LIBRARIES
        debug ${binary_dir}/gtest/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a
        debug ${binary_dir}/gtest/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a
        optimized ${binary_dir}/gtest/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a
        optimized ${binary_dir}/gtest/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a)

    set(TEST_SOURCES tests/intelhexTest.cpp intelhex.hpp IntelHexErrors.hpp)

    include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} .)

    add_executable(intelhex-test ${TEST_SOURCES})
    add_dependencies(intelhex-test googlemock)
    target_link_libraries(intelhex-test ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})

    add_test(NAME intelhex-test COMMAND intelhex-test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()