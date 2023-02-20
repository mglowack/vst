cmake_minimum_required(VERSION 3.25)

find_package(Boost COMPONENTS container REQUIRED)

add_library (vst vst.hpp named_type.hpp)
set_target_properties(vst PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(vst PUBLIC .)
target_compile_options(vst PUBLIC -Wall -Wextra -Werror -Wno-missing-braces)
target_link_libraries(vst PUBLIC ${Boost_LIBRARIES})
