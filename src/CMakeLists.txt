cmake_minimum_required(VERSION 3.25)

find_package(Boost COMPONENTS container REQUIRED)

add_library (vst INTERFACE)
set_target_properties(vst PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(vst 
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${Boost_INCLUDE_DIRS})
# target_link_libraries(vst PUBLIC ${Boost_LIBRARIES})
