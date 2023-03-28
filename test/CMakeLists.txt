cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

find_package(Boost COMPONENTS container REQUIRED)

add_executable(
    test
    test_vst.cpp
    test_named_type.cpp)
target_link_libraries(
    test PRIVATE
    gmock_main
    ${Boost_LIBRARIES}
    vst
)
gtest_discover_tests(test)