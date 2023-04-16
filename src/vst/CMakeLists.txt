cmake_minimum_required(VERSION 3.25)

find_package(Boost COMPONENTS container REQUIRED)

# add_library (vst INTERFACE vst_wrapped_value.cpp vst_named_var.cpp vst_indexed_var.cpp)
add_library (
    vst OBJECT
    vst.hpp
    vst_defs_ops.cpp
    vst_defs_with_fields.cpp
    vst_trait.cpp
    vst_wrapped_value.cpp
    vst_named_var.cpp
    vst_indexed_var.cpp
    vst_operators_hashable.cpp
    vst_operators_stream.cpp
    vst_operators_addable.cpp
    vst_operators_ordered.cpp
    vst_operators_comparable.cpp
    vst_named_field_ptr.cpp
)
set_target_properties(vst PROPERTIES LINKER_LANGUAGE CXX)
# target_include_directories(vst
    # INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${Boost_INCLUDE_DIRS})
target_include_directories(vst PUBLIC . ${Boost_INCLUDE_DIRS})
target_link_libraries(vst PUBLIC dev ${Boost_LIBRARIES})
