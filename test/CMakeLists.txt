cmake_minimum_required(VERSION 3.25)

# find_package(GTest)
# if(GTEST_FOUND)
#     add_executable(test test_vst.cpp test_named_type.cpp)
#     target_link_libraries(test PRIVATE
#         ${GTEST_BOTH_LIBRARIES}
#         vst
#     )

#     target_include_directories(test PUBLIC
#         ${GTEST_INCLUDE_DIRS}
#     )
#     target_compile_options(test PUBLIC -Wall -Wextra -Werror -Wno-missing-braces)
# endif()

find_package(Boost COMPONENTS container REQUIRED)

add_executable(test 
    test_vst.cpp
    test_named_type.cpp)
target_link_libraries(test PRIVATE
    gmock_main
    ${Boost_LIBRARIES}
    vst
)