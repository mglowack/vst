cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

add_executable(
    test_dev
    test_dev_tuple.cpp
    test_dev_type_list.cpp
    test_dev_type_traits.cpp
)
target_link_libraries(
    test_dev PRIVATE
    gmock_main
    dev
)
gtest_discover_tests(test_dev)