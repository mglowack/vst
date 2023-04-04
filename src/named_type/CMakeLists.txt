cmake_minimum_required(VERSION 3.25)

# add_library (vst INTERFACE vst_wrapped_value.cpp vst_named_var.cpp vst_indexed_var.cpp)
add_library (
    named_type OBJECT
    named_type.hpp
    named_type_ops_category.cpp
    named_type_pod.cpp
    named_type_trait.cpp
    named_type_defs.cpp
    named_type_impl.cpp)
set_target_properties(named_type PROPERTIES LINKER_LANGUAGE CXX)
# target_include_directories(vst
    # INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> ${Boost_INCLUDE_DIRS})
target_include_directories(named_type PUBLIC . vst)
target_link_libraries(named_type PUBLIC vst)
