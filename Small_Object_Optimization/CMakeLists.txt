# Requires CMake 3.2 or higher
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(smapp VERSION 0.1 LANGUAGES CXX)

# Setup testing using GTEST
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(extern/googletest)

#
# Build targets
#

add_executable(tests    test.cpp)
add_test(tests tests)

#
# Compiler and linker options
#

# Link test executable against gtest & gtest_main
target_link_libraries(tests gtest_main gmock)

# the default for ctest is very short... also the dependency to re-build tests is missing
add_custom_target(runtest COMMAND ./tests${CMAKE_EXECUTABLE_SUFFIX})
add_dependencies(runtest tests)

#set_property(TARGET tests           PROPERTY CXX_STANDARD 11)
#set_property(TARGET tests           PROPERTY CXX_STANDARD_REQUIRED ON)

# We want a lot of warnings!
# (see https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md)
if(MSVC)
    SET(PROJ_WARNINGS /W4 /WX
                        /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545
                        /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906
                        /w14928)
    target_compile_options(tests PRIVATE ${PROJ_WARNINGS})
    
    # Prevent deprecation errors for std::tr1 in googletest
    target_compile_options(tests PRIVATE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
else()
    SET(PROJ_WARNINGS -Wall -Werror -Wextra -Wshadow -Wold-style-cast -Wcast-align -Wunused
                        -Wpedantic -Wconversion -Wsign-conversion -Wformat=2)
    target_compile_options(tests PRIVATE ${PROJ_WARNINGS})
endif()


option(ENABLE_ASAN "Enable address sanitizer instrumentation" OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fsanitize=address")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

option(ENABLE_TSAN "Enable thread sanitizer instrumentation" OFF)

if(ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fsanitize=thread")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=thread")
endif()

option(ENABLE_UBSAN "Enable UB sanitizer instrumentation" OFF)

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fsanitize=undefined")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=undefined")
endif()


# clang tidy support (using version 6.0 atm)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(NOT DEFINED CLANG_TIDY)
    set(CLANG_TIDY clang-tidy-6.0)
endif()
list(APPEND CLANG_TIDY_ARGS
    -p .
    ${CMAKE_SOURCE_DIR}/test/crasher.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_custom_target(
    tidy
    COMMAND ${CLANG_TIDY} ${CLANG_TIDY_ARGS}
    COMMENT "running clang tidy"
)
