cmake_minimum_required(VERSION 3.25)
project(vst)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(${project}_BUILD_TESTS "whether or not tests should be built" ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Werror)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    add_compile_options(-fuse-ld=mold)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    add_compile_options(-Wno-unused-but-set-variable -Wno-missing-braces)
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

include(FetchContent)
FetchContent_Declare(
    cmake-deps
    GIT_REPOSITORY https://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@github.com/mglowack/cmake-deps.git
)
FetchContent_MakeAvailable(cmake-deps)

include(deps/project.cmake)
add_subdirectory(src/dev)
add_subdirectory(src/vst)
add_subdirectory(src/named_type)

if(${project}_BUILD_TESTS)
    include(deps/tests.cmake)
    add_subdirectory(src/dev/test)
    add_subdirectory(src/vst/test)
    add_subdirectory(src/named_type/test)
endif()

enable_testing()