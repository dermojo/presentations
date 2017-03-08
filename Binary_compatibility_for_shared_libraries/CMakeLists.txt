#
# Binary compatibility for shared libraries - example code
#
# Requires CMake 3.2 or higher
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(bincompat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

#
# Build targets
#

add_library(foo SHARED foo.cpp)

add_executable(foo_client foo_client.cpp)

include_directories(.)
target_link_libraries(foo_client foo)

#
# Compiler and linker options
#

# enable C++ 11 - not really required...
set_property(TARGET foo PROPERTY CXX_STANDARD 11)
set_property(TARGET foo PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET foo_client PROPERTY CXX_STANDARD 11)
set_property(TARGET foo_client PROPERTY CXX_STANDARD_REQUIRED ON)

# We want a lot of warnings!
target_compile_options(foo PUBLIC -Wall -Werror)
#target_compile_options(foo PRIVATE -fvisibility=hidden)
target_compile_options(foo_client PUBLIC -Wall -Werror)
#target_compile_options(foo_client PRIVATE -fvisibility=hidden)
