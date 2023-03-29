cmake_minimum_required(VERSION 3.25)
project(vst)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../deps/googletest-1.13.0 gtest)

add_compile_options(-Wall -Werror -fuse-ld=mold)

add_subdirectory(src/vst)
add_subdirectory(src/vst/test)