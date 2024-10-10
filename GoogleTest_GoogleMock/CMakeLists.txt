cmake_minimum_required(VERSION 3.20)
project(testdemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # just for fun

find_package(GTest REQUIRED)

# my project
add_library(stringify STATIC stringify.cpp)

enable_testing()
add_executable(tests tests.cpp)
# specific to my OSX setup
target_include_directories(tests PRIVATE /opt/homebrew/include/)
target_link_directories(tests PRIVATE /opt/homebrew/lib)

target_link_libraries(tests PRIVATE stringify gtest gtest_main)
gtest_discover_tests(tests)

add_executable(tests_with_mocks tests_with_mocks.cpp)
target_include_directories(tests_with_mocks PRIVATE /opt/homebrew/include/)
target_link_directories(tests_with_mocks PRIVATE /opt/homebrew/lib)
target_link_libraries(tests_with_mocks PRIVATE stringify gtest gtest_main gmock)
gtest_discover_tests(tests_with_mocks)
