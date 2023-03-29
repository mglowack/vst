cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

find_package(Boost COMPONENTS container REQUIRED)

add_executable(
    vst_test
    test_vst.cpp)
target_link_libraries(
    vst_test PRIVATE
    gmock_main
    ${Boost_LIBRARIES}
    vst
)
gtest_discover_tests(vst_test)