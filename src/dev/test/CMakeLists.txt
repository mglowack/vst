cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

add_executable(
    test_dev
    test_tuple.cpp
)
target_link_libraries(
    test_dev PRIVATE
    gmock_main
    dev
)
gtest_discover_tests(test_dev)
# add_test(NAME test_dev COMMAND test_dev)