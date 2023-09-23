cmake_minimum_required(VERSION 3.25)
project(vst)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(${project}_BUILD_TESTS "whether or not tests should be built" ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Werror -fuse-ld=mold)

include(FetchContent)
FetchContent_Declare(
    cmake-deps
    GIT_REPOSITORY https://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@github.com/mglowack/cmake-deps.git
)
FetchContent_MakeAvailable(cmake-deps)

include(deps.cmake)
add_subdirectory(src/dev)
add_subdirectory(src/vst)
add_subdirectory(src/named_type)

if(${project}_BUILD_TESTS)
    include(test-deps.cmake)
    add_subdirectory(src/dev/test)
    add_subdirectory(src/vst/test)
    add_subdirectory(src/named_type/test)
endif()

enable_testing()