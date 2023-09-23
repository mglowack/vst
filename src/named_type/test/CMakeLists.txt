cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

add_executable(
    test_named_type
    test_named_type.cpp)
target_link_libraries(
    test_named_type PRIVATE
    gmock_main
    Boost::multi_index
    named_type
)
gtest_discover_tests(test_named_type)