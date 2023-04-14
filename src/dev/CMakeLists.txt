cmake_minimum_required(VERSION 3.25)

# find_package(Boost COMPONENTS container REQUIRED)

# add_library (vst INTERFACE vst_wrapped_value.cpp vst_named_var.cpp vst_indexed_var.cpp)
add_library (
    dev OBJECT
    dev_concepts.cpp
    dev_stringify.cpp
    dev_type_list.cpp
    dev_type_traits.cpp
    dev_functional.cpp
)
set_target_properties(dev PROPERTIES LINKER_LANGUAGE CXX)
# target_include_directories(vst
#     INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${Boost_INCLUDE_DIRS})
# target_include_directories(dev
    # INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
# target_include_directories(dev PUBLIC . ${Boost_INCLUDE_DIRS})
target_include_directories(dev PUBLIC .)
# target_link_libraries(vst PUBLIC ${Boost_LIBRARIES})
