#
# Boost.Process - example code
#
# Requires CMake 3.2 or higher
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(boost_process VERSION 0.1 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Release)

#
# Build targets
#

add_executable(sample sample.cpp)

if (MSVC)
    set (Boost_DETAILED_FAILURE_MSG ON)
    set (Boost_THREADAPI win32)
    set (BOOST_ROOT "d:/local/boost_1_69_0")
    set (BOOST_LIBRARYDIR  "d:/local/boost_1_69_0/lib64-msvc-14.1")
    #set (Boost_COMPILER "-vc141")
    #set (Boost_USE_STATIC_RUNTIME ON)  
    #set (BOOST_DEBUG ON)
endif()

# Find the Boost library
# we need 'system', but also 'filesystem' on Windows
find_package(Boost 1.62 REQUIRED COMPONENTS filesystem system)
target_link_libraries(sample Boost::filesystem Boost::system)
target_include_directories(sample PRIVATE ${Boost_INCLUDE_DIRS})
target_compile_definitions(sample PRIVATE BOOST_ALL_DYN_LINK)

#
# Compiler and linker options
#

#set_property(TARGET sample PROPERTY CXX_STANDARD 11)
#set_property(TARGET sample PROPERTY CXX_STANDARD_REQUIRED ON)
set_target_properties(sample PROPERTIES CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# We want a lot of warnings!
if (MSVC)
    target_compile_options(sample PRIVATE /W4)
else ()
    target_compile_options(sample PRIVATE -Wall -Werror)
    target_link_libraries(sample pthread)
endif()
