cmake_minimum_required(VERSION 3.25)
include(CTest)
include(GoogleTest)

find_package(Boost COMPONENTS container REQUIRED)

add_executable(
    test_vst
    test_vst.cpp
    test_vst_manual.cpp
    test_vst_defs_ops.cpp
    test_vst_defs_fields.cpp
    test_vst_defs_with_fields.cpp
)
target_link_libraries(
    test_vst PRIVATE
    gmock_main
    ${Boost_LIBRARIES}
    vst
    dev
)
gtest_discover_tests(test_vst)
# add_test(NAME test_vst COMMAND test_vst)