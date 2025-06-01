cmake_minimum_required(VERSION 3.20)
project(testdemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)

# my project
add_library(stringify STATIC stringify.cpp)

enable_testing()
add_executable(tests tests.cpp)
# specific to my OSX setup
include_directories(tests PRIVATE /opt/homebrew/include/)

target_link_libraries(tests PRIVATE stringify GTest::gtest GTest::gtest_main)
gtest_discover_tests(tests)

add_executable(tests_with_mocks tests_with_mocks.cpp)
target_include_directories(tests_with_mocks PRIVATE /opt/homebrew/include/)
target_link_directories(tests_with_mocks PRIVATE /opt/homebrew/lib)
target_link_libraries(tests_with_mocks PRIVATE stringify GTest::gtest GTest::gtest_main GTest::gmock)
gtest_discover_tests(tests_with_mocks)

add_executable(dry dry.cpp)
target_link_libraries(dry PRIVATE GTest::gtest GTest::gtest_main)
add_executable(dry_parameterized dry_parameterized.cpp)
target_link_libraries(dry_parameterized PRIVATE GTest::gtest GTest::gtest_main)
add_executable(custom custom.cpp)
target_link_libraries(custom PRIVATE /opt/homebrew/lib/libfmt.a GTest::gtest GTest::gtest_main)
add_executable(mocking mocking.cpp)
target_link_libraries(mocking PRIVATE GTest::gtest GTest::gmock GTest::gtest_main)
