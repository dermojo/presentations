#
# C++ scripting in Python - example code
#
# Currently, only Linux builds are supported.
# Requirements:
#   - QT5
#   - Pybind11 2.0.0 (extracted to ./pybind11-2.0.0)
#
# Requires CMake 3.2 or higher
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(cpp-scripting VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

#
# Build targets
#

add_executable(example
    src/example.cpp
    src/captured_stream.cpp
    src/resources.cpp
    )

# Find the QtWidgets library
find_package(Qt5Widgets)
target_link_libraries(example Qt5::Widgets)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(pybind11-2.0.0/include)
target_link_libraries(example ${PYTHON_LIBRARIES})

#
# Compiler and linker options
#

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(example PUBLIC include)

set_property(TARGET example PROPERTY CXX_STANDARD 11)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED ON)

add_custom_target(run COMMAND ./example)
add_dependencies(run example)

# We want a lot of warnings!
target_compile_options(example PUBLIC -Wall -Werror)
target_compile_options(example PRIVATE -fvisibility=hidden)
# These are enabled on Clang and don't help...
#target_compile_options(example PUBLIC -Wno-missing-braces)
target_link_libraries(example pthread)
